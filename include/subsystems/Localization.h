#pragma once

#include "Utils.h"
#include "subsystems/DriveBase.h"
#include "subsystems/SubsystemBase.h"

struct Pose {
    Eigen::Vector2d pos;
    double heading;
};

class Localization : SubsystemBase {
public:
    Localization();

    void Update(double dt, DriveBaseFeedback driveVels);
    void ReportState(std::string prefix = "/");

    Pose getPose() {
        return _pose;
    }

    void ResetOdometry() {
        return
    }

private:
    Pose _pose{{0.0, 0.0}, 0.0};
    double _slipCoefficient{0.0};
};