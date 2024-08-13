
#include "controllers/FollowingController.h"

FollowingController::FollowingController(Vision* vision) : ControllerBase("following"), _vision(vision) {}

void FollowingController::Configure(tinyxml2::XMLElement* xml) {
    const char * targetNameC;
    xml->QueryStringAttribute("targetName", &targetNameC);
    _targetName = std::string(targetNameC);
    xml->QueryDoubleAttribute("maxFollowDistance", &_maxFollowDistance);
    xml->QueryDoubleAttribute("minFollowDistance", &_minFollowDistance);
    xml->QueryDoubleAttribute("distanceFilterAlpha", &_distanceFilterAlpha);
    xml->QueryDoubleAttribute("driveVelocity", &_driveVelocity);
    xml->QueryDoubleAttribute("headingKp", &_headingKp);
}

ControlCmds FollowingController::Run(ControlCmds cmds) {

    _locked = false;
    _target.pose = {0.0, 0.0, 100.0};
    for (Detection& det : _vision->GetDetections()) {
        if (_targetName == det.name && det.pose.z() < _target.pose.z()) {
            _target = det;
        }
    }
    if (_target.pose.z() < _maxFollowDistance) {
        _locked = true;
    }

    if (_locked) {
        if (std::abs(_target.pose.z() - _targetFilteredDistance) > 1.0) {
            _targetFilteredDistance = _target.pose.z();
        }
        _targetFilteredDistance = _distanceFilterAlpha * _target.pose.z() + (1 - _distanceFilterAlpha) * _targetFilteredDistance;
        if (_targetFilteredDistance > _minFollowDistance) {
            cmds.drive.velocity = _driveVelocity;
        }
        cmds.drive.angularVelocity = _target.pose.x() * _headingKp;
    }

    return cmds;
}

void FollowingController::ReportState(std::string prefix) {
    prefix += "following_controller/";
    StateReporter::GetInstance().UpdateKey(prefix + "locked", _locked);
    StateReporter::GetInstance().UpdateKey(prefix + "targetX", _target.pose.x());
    StateReporter::GetInstance().UpdateKey(prefix + "targetZ", _target.pose.z());
    StateReporter::GetInstance().UpdateKey(prefix + "targetDistance", _targetFilteredDistance);
}