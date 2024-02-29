#ifndef SWERVE_TESTING_GAMEPAD_H
#define SWERVE_TESTING_GAMEPAD_H


#include <libevdev-1.0/libevdev/libevdev.h>
#include <thread>
#include <map>
#include <complex>


typedef enum {
    A = BTN_SOUTH,
    B = BTN_EAST,
    X = BTN_WEST,
    Y = BTN_NORTH,
    SELECT = BTN_SELECT,
    START  = BTN_START,
    MODE = BTN_MODE,
    L1 = BTN_TL,
    L2 = BTN_TL2,
    L3 = BTN_THUMBL,
    R1 = BTN_TR,
    R2Touch = BTN_TR2,
    R3 = BTN_THUMBR,
} BUTTONS;
typedef enum {
    LX = ABS_X,
    LY = ABS_Y,
    LZ = ABS_Z, // Analog L2
    RX = ABS_RX,
    RY = ABS_RY,
    RZ = ABS_RZ, // Analog R2
    // DS4 puts d-pad on ABS events, but they read -1, 0, 1 ternary. Unimplemented for now
} AXES;

typedef struct {
    double angle;
    double magnitude;
} ANGLE_MAGNITUDE;

class Gamepad {
private:
    struct libevdev *dev = nullptr;
    std::thread* inputReader;
    std::atomic<bool> active = true;
    std::map<int, int> buttonStates;
public:
    Gamepad(const char* eventFileName);
    ~Gamepad();
    int get(BUTTONS button);
    int getPressed(BUTTONS button);
    int getReleased(BUTTONS button);
    double getAxis(AXES axis);
    std::array<double, 2> getAxisValues(AXES x, AXES y);
    double getAngle(std::array<double, 2> values);
    double getMagnitude(std::array<double, 2> values);
    ANGLE_MAGNITUDE getAxisAngleAndMagnitude(AXES x, AXES y);
    ANGLE_MAGNITUDE getAxisAngleAndMagnitude(AXES x, AXES y, double deadband);
};

#endif //SWERVE_TESTING_GAMEPAD_H
