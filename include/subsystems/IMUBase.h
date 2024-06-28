#pragma once

#include "subsystems/SubsystemBase.h"
#include "Utils.h"
#include "StateReporter.h"

class IMUBase : SubsystemBase {
  public:

    virtual void Update(double dt) = 0;
    virtual void ReportState(std::string prefix = "/") {
      // StateReporter::GetInstance().UpdateKey(prefix + "roll", _gyroRates.x);
      // StateReporter::GetInstance().UpdateKey(prefix + "pitch", _gyroRates.y);
      StateReporter::GetInstance().UpdateKey(prefix + "yaw", _gyroAngles.z);
    }

    float getRoll() {return _gyroAngles.x;}
    float getPitch() {return _gyroAngles.y;}
    float getYaw() {return _gyroAngles.z;}

protected:

    Utils::Vector3 _gyroAngles;
};

class SimIMU : public IMUBase {
public:
	void Update(double dt) {
		_gyroAngles = {0.0, 0.0, 0.0};
	}
};