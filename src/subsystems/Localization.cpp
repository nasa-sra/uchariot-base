#include "subsystems/Localization.h"

#include "Utils.h"

Localization::Localization(DriveBase* driveBase, IMUBase* imu, Vision* vision,
                           GPS* gps) {
    _driveBase = driveBase;
    _imu = imu;
    _vision = vision;
    _gps = gps;
}

void Localization::Update(double dt) {
    DriveBaseFeedback driveVels = _driveBase->GetVelocities();

    double vl = (driveVels.lf + driveVels.lb) / 2;
    double vr = (driveVels.rf + driveVels.rb) / 2;

    double v = (vl + vr) / 2.0;
    double omega = (vr - vl) / ROBOT_WIDTH;

    Eigen::Vector2d vel(v * cos(_pose.heading), v * sin(_pose.heading));

    _pose.pos += vel * dt;

    gps_fix_t fix = _gps->GetFix();
    if (fix.time.tv_nsec != _lastGPSUpdate.tv_nsec &&
        fix.time.tv_sec != _lastGPSUpdate.tv_sec) {
        _pose.pos = Utils::geoToLTP(Utils::GeoPoint(fix), _origin).head<2>();
        _lastGPSUpdate = fix.time;
    }
    _geoPos = Utils::LTPToGeo({_pose.pos[0], _pose.pos[1], 0.0}, _origin);

    // Utils::LogFmt("IMU %.4f    RS %.4f    ERR %.4f", heading_imu, heading_rs,
    // heading_imu - heading_rs);

    if (_useOdometryHeading) {
        _pose.heading += omega * dt;  // For running on cart/sim
    } else {
        // _pose.heading = _vision->GetHeading() - _rsOffset; // with realsense
        // gyro
        _pose.heading = _imu->GetYaw() - _imuOffset;  // with BNO055
    }
}
//
void Localization::ReportState(std::string prefix) {
    prefix += "localization/";
    StateReporter::GetInstance().UpdateKey(prefix + "x", _pose.pos.x());
    StateReporter::GetInstance().UpdateKey(prefix + "y", _pose.pos.y());
    StateReporter::GetInstance().UpdateKey(prefix + "heading", _pose.heading);
    StateReporter::GetInstance().UpdateKey(prefix + "latitude", _geoPos.lat);
    StateReporter::GetInstance().UpdateKey(prefix + "longitude", _geoPos.lon);
}

void Localization::ResetHeading() {
    _rsOffset = _vision->GetHeading();
    _imuOffset = _imu->GetYaw();
}

void Localization::ResetPose() { _pose = {{0.0, 0.0}, _pose.heading}; }