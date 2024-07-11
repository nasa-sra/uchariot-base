#include "subsystems/Localization.h"
#include "Utils.h"

Localization::Localization() {
}

void Localization::Update(double dt, DriveBaseFeedback driveVels) {

    double lSlipCoeff = abs(driveVels.lf - driveVels.lb) / std::max(driveVels.lb, driveVels.lf);
    double rSlipCoeff = abs(driveVels.rf - driveVels.rb) / std::max(driveVels.rb, driveVels.rf);

    _slipCoefficient = lSlipCoeff * (lSlipCoeff / (lSlipCoeff + rSlipCoeff)) +
                             rSlipCoeff * (rSlipCoeff / (lSlipCoeff + rSlipCoeff));

    double vl = (driveVels.lf + driveVels.lb) / 2;
    double vr = (driveVels.rf + driveVels.rb) / 2;

    double v = (vl + vr) / 2.0;
    double omega = (vr - vl) / ROBOT_WIDTH;

    Eigen::Vector2d vel(v * cos(_pose.heading), v * sin(_pose.heading));

    double notReallyStdDev = Utils::Clamp(0.5 - _slipCoefficient, 0.0, 0.5);
    _pose.pos += vel * dt * notReallyStdDev;
    _pose.heading += omega * dt * notReallyStdDev;
}

void Localization::ReportState(std::string prefix) {
    prefix += "localization/";
    StateReporter::GetInstance().UpdateKey(prefix + "x", _pose.pos.x());
    StateReporter::GetInstance().UpdateKey(prefix + "y", _pose.pos.y());
    StateReporter::GetInstance().UpdateKey(prefix + "heading", _pose.heading);
}