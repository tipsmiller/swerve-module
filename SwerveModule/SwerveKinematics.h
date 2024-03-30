#ifndef SWERVE_TESTING_SWERVEKINEMATICS_H
#define SWERVE_TESTING_SWERVEKINEMATICS_H

/*
 *  This file contains math needed to convert translation and rotation commands (from a game pad etc.)
 *  into the angles and velocities for each swerve module.
 *  Each swerve module must have an initial Mx and My position relative to robot 0,0.
 *
 *  When a command is given (Vx, Vy, W) the translational components can be moved to robot-relative components by providing
 *  the current angle of the robot (thetaRobot). If thetaRobot is excluded, the robot will drive like an RC car.
 *  If thetaRobot is included, the robot will drive relative to the global environment (field-oriented control).
 *
 *  The output for a swerve module is a velocity vector. The vector is composed of X and Y components,
 *  which are equivalent to the Vx and Vy inputs after moving to robot-relative space. The angular velocity command
 *  of the robot (W) must be converted to a tangential velocity for the swerve module to follow. This is done by
 *  cross-multiplying the module position with the angular velocity:
 *  tangential velocity [Vx, Vy] = module position [Mx, My, 0] X angular velocity [0, 0, W]
 *  tangential velocity = [My*W, -Mx*W]
 *
 *  The total output for the module should be translational [Vx, Vy] + rotational [My*W, -Mx*W], or [Vx+My*W, Vy-Mx*W].
 *  This can then be sent to the module as a velocity and angle.
 *
 *  The modules can only go so fast, so the speeds of the modules must be reduced so the fastest command is no faster than
 *  the maximum speed of the driving motor. First, check the maximum velocity command for the modules.
 *  If it is greater than the maximum velocity achievable, divide every velocity by the largest velocity.
 *  Since all velocities are positive along a vector, there is no need to use the absolute value.
 * */

#include <vector>
#include <cstdlib>
#include <cmath>

typedef struct {
    double Mx;
    double My;
    double maxVelocity;
} SWERVE_MODULE_DESCRIPTOR;

typedef struct {
    double velocity;
    double angle;
} SWERVE_COMMAND;

SWERVE_COMMAND getModuleCommand(double Vrx, double Vry, double W, SWERVE_MODULE_DESCRIPTOR &module) {
    SWERVE_COMMAND command;
    double moduleVx = Vrx + module.My*W;
    double moduleVy = Vry - module.Mx*W;
    command.velocity = sqrt(pow(moduleVx, 2) + pow(moduleVy, 2));
    command.angle = atan2(moduleVy, moduleVx);
    return command;
};

// Velocities are all positive: no need for absolute values
void desaturateWheelVelocities(std::vector<SWERVE_COMMAND> &commands, double maxVelocity) {
    double mostVelocity;
    for (auto &command : commands) {
        mostVelocity = std::max(mostVelocity, command.velocity);
    }
    if (mostVelocity > maxVelocity) {
        for (auto &command : commands) {
            command.velocity = command.velocity / mostVelocity;
        }
    }
}

// Assumes all modules have the same max velocity
std::vector<SWERVE_COMMAND> getSwerveCommands(double Vx, double Vy, double W, double thetaRobot, std::vector<SWERVE_MODULE_DESCRIPTOR> &modules) {
    std::vector<SWERVE_COMMAND> commands;
    commands.reserve(modules.size());
    double translationMagnitude = sqrt(pow(Vx, 2) + pow(Vy, 2));
    double translationAngle = atan2(Vy, Vx);
    double Vrx = translationMagnitude * cos(translationAngle + thetaRobot);
    double Vry = translationMagnitude * sin(translationAngle + thetaRobot);
    for (auto &module : modules) {
        commands.push_back(getModuleCommand(Vrx, Vry, W, module));
    }
    desaturateWheelVelocities(commands, modules[0].maxVelocity);
    return commands;
}

#endif //SWERVE_TESTING_SWERVEKINEMATICS_H
