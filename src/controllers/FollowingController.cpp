
#include "controllers/FollowingController.h"

FollowingController::FollowingController(Vision* vision) : ControllerBase("following"), _vision(vision) {}

ControlCmds FollowingController::Run(ControlCmds cmds) {

    _target = Detection();
    _target.pose = {0.0, 0.0, 6.0};
    for (Detection& det : _vision->GetDetections()) {
        if (det.name == "Person" && det.pose.z() < _target.pose.z()) {
            _target = det;
        }
    }

    if (_target.name != "") {
        if (std::abs(_target.pose.z() - _targetFilteredDistance) > 1.0) {
            _targetFilteredDistance = _target.pose.z();
        }
        _targetFilteredDistance = _distanceFilterAlpha * _target.pose.z() + (1 - _distanceFilterAlpha) * _targetFilteredDistance;
        if (_targetFilteredDistance > 2.0) {
            cmds.drive.velocity = 0.5;
        }
        cmds.drive.angularVelocity = _target.pose.x() * -0.1;
    }

    return cmds;
}

void FollowingController::ReportState(std::string prefix) {
    prefix += "following_controller/";
    StateReporter::GetInstance().UpdateKey(prefix + "targetX", _target.pose.x());
    StateReporter::GetInstance().UpdateKey(prefix + "targetZ", _target.pose.z());
    StateReporter::GetInstance().UpdateKey(prefix + "targetDistance", _targetFilteredDistance);
}