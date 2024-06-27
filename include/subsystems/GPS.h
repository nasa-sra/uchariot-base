#pragma once

#include "subsystems/SubsystemBase.h"
#include "StateReporter.h"
#include "Serial.h"
#include "Utils.h"
#include "nmea/nmea.h"

struct GPSSentence {
    std::string _talker;
    std::string _type;
    std::string _data;
};

class GPS : public SubsystemBase {
public:
    GPS();
    ~GPS();
    void Update() override;
    void ReportState(std::string prefix = "/") override;

    static GPS& GetInstance() {
        static GPS _instance;
        return _instance;
    }
   
private:
    SerialConn _serial;
    nmeaINFO _info;
    nmeaPARSER _parser;

    void SendCommand(const std::string& str_cmd, bool do_checksum = true);
};