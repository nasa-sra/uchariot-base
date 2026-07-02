
#include "SubsystemBase.h"

//
class VoltageMeter : public SubsystemBase {
   public:
    VoltageMeter();
    void Update(double dt) override;
    void ReportState(std::string prefix = "/") {
        prefix += "voltageMeter/";
        StateReporter::GetInstance().UpdateKey(prefix + "voltage", _voltage);
        StateReporter::GetInstance().UpdateKey(prefix + "current", _current);
        StateReporter::GetInstance().UpdateKey(prefix + "power", _power);
    };

   private:
    float _voltage = 0.0f;
    float _current = 0.0f;
    float _power = 0.0f;
};

class SimVoltageMeter : public VoltageMeter {
   public:
    void Update(double dt) override;
};