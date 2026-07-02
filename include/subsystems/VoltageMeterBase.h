#pragma once

#include "SubsystemBase.h"
#include "StateReporter.h"

//
class VoltageMeterBase : public SubsystemBase {
   public:
    VoltageMeter();
    void Update(double dt) override;
    void ReportState(std::string prefix = "/") {
        prefix += "voltageMeter/";
        StateReporter::GetInstance().UpdateKey(prefix + "voltage", _voltage);
        StateReporter::GetInstance().UpdateKey(prefix + "current", _current);
        StateReporter::GetInstance().UpdateKey(prefix + "power", _power);
    };

    float GetVoltage() { return _voltage; }
    float GetCurrent() { return _current; }
    float GetPower() { return _power; }

   private:
    float _voltage = 0.0f;
    float _current = 0.0f;
    float _power = 0.0f;
};

class SimVoltageMeter : public VoltageMeter {
   public:
    void Update(double dt) override;
};