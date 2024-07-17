#include "subsystems/Localization.h"
#include "Utils.h"

Localization::Localization() {
}

void Localization::Update(double dt, DriveBaseFeedback driveVels, double heading_imu, double heading_rs) {

    double vl = (driveVels.lf + driveVels.lb) / 2;
    double vr = (driveVels.rf + driveVels.rb) / 2;

    double v = (vl + vr) / 2.0;
    double omega = (vr - vl) / ROBOT_WIDTH;

    Eigen::Vector2d vel(v * cos(_pose.heading), v * sin(_pose.heading));

    _pose.pos += vel * dt;

	Utils::LogFmt("IMU %.4f    RS %.4f    ERR %.4f", heading_imu, heading_rs, heading_imu - heading_rs); 

#ifdef USE_ODOM_ROT
    _heading_odom += omega * dt;
#endif
    _pose.heading += 0; // the real odom
}

void Localization::ReportState(std::string prefix) {
    prefix += "localization/";
    StateReporter::GetInstance().UpdateKey(prefix + "x", _pose.pos.x());
    StateReporter::GetInstance().UpdateKey(prefix + "y", _pose.pos.y());
    StateReporter::GetInstance().UpdateKey(prefix + "heading", _pose.heading);
}
