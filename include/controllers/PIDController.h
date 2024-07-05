#pragma once
#include "Utils.h"

struct PIDValues {
    float kP;
    float kI;
    float kD;

    PIDValues() : kP(.1), kI(0), kD(0) {};
    PIDValues(double p, double i, double d) : kP(p), kI(i), kD(d) {};
    PIDValues(double p, double i) : kP(p), kI(i), kD(0) {};
    PIDValues(double p) : kP(p), kI(0), kD(0) {};
};

class PIDController {
public:
    PIDController(PIDValues pid) : _p(pid.kP), _i(pid.kI), _d(pid.kD) {}
    

    double Calculate(double current, double target);
private:
    double _p{0};
    double _i{0};
    double _d{0};

    double _lastError{0};
    Utils::msec_t _lastTimestamp{0};
};