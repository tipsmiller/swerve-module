#ifndef SWERVE_TESTING_SWERVEMODULE_H
#define SWERVE_TESTING_SWERVEMODULE_H


#include "VescCan.h"

class SwerveModule {
private:
    std::atomic<bool> active = true;
    std::thread *periodicThread;
    int calState = 0;
    bool calibrated = false;
public:
    const double steeringOffset = -4.0;
    VescCan steeringVesc;
    SwerveModule(int steeringId);
    ~SwerveModule();
    void begin();
    void homeSteering();
    void periodic(); // gets its own thread
};


#endif //SWERVE_TESTING_SWERVEMODULE_H
