#pragma once

#include "Utils.h"
#include "subsystems/DriveBase.h"
#include "subsystems/IMUBase.h"
#include "subsystems/Vision.h"
#include "subsystems/SubsystemBase.h"

struct Pose {
    Eigen::Vector2d pos;
    double heading;
};

class Localization : SubsystemBase {
public:
    Localization(DriveBase* driveBase, IMUBase* imu, Vision* vision);

    void Update(double dt);
    void ReportState(std::string prefix = "/");

    void ResetHeading();
    void ResetPose();
    
    Pose getPose() { return _pose; }

private:

    DriveBase* _driveBase;
    IMUBase* _imu;
    Vision* _vision;

    Pose _pose{{0.0, 0.0}, 0.0};

    double _imuOffset{0.0};
    double _rsOffset{0.0};

};
