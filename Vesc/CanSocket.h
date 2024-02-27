#ifndef SWERVE_TESTING_CANSOCKET_H
#define SWERVE_TESTING_CANSOCKET_H

#include <linux/can.h>
#include <unistd.h>
#include <net/if.h>


class CanSocket{
private:
    int socketFd;
    struct sockaddr_can addr;
    socklen_t len = sizeof(addr);
    ifreq ifr;
public:
    CanSocket();
    can_frame readCanFrame();
    long writeCanFrame(can_frame &frame);
    void addFilter(can_filter &filter);
};


#endif //SWERVE_TESTING_CANSOCKET_H
