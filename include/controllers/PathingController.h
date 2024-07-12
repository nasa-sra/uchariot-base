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
    Eigen::Vector3d parseCoordinates(std::string coords);
    Utils::GeoPoint parseGeoCoordinates(std::string coords, bool altitude, bool flipped=false);
    Eigen::Vector3d geoToPathCoord(Utils::GeoPoint geo, Utils::GeoPoint geoOrigin);

    std::string _pathName{""};
    bool _runningPath{false};

    int _pathResolution{1};
    float _pathSpeed{2};
    float _pathRadius{3};

    std::vector<PathStep> _path;
    int _currentStep{0};

    double _velocityGain{5.0};
    double _headingGain{1.0};
    float _endTolerance{0.1};

    Eigen::Vector2d _nextWaypoint{0.0, 0.0};

};