#ifndef SWERVE_TESTING_VESCCAN_H
#define SWERVE_TESTING_VESCCAN_H


#include <atomic>
#include "datatypes.h"
#include "CanSocket.h"


class VescCan {
private:
    std::thread *readLoopThread;
    CanSocket canSocket;
    void readLoop(); // to be spun off on a thread to read the can frames continuously
    void decodeStatus4(can_frame &frame);
    void decodeStatus6(can_frame &frame);
public:
    int id;
    std::atomic<can_status_msg_4> status4 {};
    std::atomic<can_status_msg_6> status6 {};
    std::atomic<bool> active = true;
    VescCan();
    void init(int id);
    ~VescCan();
    void setDuty(double duty);
    void setPosition(double degrees);
    void setPositionOffset(double degrees);
};


#endif //SWERVE_TESTING_VESCCAN_H
