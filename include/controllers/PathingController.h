#pragma once

#include "subsystems/Localization.h"
#include "controllers/ControllerBase.h"
#include "rapidjson/document.h"

struct PathStep {
    Eigen::Vector3d pos;
    Utils::GeoPoint geoPoint;
    float speed;
    float tolerance;
};

class PathingController : public ControllerBase {

public:
    PathingController() : ControllerBase("pathing") {}

    void Load() override; 
    void Unload() override;
    ControlCmds Run(Pose robotPose);

    void ReportState(std::string prefix = "/");
    void HandleNetworkInput(rapidjson::Document& doc);

private:

    bool loadPath(std::string filePath);
    Utils::GeoPoint parseCoordinates(std::string coords, bool altitude, bool flipped=false);
    Eigen::Vector3d geoToPathCoord(Utils::GeoPoint geo, Utils::GeoPoint geoOrigin);

    std::string _pathName{""};
    bool _runningPath{false};

    std::vector<PathStep> _path;
    int _currentStep{0};

    double _headingGain{0.01};
    float _endTolerance{0.1};

};