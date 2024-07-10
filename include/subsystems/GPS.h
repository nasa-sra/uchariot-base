#pragma once

#include "subsystems/SubsystemBase.h"
#include "StateReporter.h"
#include "Serial.h"
#include "Utils.h"
#include "nmea/nmea.h"

class GPS : public SubsystemBase {
public:
    GPS();
    ~GPS();
    void Update(double dt) override;
    void ReportState(std::string prefix = "/") override;
   
private:
    SerialConn _serial;
    nmeaINFO _info;
    nmeaPARSER _parser;

    void SendCommand(const std::string& str_cmd, bool do_checksum = true);
};
