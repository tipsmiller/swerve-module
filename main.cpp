#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <ctime>
#include <sys/time.h>
#include "Vesc/VescCan.h"
#include "SwerveModule/SwerveModule.h"
#include "Gamepad.h"

std::atomic<bool> active;
struct timespec ts {};

void printTime() {
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("Time: %10jd.%03ld\n", (intmax_t) ts.tv_sec, ts.tv_nsec / 1000000);
    fflush(stdout);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void mainLoop() {
    active = true;
    SwerveModule swerve1(0);  // putting this in this thread means it gets cleaned up by the scope closing when the thread ends
    swerve1.begin();
    Gamepad gamepad("/dev/input/event15");
    while(active) {
        auto leftStick = gamepad.getAxisAngleAndMagnitude(LX, LY, 0.1);
        if (leftStick.magnitude > 0) {
            swerve1.setAngle(leftStick.angle);
            swerve1.setVelocity(leftStick.magnitude);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    printf("exiting main loop thread\n");
}

int main() {
    printf("---PRESS ENTER TO EXIT---\n");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::thread mainLoopThread(mainLoop);


    getchar();
    active = false;
    if(mainLoopThread.joinable()) {
        mainLoopThread.join();
    }
    printf("---EXITING---\n");
    return 0;
}
