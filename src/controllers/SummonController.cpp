
#include "controllers/SummonController.h"

SummonController::SummonController(Localization* localization, Vision* vision) : ControllerBase("summon"), 
    _vision(vision),
    _localization(localization) {}

void SummonController::Configure(tinyxml2::XMLElement* xml) {
    xml->QueryDoubleAttribute("driveVelocity", &_driveVelocity);
    xml->QueryDoubleAttribute("velocityKp", &_velocityKp);
    xml->QueryDoubleAttribute("headingKp", &_headingKp);
    xml->QueryDoubleAttribute("transitionDistance", &_transitionDistance);
    xml->QueryDoubleAttribute("stopDistance", &_stopDistance);
    const char* targetNameC;
    xml->QueryStringAttribute("targetName", &targetNameC);
    _targetName = std::string(targetNameC);
}

void SummonController::Load() {
    _summoned = false;
    _runningPath = false;
    _targetPos = {0.0, 0.0};
    _distanceToWaypoint = 0.0;
    _targetHeading = 0.0;
}

ControlCmds SummonController::Run(ControlCmds cmds) {

    _targetDet.pose = {0.0, 0.0, 100.0};
    for (Detection& det : _vision->GetDetections()) {
        if (_targetName == det.name && det.pose.z() < _targetDet.pose.z()) {
            _targetDet = det;
        }
    }

    if (_summoned) {
        _localization->SetOrigin(_localization->GetGeoLocation());
        _summoned = false;
        _runningPath = true;
    }

    if (_runningPath) {
        Pose robotPose = _localization->GetPose();
        Eigen::Vector2d diff = _targetPos - robotPose.pos;
        _distanceToWaypoint = diff.norm();
        double velocity = std::clamp((float)(_distanceToWaypoint * _velocityKp), 0.0f, (float) _driveVelocity);
        _targetHeading = atan2(diff.y(), diff.x());
        
        double headingErr = Utils::shortestAngularDistance(robotPose.heading, _targetHeading);
        double angularVelocity = headingErr * _headingKp;

        cmds.drive = {velocity, angularVelocity};

        if ((_distanceToWaypoint < _transitionDistance && _targetDet.pose.z() < _transitionDistance) ||
                _distanceToWaypoint < 1.0) {
            _runningPath = false;
            _targetPos = {0.0, 0.0};
            _runningTracking = true;
        }
    }

    if (_runningTracking) {
        if (_targetDet.pose.z() < _stopDistance) {
            Utils::LogFmt("Arrived at summon");
            _runningTracking = false;
        }

        cmds.drive.velocity = _driveVelocity;
        cmds.drive.angularVelocity = _targetDet.pose.x() * _headingKp;
    }

    return cmds;
}

void SummonController::Summon(Utils::GeoPoint target) {
    Utils::LogFmt("Robot Summoned to %f, %f", target.lat, target.lon);
    _targetPos = Utils::geoToLTP(target, _localization->GetGeoLocation()).head<2>();
    // _targetPos /= 2.75; // need to fix math
    _summoned = true;
}

void SummonController::ReportState(std::string prefix) {
    prefix += "summoning_controller/";
    StateReporter::GetInstance().UpdateKey(prefix + "runningPath", _runningPath);
    StateReporter::GetInstance().UpdateKey(prefix + "runningTracking", _runningTracking);
    StateReporter::GetInstance().UpdateKey(prefix + "targetPosX", _targetPos.x());
    StateReporter::GetInstance().UpdateKey(prefix + "targetPosY", _targetPos.y());
    StateReporter::GetInstance().UpdateKey(prefix + "targetHeading", _targetHeading);
    StateReporter::GetInstance().UpdateKey(prefix + "distanceToWaypoint", _distanceToWaypoint);
    StateReporter::GetInstance().UpdateKey(prefix + "targetDetectionX", _targetDet.pose.x());
    StateReporter::GetInstance().UpdateKey(prefix + "targetDetectionZ", _targetDet.pose.z());
}