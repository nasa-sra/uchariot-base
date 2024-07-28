#include "controllers/TeleopController.h"

TeleopController::TeleopController() : ControllerBase("teleop") {}

void TeleopController::SetDriveInput(double velocity, double rotation) {
    _fwdVelocity = velocity;
    _angVelocity = rotation;
}

ControlCmds TeleopController::Run(ControlCmds cmds) {
    cmds.drive.velocity = _fwdVelocity;
    cmds.drive.angularVelocity = _angVelocity;
    return cmds;
}