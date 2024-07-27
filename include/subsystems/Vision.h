#pragma once

#include <Eigen/Core>
#include <vector>

#include "DriveBase.h"
#include "MessageQueue.h"
#include "Utils.h"
#include "rapidjson/document.h"
#include "subsystems/SubsystemBase.h"

struct VisionData {
    std::string name;
    Eigen::Vector3d pose;

    VisionData() : pose() {};
};

class Vision : SubsystemBase {
    const char *name;

    std::vector<VisionData> _visionData;

    rapidjson::Document _document;

    double _heading;

   public:
    Vision(DriveBase *driveBase)
        : name("Vision"), _driveBase(driveBase), _document() {};

    void Update(double dt);
    void ReportState(std::string prefix = "/");

    void UpdateVisionData(std::string data);

    inline double GetHeading() { return _heading; }

   private:
    DriveBase *_driveBase;
};