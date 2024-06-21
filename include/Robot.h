#pragma once

#include "VescController.h"

class Robot {
public:

    Robot();

    void run(int rate, bool& running);
    void changeDriveMode(const char* mode);
    void drive(float leftSpeed, float rightSpeed);

private:

    VescController _leftFront;
    VescController _rightFront;
    VescController _leftBack;
    VescController _rightBack;

    enum DRIVE_MODE {
        DRIVE_FORWARD,
        DRIVE_BACK,
        DRIVE_LEFT,
        DRIVE_RIGHT,
        STOP
    };

    DRIVE_MODE _driveMode = DRIVE_FORWARD;
};