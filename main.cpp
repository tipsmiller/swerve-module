#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <ctime>
#include <sys/time.h>
#include "Vesc/VescCan.h"
#include "SwerveModule/SwerveModule.h"

std::atomic<bool> active;
struct timespec ts {};
SwerveModule swerve1(0);

void printTime() {
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("Time: %10jd.%03ld\n", (intmax_t) ts.tv_sec, ts.tv_nsec / 1000000);
    fflush(stdout);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void mainLoop() {
    active = true;
    swerve1.begin(); // putting this in this thread means it gets cleaned up by the scope closing when the thread ends
    while(active) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
