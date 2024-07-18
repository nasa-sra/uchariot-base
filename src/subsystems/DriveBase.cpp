#define _USE_MATH_DEFINES
#include <cmath>

#include "subsystems/DriveBase.h"

void DriveBaseCmds::ReportState(std::string prefix) {
    StateReporter::GetInstance().UpdateKey(prefix + "velocity", velocity);
    StateReporter::GetInstance().UpdateKey(prefix + "angular_velocity", angularVelocity);
}

DriveBase::DriveBase() : _left_front(1), _right_front(4), _left_back(2), _right_back(3) {

    float gearRatio = 160;
    float wheelRadius = 0.254;                               // m
    float scale = gearRatio * 60 / (2 * M_PI * wheelRadius); // converts from m/s to motor RPM
    _left_front.SetScale(scale);
    _right_front.SetScale(scale);
    _left_back.SetScale(scale);
    _right_back.SetScale(scale);

    VescController::Mode mode = VescController::Mode::VELOCITY;
    _left_front.SetMode(mode);
    _right_front.SetMode(mode);
    _left_back.SetMode(mode);
    _right_back.SetMode(mode);
}

void DriveBase::Update(double dt) {

    // Utils::LogFmt("Drivebase Speeds: lb %f  lf %f  rb %f  sb %f", _cmds._lb_speed, _cmds._lf_speed, _cmds._rb_speed,
    // _cmds._rf_speed);

    const double maxAng = MAX_DRIVE_SPEED / ROBOT_WIDTH;
    double vel = std::clamp(_cmds.velocity, -MAX_DRIVE_SPEED, MAX_DRIVE_SPEED);
    double omega = std::clamp(2 * _cmds.angularVelocity, -maxAng, maxAng);

    double accelerationLimit = 3.0; // m/s^2
    double maxDv = accelerationLimit * dt;

    double left = std::clamp((omega * ROBOT_WIDTH / 2) + vel, -MAX_DRIVE_SPEED, MAX_DRIVE_SPEED);
    left = std::clamp(left, _left_front.GetCmdVelocity() - maxDv, _left_front.GetCmdVelocity() + maxDv);
    double right = std::clamp(vel - (omega * ROBOT_WIDTH / 2), -MAX_DRIVE_SPEED, MAX_DRIVE_SPEED);
    right = std::clamp(right, _right_front.GetCmdVelocity() - maxDv, _right_front.GetCmdVelocity() + maxDv);

    _left_front.SetCmd(left);
    _right_front.SetCmd(right);
    _left_back.SetCmd(left);
    _right_back.SetCmd(right);
    _cmds = {0, 0};

    _left_front.Update();
    _right_front.Update();
    _left_back.Update();
    _right_back.Update();

    _voltage = (_left_front.GetVoltage() + _right_front.GetVoltage() + _left_back.GetVoltage() +
                _right_back.GetVoltage()) / 4;
}

void DriveBase::ReportState(std::string prefix) {
    prefix += "drive_base/";
    StateReporter::GetInstance().UpdateKey(prefix + "voltage", _voltage);

    _left_front.ReportState(prefix + "left_front/");
    _right_front.ReportState(prefix + "right_front/");
    _left_back.ReportState(prefix + "left_back/");
    _right_back.ReportState(prefix + "right_back/");
}

DriveBaseFeedback DriveBase::GetVelocities() {
    return {_left_front.GetVelocity(), _right_front.GetVelocity(), _left_back.GetVelocity(), _right_back.GetVelocity()};
}
