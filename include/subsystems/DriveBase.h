#pragma once

#include "VescController.h"

#include "StateReporter.h"
#include "subsystems/SubsystemBase.h"
#include "controllers/PIDController.h"

struct DriveBaseCmds {
    float _lf_speed;
    float _rf_speed;
    float _lb_speed;
    float _rb_speed;

    DriveBaseCmds() : _lf_speed(0), _rf_speed(0), _lb_speed(0), _rb_speed(0) {};
    DriveBaseCmds(float left, float right) : _lf_speed(left), _rf_speed(right), _lb_speed(left), _rb_speed(right) {};
    DriveBaseCmds(float lf, float rf, float lb, float rb)
        : _lf_speed(lf), _rf_speed(rf), _lb_speed(lb), _rb_speed(rb) {};

    void ReportState(std::string prefix = "/");
};

class NewDriveBaseCmds {
public:
    double angularVelocity{0};
    double driveVelocity{0};
    double _speed{2500};
private:
    float _lf_speed{0};
    float _rf_speed{0};
    float _lb_speed{0};
    float _rb_speed{0};

    PIDController _driveController;
    PIDController _turnController;

public:
    NewDriveBaseCmds(PIDController drive, PIDController turn) : _driveController(drive), _turnController(turn) {};
    DriveBaseCmds Drive();
private:
    
};

struct DriveBaseFeedback {
    double lf, rf, lb, rb;
};

class DriveBase : public SubsystemBase {
public:
    DriveBase();

    void Update(double dt) override;
    void ReportState(std::string prefix = "/") override;
    DriveBaseFeedback GetVelocities();

    inline void SetCmds(DriveBaseCmds cmds) {
        _cmds = cmds;
    }

private:
    DriveBaseCmds _cmds;

    VescController _left_front;
    VescController _right_front;
    VescController _left_back;
    VescController _right_back;

    double _voltage;
};