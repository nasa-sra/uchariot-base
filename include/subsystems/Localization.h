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

    void Update(double dt, DriveBaseFeedback driveVels, double heading_imu, double heading_rs);
    void ReportState(std::string prefix = "/");

    Pose getPose() {
        return _pose;
    }

    void ResetOdometry() {
        _pose({0.0, 0.0}, 0.0);
    }

    void ResetPose() {
        _pose({0.0, 0.0}, _pose.heading);
    }

    double _heading_odom;

    Pose _pose{{0.0, 0.0}, 0.0};
};
