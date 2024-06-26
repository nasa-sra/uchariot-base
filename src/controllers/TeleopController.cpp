#include "controllers/TeleopController.h"

void TeleopController::HandleNetworkInput(rapidjson::Document& doc) {

    _speed = doc["speed"].GetDouble();    // [0, MAX_SPEED]
    _fwd = doc["fwd"].GetDouble();        // [-1, 1]
    _turn = doc["turn"].GetDouble();      // [-1, 1]
}

ControlCmds TeleopController::Run() {

    ControlCmds cmds;

    const double deadband = 0.05, min = 0.125;
    if (std::abs(_fwd) < deadband) _fwd = 0;
    if (std::abs(_turn) < deadband) _turn = 0;
    double left, right;

    if (_turn >= 0) {    // handle right
        left = _speed * _fwd;
        right = (1 - _turn) * (left - min * left) + min * left;
    }
    else {  // handle left 
        right = _speed * _fwd;
        left = (1 + _turn) * (right - min * right) + min * right;
    }
 
    cmds.drive = DriveBaseCmds(left, right);
}