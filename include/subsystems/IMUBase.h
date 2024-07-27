#pragma once

#include <Eigen/Core>

#include "StateReporter.h"
#include "Utils.h"
#include "subsystems/SubsystemBase.h"

class IMUBase : SubsystemBase {
   public:
    virtual void Update(double dt) = 0;
    virtual void ReportState(std::string prefix = "/") {
        prefix += "imu/";
        StateReporter::GetInstance().UpdateKey(prefix + "yaw", _gyroAngles.z());
        StateReporter::GetInstance().UpdateKey(prefix + "pitch",
                                               _gyroAngles.y());
        StateReporter::GetInstance().UpdateKey(prefix + "roll",
                                               _gyroAngles.x());
    }

    float GetRoll() { return _gyroAngles.x(); }
    float GetPitch() { return _gyroAngles.y(); }
    float GetYaw() { return _gyroAngles.z(); }

   protected:
    Eigen::Vector3f _gyroAngles;
};

class SimIMU : public IMUBase {
   public:
    void Update(double dt) { _gyroAngles = {0.0, 0.0, 0.0}; }
};