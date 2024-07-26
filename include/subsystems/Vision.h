#pragma once

#include <vector>

#include "Utils.h"
#include "subsystems/SubsystemBase.h"
#include "MessageQueue.h"
#include "rapidjson/document.h"
#include <Eigen/Core>

struct VisionData
{
    std::string name;
    Eigen::Vector3d pose;
};

class Vision : SubsystemBase
{
    const char *name;

    std::vector<VisionData> _visionData;

    rapidjson::Document _document;

    double _heading;

public:
    Vision() : name("Vision"), _document() {};

    void Update(double dt);
    void ReportState(std::string prefix = "/");

    void UpdateVisionData(std::string data);

    inline double GetHeading() { return _heading; }
};