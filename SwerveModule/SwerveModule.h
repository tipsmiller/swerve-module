#ifndef SWERVE_TESTING_SWERVEMODULE_H
#define SWERVE_TESTING_SWERVEMODULE_H


#include "VescCan.h"

class SwerveModule {
private:
    std::atomic<bool> active = true;
    std::thread *periodicThread;
    int calState = 0;
    bool calibrated = false;
    std::atomic<double> angleSetpoint = 0.0;
    std::atomic<double> velocitySetpoint = 0.0;
public:
    const double steeringOffset = -10.0;
    VescCan steeringVesc;
    SwerveModule(int steeringId);
    ~SwerveModule();
    void begin();
    void homeSteering();
    void periodic(); // gets its own thread
    void setAngle(double degrees);
    void setVelocity(double velocity);
};


#endif //SWERVE_TESTING_SWERVEMODULE_H
