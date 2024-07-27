#include "controllers/TeleopController.h"

void TeleopController::HandleNetworkInput(rapidjson::Document& doc) {
    _fwdVelocity = doc["velocity"].GetDouble();  // m/s
    _angVelocity = doc["rotation"].GetDouble();  // rad/s
}

ControlCmds TeleopController::Run() { return {_fwdVelocity, _angVelocity}; }