#pragma once

#define ROBOT_WIDTH 1.0

#include "VescController.h"

#include "StateReporter.h"
#include "subsystems/SubsystemBase.h"
#include "controllers/PIDController.h"

struct DriveBaseCmds {

    float _lf_speed{0};
    float _rf_speed{0};
    float _lb_speed{0};
    float _rb_speed{0};

    DriveBaseCmds(){}
    DriveBaseCmds(double angular, double drive);

    void ReportState(std::string prefix = "/");

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