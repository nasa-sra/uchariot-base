#define _USE_MATH_DEFINES
#include <cmath>

#include "subsystems/DriveBase.h"

DriveBase::DriveBase() : 
    _left_front(1),
    _right_front(4),
    _left_back(2),
    _right_back(3) {

        float gearRatio = 6.0; // Maybe?
        float wheelRadius = 12.0; // in !definitely not!
        float scale = gearRatio * 60 / (2 * M_PI * wheelRadius); // converts from in/s to motor RPM
        _left_front.SetScale(-scale);
        _right_front.SetScale(-scale);
        _left_back.SetScale(scale);
        _right_back.SetScale(scale);

        VescController::Mode mode = VescController::Mode::VELOCITY;
        _left_front.SetMode(mode);
        _right_front.SetMode(mode);
        _left_back.SetMode(mode);
        _right_back.SetMode(mode);
}


void DriveBase::Update() {

    // Utils::LogFmt("Drivebase Speeds: lb %f  lf %f  rb %f  sb %f", _output._lb_speed, _output._lf_speed, _output._rb_speed, _output._rf_speed);

    _left_back.SetCmd(_output._lb_speed);
    _left_front.SetCmd(_output._lf_speed);
    _right_back.SetCmd(_output._rb_speed);
    _right_front.SetCmd(_output._rf_speed);
    _output = DriveOutput(); // reset to 0
}