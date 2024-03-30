#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>
#include <ctime>
#include <sys/time.h>
#include <vector>
#include "Vesc/VescCan.h"
#include "SwerveModule/SwerveModule.h"
#include "Gamepad.h"
#include "SwerveKinematics.h"

std::atomic<bool> active;
struct timespec ts{};
int ids[4][2] = {
        {0, 1},
        {2, 3},
        {4, 5},
        {6, 7},
};
float initialAngles[4] = {0.0, 90.0, 180.0, 270.0};
std::vector<SWERVE_MODULE_DESCRIPTOR> moduleDescriptors = {
        // TODO: Setting these to their actual values leads to very small W values. Need to scale that somehow.
//        {.Mx = -0.0762, .My = -0.0762, .maxVelocity = 1.0},
//        {.Mx = 0.0762, .My = -0.0762, .maxVelocity = 1.0},
//        {.Mx = 0.0762, .My = 0.0762, .maxVelocity = 1.0},
//        {.Mx = -0.0762, .My = 0.0762, .maxVelocity = 1.0},
        {.Mx = -1.0, .My = -1.0, .maxVelocity = 1.0},
        {.Mx = 1.0, .My = -1.0, .maxVelocity = 1.0},
        {.Mx = 1.0, .My = 1.0, .maxVelocity = 1.0},
        {.Mx = -1.0, .My = 1.0, .maxVelocity = 1.0},
};

void printTime() {
    clock_gettime(CLOCK_REALTIME, &ts);
    printf("Time: %10jd.%03ld\n", (intmax_t) ts.tv_sec, ts.tv_nsec / 1000000);
    fflush(stdout);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void mainLoop() {
    active = true;
    Gamepad gamepad("/dev/input/event15");
    SwerveModule modules[4] = {
            SwerveModule(ids[0][0], ids[0][1], initialAngles[0]),
            SwerveModule(ids[1][0], ids[1][1], initialAngles[1]),
            SwerveModule(ids[2][0], ids[2][1], initialAngles[2]),
            SwerveModule(ids[3][0], ids[3][1], initialAngles[3]),
    };
    for (auto &module: modules) {
        module.begin();
    }
    while (active) {
        auto leftStick = gamepad.getAxisAngleAndMagnitude(LX, LY, 0.2);
        auto rightStick = gamepad.getAxisAngleAndMagnitude(RX, RY, 0.2);
        double vX = std::cos(leftStick.angle*M_PI/180) * leftStick.magnitude;
        double vY = std::sin(leftStick.angle*M_PI/180) * leftStick.magnitude;
        double W = std::cos(rightStick.angle*M_PI/180) * rightStick.magnitude;
        auto commands = getSwerveCommands(vX, vY, W, 0.0, moduleDescriptors);
        for (int i=0; i<4; i++) {
            double angle = commands[i].angle * 180/M_PI;
            modules[i].setAngle(angle);
            modules[i].setVelocity(commands[i].velocity);
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
    if (mainLoopThread.joinable()) {
        mainLoopThread.join();
    }
    printf("---EXITING---\n");
    return 0;
}
