#include "controllers/OverrideController.h"

OverrideController::OverrideController(Vision* vision) : ControllerBase("override"), _vision(vision) {}

void OverrideController::Configure(tinyxml2::XMLElement* xml) {
    xml->QueryDoubleAttribute("stoppingDistance", &_stoppingDistance);
}

ControlCmds OverrideController::Run(ControlCmds cmds) {
    double closestObstacle = _vision->GetClosestDetectionDisance();
    _obstacleBlocked = closestObstacle > 0 && closestObstacle < _stoppingDistance;
    if (_obstacleBlocked) {
        if (cmds.drive.velocity > 0) {
            cmds.drive.velocity = 0;
        }
    }

    if (_eStopped) {cmds = ControlCmds();}
    
    return cmds;
}

void OverrideController::ReportState(std::string prefix) {
    prefix += "override_controller/";
    StateReporter::GetInstance().UpdateKey(prefix + "estopped", _eStopped);
    StateReporter::GetInstance().UpdateKey(prefix + "obstacle_blocked", _obstacleBlocked);
}
