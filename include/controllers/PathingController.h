#pragma once

#include "controllers/ControllerBase.h"
#include "subsystems/Localization.h"

struct PathStep {
    Eigen::Vector3d pos;
    Utils::GeoPoint geoPoint;
    float speed;
    float tolerance;
};

struct PathObject {
    Pose pose;
    double radius_m;

    PathObject(Pose pose, double radius_m) : pose(pose), radius_m(radius_m){};
};

class PathingController : public ControllerBase {

public:
    PathingController(Localization* localization);

    void Configure(tinyxml2::XMLElement* xml) override;
    void Load() override;
    void Unload() override;
    ControlCmds Run(ControlCmds cmds = ControlCmds()) override;
    void SetPathName(std::string name);
    void Pause();
    void Stop();

    void ReportState(std::string prefix = "/") override;

    Utils::GeoPoint GetOrigin() { return _origin; }

private:
    bool loadPath(std::string filePath);
    bool loadXMLPath(std::string filePath);
    bool loadKMLPath(std::string filePath);
    Eigen::Vector3d parseCoordinates(std::string coords);
    Utils::GeoPoint parseGeoCoordinates(std::string coords, bool altitude, bool flipped = false);
    Eigen::Vector3d geoToPathCoord(Utils::GeoPoint geo, Utils::GeoPoint geoOrigin);

    Localization* _localization;

    std::string _pathName{""};
    bool _runningPath{false};
    bool _pathPaused{false};

    int _pathResolution{1};
    float _pathSpeed{2};
    float _pathRadius{3};

    std::vector<PathStep> _path;
    int _currentStep{0};

    double _velocityGain{0.5};
    double _headingGain{-1.0};
    float _endTolerance{0.1};
    double _targetHeading{0.0};
    double _distanceToWaypoint{0.0};

    Eigen::Vector2d _nextWaypoint{0.0, 0.0};

    Utils::GeoPoint _origin;
};