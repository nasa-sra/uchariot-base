#pragma once

#include "Utils.h"
#include "subsystems/SubsystemBase.h"
#include "subsystems/DriveBase.h"
#include "subsystems/IMUBase.h"
#include "subsystems/Vision.h"
#include "subsystems/GPS.h"

struct Pose {
    Eigen::Vector2d pos;
    double heading;
};

class Localization : SubsystemBase {
public:
    Localization(DriveBase* driveBase, IMUBase* imu, Vision* vision, GPS* gps);

    void Update(double dt);
    void ReportState(std::string prefix = "/");

    void ResetHeading();
    void ResetPose();

    void SetOrigin(Utils::GeoPoint origin) {_origin = origin; }
    Pose GetPose() { return _pose; }

private:

    DriveBase* _driveBase;
    IMUBase* _imu;
    Vision* _vision;
    GPS* _gps;

    Pose _pose{{0.0, 0.0}, 0.0};
    Utils::GeoPoint _origin;

    timespec_t _lastGPSUpdate;

    double _imuOffset{0.0};
    double _rsOffset{0.0};

};
