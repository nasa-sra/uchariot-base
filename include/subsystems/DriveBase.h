#pragma once

#include "VescController.h"

#include "Base.h"

struct DriveOutput {
    float _lf_speed;
    float _rf_speed;
    float _lb_speed;
    float _rb_speed;

    DriveOutput() : _lf_speed(0), _rf_speed(0), _lb_speed(0), _rb_speed(0) {};
    DriveOutput(float left, float right) : _lf_speed(left), _rf_speed(right), _lb_speed(left), _rb_speed(right) {};
    DriveOutput(float lf, float rf, float lb, float rb) : _lf_speed(lf), _rf_speed(rf), _lb_speed(lb), _rb_speed(rb) {};
};

class DriveBase : public SubsystemBase {
public:
    inline void SetOutput(DriveOutput output) { _output = output; }

    DriveBase();

    void Update() override;

    static DriveBase& GetInstance() {
        static DriveBase _instance;
        return _instance;
    }
   
private:
    DriveOutput _output;

    VescController _left_front;
    VescController _right_front;
    VescController _left_back;
    VescController _right_back;
};