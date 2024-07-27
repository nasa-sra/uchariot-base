#pragma once

#include <gps.h>

#include "StateReporter.h"
#include "Utils.h"
#include "subsystems/SubsystemBase.h"

class GPS : SubsystemBase {
   public:
    GPS();

    void Update(double dt);
    void ReportState(std::string prefix = "/");
    void Disconnect();

    gps_fix_t GetFix() { return _lastFix; }

   private:
    bool _connected{false};
    gps_data_t _gps_data;
    gps_fix_t _lastFix;
};