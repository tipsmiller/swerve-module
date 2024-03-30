#include <fcntl.h>
#include <linux/input.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "Gamepad.h"


Gamepad::Gamepad(const char *eventFileName) {
    // It's okay for this to be blocking because it will be read in its own thread
    int fd = open(eventFileName, O_RDONLY);
    int rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        exit(1);
    }
    printf("Input device name: \"%s\"\n", libevdev_get_name(dev));
    printf("Input device ID: bus %#x vendor %#x product %#x\n",
           libevdev_get_id_bustype(dev),
           libevdev_get_id_vendor(dev),
           libevdev_get_id_product(dev));
    if (!libevdev_has_event_type(dev, EV_ABS) ||
        !libevdev_has_event_code(dev, EV_KEY, BTN_SOUTH)) {
        printf("This device does not look like a gamepad\n");
    }
    inputReader = new std::thread([this]{
        while(active) {
            struct input_event ev;
            libevdev_next_event(dev, LIBEVDEV_READ_FLAG_BLOCKING, &ev);  // Blocking read
//            if (ev.type == EV_KEY) {
//                printf("Event: %s %s %d\n",
//                       libevdev_event_type_get_name(ev.type),
//                       libevdev_event_code_get_name(ev.type, ev.code),
//                       ev.value);
//            }
//            if (ev.type == EV_ABS && ev.code == ABS_HAT0X) {
//                printf("Event: %s %s %d\n",
//                       libevdev_event_type_get_name(ev.type),
//                       libevdev_event_code_get_name(ev.type, ev.code),
//                       ev.value);
//            }
        }
    });
}

Gamepad::~Gamepad() {
    active = false;
    if (inputReader != nullptr && inputReader->joinable()) {
        inputReader->join();
    }
}

int Gamepad::get(BUTTONS button) {
    libevdev_fetch_event_value(dev, EV_KEY, button, &buttonStates[button]);
    return buttonStates[button];
}

int Gamepad::getPressed(BUTTONS button) {
    int lastState = buttonStates[button];
    get(button);
    if (buttonStates[button] == 1 && lastState == 0) {
        printf("%s pressed\n", libevdev_event_code_get_name(EV_KEY, button));
        return true;
    } else {
        return false;
    }
}

int Gamepad::getReleased(BUTTONS button) {
    int lastState = buttonStates[button];
    get(button);
    if (buttonStates[button] == 0 && lastState == 1) {
        return true;
    } else {
        return false;
    }
}

double Gamepad::getAxis(AXES axis) {
    int value = 0;
    libevdev_fetch_event_value(dev, EV_ABS, axis, &value);
    return double(value - 128) / 128.0;
}

std::array<double, 2> Gamepad::getAxisValues(AXES x, AXES y) {
    double real = getAxis(x);
    double imag = -getAxis(y);
    return {real, imag};
}

double Gamepad::getAngle(std::array<double, 2> values) {
    return (atan2(-values[1], -values[0]) + M_PI) * 180 / M_PI;
}

double Gamepad::getMagnitude(std::array<double, 2> values) {
    return sqrt(pow(values[0], 2) + pow(values[1], 2));
}

ANGLE_MAGNITUDE Gamepad::getAxisAngleAndMagnitude(AXES x, AXES y) {
    auto values = getAxisValues(x, y);
    double angle = getAngle(values);
    double magnitude = getMagnitude(values);
    return {angle, magnitude};
}

ANGLE_MAGNITUDE Gamepad::getAxisAngleAndMagnitude(AXES x, AXES y, double deadband) {
    auto a_m = getAxisAngleAndMagnitude(x, y);
    a_m.magnitude = std::max((a_m.magnitude - deadband) / (1.0 - deadband), 0.0);
    return a_m;
}