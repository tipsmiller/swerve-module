#include <thread>
#include "SwerveModule.h"


SwerveModule::SwerveModule(int steeringId) {
    steeringVesc.init(steeringId);
}

void SwerveModule::begin() {
    periodicThread = new std::thread([this] () {periodic();});
}

SwerveModule::~SwerveModule() {
    active = false;
    if(periodicThread != nullptr && periodicThread->joinable()) {
        periodicThread->join();
    }
    steeringVesc.setDuty(0);
}

void SwerveModule::periodic() {
    while(active) {
        if(!calibrated) {
            homeSteering();
        } else {
            steeringVesc.setPosition(angleSetpoint);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void SwerveModule::homeSteering() {
    double calDuty = 0.05;
    float adcVolts = steeringVesc.status6.load().adc_1;
    //printf("adc value %1.2f\n", adcVolts);
    switch (calState) {
        case 0:
            steeringVesc.setDuty(calDuty);
            if (adcVolts >= 0.5) {
                // we've passed the sensor
                calState = 1;
            }
            break;
        case 1:
            if (adcVolts > 0.5) {
                // keep rotating until we reach the sensor
                steeringVesc.setDuty(calDuty);
            } else {
                // stop
//                steeringVesc.setDuty(0);
//                float firstPos = steeringVesc.status4.load().pid_pos_now;
                // The motor will keep running for a little bit before stopping, so we have to wait and record where it
                // stops and then add that difference to the offset
//                float secondPos = steeringVesc.status4.load().pid_pos_now;
                steeringVesc.setPositionOffset(steeringOffset);
                printf(
                        "Finished calibrating %d with steeringOffset %1.2f\n",
                        steeringVesc.id,
                        steeringOffset
                );
                angleSetpoint = 0.0;
                calibrated = true;
                calState = 2;
            }
            break;
    }
}

void SwerveModule::setAngle(double degrees) {
    angleSetpoint = degrees;
}

void SwerveModule::setVelocity(double velocity) {
    velocitySetpoint = velocity;
}
