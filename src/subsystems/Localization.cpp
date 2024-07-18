#include "subsystems/Localization.h"
#include "Utils.h"

Localization::Localization(DriveBase* driveBase, IMUBase* imu, Vision* vision) {
    _driveBase = driveBase;
    _imu = imu;
    _vision = vision;
}

void Localization::Update(double dt) {

    DriveBaseFeedback driveVels = _driveBase->GetVelocities();

    double vl = (driveVels.lf + driveVels.lb) / 2;
    double vr = (driveVels.rf + driveVels.rb) / 2;

    double v = (vl + vr) / 2.0;
    double omega = (vr - vl) / ROBOT_WIDTH;

    Eigen::Vector2d vel(v * cos(_pose.heading), v * sin(_pose.heading));

    _pose.pos += vel * dt;

	//Utils::LogFmt("IMU %.4f    RS %.4f    ERR %.4f", heading_imu, heading_rs, heading_imu - heading_rs); 

#ifdef USE_ODOM_ROT
    _pose.heading += omega * dt; // For running on cart/sim
#else
    _pose.heading = _vision->GetHeading() - _rsOffset; // with realsense gyro
    // _pose.heading = _imu->GetYaw() - _imuOffset; // with BNO055
#endif
}
// 
void Localization::ReportState(std::string prefix) {
    prefix += "localization/";
    StateReporter::GetInstance().UpdateKey(prefix + "x", _pose.pos.x());
    StateReporter::GetInstance().UpdateKey(prefix + "y", _pose.pos.y());
    StateReporter::GetInstance().UpdateKey(prefix + "heading", _pose.heading);
}

void Localization::ResetHeading() {
    _rsOffset = _vision->GetHeading();
    _imuOffset = _imu->GetYaw();
}

void Localization::ResetPose() {
    _pose = {{0.0, 0.0}, _pose.heading};
}