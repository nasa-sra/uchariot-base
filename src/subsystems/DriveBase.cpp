#define _USE_MATH_DEFINES
#include <cmath>

#include "subsystems/DriveBase.h"

void DriveBaseCmds::ReportState(std::string prefix) {
    StateReporter::GetInstance().UpdateKey(prefix + "lf_velocity", _lf_speed);
    StateReporter::GetInstance().UpdateKey(prefix + "rf_velocity", _rf_speed);
    StateReporter::GetInstance().UpdateKey(prefix + "lb_velocity", _lb_speed);
    StateReporter::GetInstance().UpdateKey(prefix + "rb_velocity", _rb_speed);
}

DriveBase::DriveBase() : 
    _left_front(1),
    _right_front(4),
    _left_back(2),
    _right_back(3) {
        // RPM * 60 = RPS_r / 30 = wRPS * 2PI * r = in/s
        float gearRatio = 30.0; // Maybe?
        float wheelRadius = 0.254; // in !definitely not!
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


void DriveBase::Update(double dt) {

    // Utils::LogFmt("Drivebase Speeds: lb %f  lf %f  rb %f  sb %f", _cmds._lb_speed, _cmds._lf_speed, _cmds._rb_speed, _cmds._rf_speed);

    _left_front.SetCmd(_cmds._lf_speed);
    _right_front.SetCmd(_cmds._rf_speed);
    _left_back.SetCmd(_cmds._lb_speed);
    _right_back.SetCmd(_cmds._rb_speed);
    _cmds = DriveBaseCmds(); // reset to 0

    _left_front.Update();
    _right_front.Update();
    _left_back.Update();
    _right_back.Update();

    _voltage = (_left_front.GetVoltage() + _right_front.GetVoltage() + _left_back.GetVoltage() + _right_back.GetVoltage()) / 4;
}

void DriveBase::ReportState(std::string prefix) {
    prefix += "drive_base/";
    StateReporter::GetInstance().UpdateKey(prefix + "voltage", _voltage);

    _left_front.ReportState(prefix + "left_front/");
    _right_front.ReportState(prefix + "right_front/");
    _left_back.ReportState(prefix + "left_back/");
    _right_back.ReportState(prefix + "right_back/");
}