#pragma once

#include "SubsystemBase.h"
#include "StateReporter.h"

//
class VoltageMeterBase : public SubsystemBase {
   public:
    VoltageMeterBase();
    void Update(double dt) override;
    void ReportState(std::string prefix = "/") {
        prefix += "voltageMeter/";
        StateReporter::GetInstance().UpdateKey(prefix + "voltage", _voltage);
        StateReporter::GetInstance().UpdateKey(prefix + "current", _current);
        StateReporter::GetInstance().UpdateKey(prefix + "power", _power);
    };

    virtual float GetVoltage() { return _voltage; }
    virtual float GetCurrent() { return _current; }
    virtual float GetPower() { return _power; }

   protected:
    float _voltage = 0.0f;
    float _current = 0.0f;
    float _power = 0.0f;
};

class SimVoltageMeter : public VoltageMeterBase {
   public:
    void Update(double dt) override;
};