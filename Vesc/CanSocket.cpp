#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can/raw.h>
#include "CanSocket.h"


CanSocket::CanSocket() {
    socketFd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (socketFd < 0) {
        perror("Error while creating socket");
        throw;
    }

    strcpy(ifr.ifr_name, "can0" );
    ioctl(socketFd, SIOCGIFINDEX, &ifr);

    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if(bind(socketFd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error in socket bind");
        throw;
    }
}

void CanSocket::addFilter(can_filter &filter) {
    int optsResponse = setsockopt(socketFd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));
    if (optsResponse != 0) {
        perror("Error setting socket options");
        throw;
    }
}

can_frame CanSocket::readCanFrame() {
    struct can_frame frame{};
    long bytesRead = recvfrom(socketFd, &frame, sizeof(struct can_frame), 0, (struct sockaddr *)(&addr), &len);
    if (bytesRead == 0) {
        perror("no frame read");
    }
    return frame;
}

long CanSocket::writeCanFrame(can_frame &frame) {
    long bytesWritten = sendto(socketFd, &frame, sizeof(struct can_frame), 0, (struct sockaddr *)(&addr), len);
    return bytesWritten;
}

