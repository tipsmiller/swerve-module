#include <thread>
#include "VescCan.h"
#include "buffer.h"


can_frame getFrameWithId(int vescId, CAN_PACKET_ID msgId) {
    can_frame frame {
        .can_id = vescId | ((uint32_t)msgId << 8) | CAN_EFF_FLAG,
    };
    return frame;
}


VescCan::VescCan() {
}

void VescCan::init(int canId) {
    id = canId;
    canSocket = CanSocket();

    can_filter filter {
            .can_id = (uint16_t)id,
            .can_mask = 0xff,
    };
    canSocket.addFilter(filter);
    readLoopThread = new std::thread([this]() {readLoop();});
}

VescCan::~VescCan() {
    active = false;
    if(readLoopThread != nullptr && readLoopThread->joinable()) {
        readLoopThread->join();
    }
}

void VescCan::setDuty(double duty) {
    can_frame frame = getFrameWithId(id, CAN_PACKET_SET_DUTY);
    int index = 0;
    buffer_append_int32(frame.data, (int32_t)(duty * 100000.0), &index);
    frame.len = index;
    canSocket.writeCanFrame(frame);
}

void VescCan::setPosition(double degrees) {
    can_frame frame = getFrameWithId(id, CAN_PACKET_SET_POS);
    int index = 0;
    buffer_append_int32(frame.data, (int32_t)(degrees * 1000000.0), &index);
    frame.len = index;
    canSocket.writeCanFrame(frame);
}

void VescCan::setPositionOffset(double degrees) {
    can_frame frame = getFrameWithId(id, CAN_PACKET_UPDATE_PID_POS_OFFSET);
    int index = 0;
    buffer_append_float32(frame.data, degrees, 1e4, &index);
    frame.len = index;
    canSocket.writeCanFrame(frame);
}

void VescCan::readLoop() {
    struct can_frame frame {};
    while(active) {
        frame = canSocket.readCanFrame();
        int packetId = ((int)frame.can_id >> 8) & (uint8_t)255;
        switch (packetId) {
            case CAN_PACKET_STATUS_4:
                decodeStatus4(frame);
                break;
            case CAN_PACKET_STATUS_6:
                decodeStatus6(frame);
                break;
            default:
                break;
        }
    }
}

void VescCan::decodeStatus4(can_frame &frame) {
    can_status_msg_4 result {};
    int index = 0;
    result.temp_fet = buffer_get_float16(frame.data, 10.0, &index);
    result.temp_motor = buffer_get_float16(frame.data, 10.0, &index);
    result.current_in = buffer_get_float16(frame.data, 10.0, &index);
    result.pid_pos_now = buffer_get_float16(frame.data, 50.0, &index);
    status4 = result;
}

void VescCan::decodeStatus6(can_frame &frame) {
    can_status_msg_6 result {};
    int index = 0;
    result.adc_1 = buffer_get_float16(frame.data, 1000.0, &index);
    result.adc_2 = buffer_get_float16(frame.data, 1000.0, &index);
    result.adc_3 = buffer_get_float16(frame.data, 1000.0, &index);
    result.ppm = buffer_get_float16(frame.data, 1000.0, &index);
    status6 = result;
}
