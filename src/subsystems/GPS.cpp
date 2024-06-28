#define _USE_MATH_DEFINES
#include <cmath>

#include "subsystems/GPS.h"

GPS::GPS() : _serial("/dev/ttyAMA0", 115200) {

    nmea_zero_INFO(&_info);
    nmea_parser_init(&_parser);

    SendCommand("PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0");
    SendCommand("PMTK220,500");
}

void GPS::SendCommand(const std::string& str_cmd, bool do_checksum) {
    _serial.Send("$");
    _serial.Send(str_cmd);
    if (do_checksum) {
        int checksum = 0;
        for (int i = 0; i < str_cmd.length(); i++) checksum ^= str_cmd[i];
        _serial.Send('*');
        _serial.Send(Utils::StrFmt("%02X", checksum));
    }
    _serial.Send("\r\n");
}

std::string GPS::ReadSentence() {
    std::string line = _serial.ReceiveLine();
    std::string data;
    for (int i = 1; i < line.length(); i++) { // start i=1 if need to strip prefix $
        if (line[i] == '*') break;
        data += line[i];
    }
    return data;
}

GPSSentence GPS::ParseSentence() {
    std::string raw = ReadSentence();
    int delim = raw.find_first_of(',');
    GPSSentence sentence;
    std::string type_sec = raw.substr(0, delim);
    if (type_sec[0] == 'P') {
        sentence._talker = type_sec.substr(0, 1);
        sentence._type = type_sec.substr(1);
    } else {
        sentence._talker = type_sec.substr(0, 2);
        sentence._type = type_sec.substr(2);
    }
    sentence._data = raw.substr(delim + 1);
    return sentence;
}

void GPS::Update(double dt) {
    std::string line = _serial.ReceiveLine();
    nmea_parse(&_parser, line.c_str(), (int)line.length(), &_info);

    Utils::LogFmt("lat %f lon %f", _info.lat, _info.lon);
}

void GPS::ReportState(std::string prefix) {
    prefix += "gps/";
}

GPS::~GPS() {
    nmea_parser_destroy(&_parser);
}