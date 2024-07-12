#pragma once

#include <gps.h>

#include "subsystems/SubsystemBase.h"
#include "Utils.h"
#include "StateReporter.h"

class GPS : SubsystemBase {
public:
    
    GPS();

    void Update(double dt);
    void ReportState(std::string prefix = "/");
    void Disconnect();

private:

    bool _connected{false};
    struct gps_data_t _gps_data;

};