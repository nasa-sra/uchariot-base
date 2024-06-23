#include "subsystems/DriveBase.h"

void DriveBase::Update() {

    // Utils::LogFmt("Drivebase Speeds: lb %f  lf %f  rb %f  sb %f", _output._lb_speed, _output._lf_speed, _output._rb_speed, _output._rf_speed);

    _left_back.SetCmd(_output._lb_speed);
    _left_front.SetCmd(_output._lf_speed);
    _right_back.SetCmd(_output._rb_speed);
    _right_front.SetCmd(_output._rf_speed);
    _output = DriveOutput(); // reset to 0
}