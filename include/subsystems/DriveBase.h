#pragma once

#define ROBOT_WIDTH 5.0


#include "VescController.h"

#include "StateReporter.h"
#include "subsystems/SubsystemBase.h"
#include "controllers/PIDController.h"

// struct DriveBaseCmds {
//     float _lf_speed;
//     float _rf_speed;
//     float _lb_speed;
//     float _rb_speed;

//     DriveBaseCmds() : _lf_speed(0), _rf_speed(0), _lb_speed(0), _rb_speed(0) {};
//     DriveBaseCmds(float left, float right) : _lf_speed(left), _rf_speed(right), _lb_speed(left), _rb_speed(right) {};
//     DriveBaseCmds(float lf, float rf, float lb, float rb)
//         : _lf_speed(lf), _rf_speed(rf), _lb_speed(lb), _rb_speed(rb) {};

//     void ReportState(std::string prefix = "/");
// };

struct DriveBaseCmds {
    double _speed{10};

    float _lf_speed{0};
    float _rf_speed{0};
    float _lb_speed{0};
    float _rb_speed{0};

    void ReportState(std::string prefix = "/");

    DriveBaseCmds(double angular, double drive) {
        double maxAng = _speed / ROBOT_WIDTH;

        double _omega = std::clamp(2 * angular, -maxAng, maxAng);

        _rf_speed = std::clamp((_omega * ROBOT_WIDTH / 2) + std::clamp(drive, -_speed, _speed), -_speed, _speed);
        _rb_speed = _rf_speed;
        _lf_speed = std::clamp(std::clamp(drive, -_speed, _speed) - (_omega * ROBOT_WIDTH / 2), -_speed, _speed);
        _lb_speed = _lf_speed;
    }

    DriveBaseCmds(){}
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