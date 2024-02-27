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
            steeringVesc.setPosition(0.0);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void SwerveModule::homeSteering() {
    double calDuty = 0.2;
    float adcVolts = steeringVesc.status6.load().adc_1;
    //printf("adc value %1.2f\n", adcVolts);
    switch (calState) {
        case 0:
            steeringVesc.setDuty(calDuty);
            if (adcVolts < 0.5) {
                // keep rotating until we're past the sensor
                break;
            } else {
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
                steeringVesc.setDuty(0);
                float firstPos = steeringVesc.status4.load().pid_pos_now;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                // The motor will keep running for a little bit before stopping, so we have to wait and record where it
                // stops and then add that difference to the offset
                float secondPos = steeringVesc.status4.load().pid_pos_now;
                steeringVesc.setPositionOffset(steeringOffset + (firstPos - secondPos));
                printf(
                        "Finished calibrating %d with first offset %1.2f, second offset %1.2f, and steeringOffset %1.2f\n",
                        steeringVesc.id,
                        firstPos,
                        secondPos,
                        steeringOffset
                );
                calibrated = true;
                calState = 2;
            }
            break;
    }
}
