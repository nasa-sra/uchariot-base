
#include "subsystems/Localization.h"

Localization::Localization() {}

void Localization::Update(double dt, DriveBaseFeedback driveVels) {

    double vl = (driveVels.lf + driveVels.lb) / 2;
    double vr = (driveVels.rf + driveVels.rb) / 2;

    double v = (vl + vr) / 2.0;
    double omega = (vr - vl) / ROBOT_WIDTH;

    Eigen::Vector2d vel(v * cos(_pose.heading), v * sin(_pose.heading));
    _pose.pos += vel * dt;
    _pose.heading += omega * dt;

}

void Localization::ReportState(std::string prefix) {
    prefix += "localization/";
    StateReporter::GetInstance().UpdateKey(prefix + "x", _pose.pos.x());
    StateReporter::GetInstance().UpdateKey(prefix + "y", _pose.pos.y());
    StateReporter::GetInstance().UpdateKey(prefix + "heading", _pose.heading);
}