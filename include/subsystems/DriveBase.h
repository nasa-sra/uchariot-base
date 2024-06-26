#pragma once

#include "VescController.h"

#include "subsystems/SubsystemBase.h"

struct DriveBaseCmds {
    float _lf_speed;
    float _rf_speed;
    float _lb_speed;
    float _rb_speed;

    DriveBaseCmds() : _lf_speed(0), _rf_speed(0), _lb_speed(0), _rb_speed(0) {};
    DriveBaseCmds(float left, float right) : _lf_speed(left), _rf_speed(right), _lb_speed(left), _rb_speed(right) {};
    DriveBaseCmds(float lf, float rf, float lb, float rb) : _lf_speed(lf), _rf_speed(rf), _lb_speed(lb), _rb_speed(rb) {};
};

class DriveBase : public SubsystemBase {
public:

    DriveBase();

    void Update() override;

    inline void SetCmds(DriveBaseCmds cmds) { _cmds = cmds; }

    static DriveBase& GetInstance() {
        static DriveBase _instance;
        return _instance;
    }
   
private:
    DriveBaseCmds _cmds;

    VescController _left_front;
    VescController _right_front;
    VescController _left_back;
    VescController _right_back;
};