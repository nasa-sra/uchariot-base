#pragma once

#include "VescController.h"

class Robot {
public:

    Robot();

    void run(int rate, bool& running);

private:

    VescController _leftFront;
    VescController _rightFront;
    VescController _leftBack;
    VescController _rightBack;

};