#pragma once

#include "Utils.h"
#include "subsystems/SubsystemBase.h"
#include "subsystems/DriveBase.h"

struct Pose {
    Eigen::Vector2d pos;
    double heading;
};

class Localization : SubsystemBase {
  public:

    Localization();

    void Update(double dt, DriveBaseFeedback driveVels, double heading_imu, double heading_rs);
    void ReportState(std::string prefix = "/");

    Pose getPose() {return _pose;}

private:

	double _heading_odom;

    Pose _pose{{0.0, 0.0}, 0.0};

};
