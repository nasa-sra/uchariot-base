
#include "subsystems/GPS.h"

#include <math.h>

// #define VERBOSE

#define MODE_STR_NUM 4

static std::string mode_str[MODE_STR_NUM] = {"n/a", "None", "2D", "3D"};

GPS::GPS() {
    if (0 != gps_open("localhost", "2947", &_gps_data)) {
        Utils::LogFmt("GPS: Failed to connect to gpsd server");
    } else {
        _connected = true;
        Utils::LogFmt("GPSD Server Connected");
    }

    (void)gps_stream(&_gps_data, WATCH_ENABLE | WATCH_JSON, NULL);
}

void GPS::Update(double dt) {
    while (_connected && gps_waiting(&_gps_data, 1000)) {
        if (-1 == gps_read(&_gps_data, NULL, 0)) {
            Utils::LogFmt("GPS: Read error");
            break;
        }
        if (MODE_SET != (MODE_SET & _gps_data.set)) {
#ifdef VERBOSE  // not necessarily an error condition
            Utils::LogFmt("GPS: Did not get a mode");
#endif
            continue;
        }
        if (0 > _gps_data.fix.mode || MODE_STR_NUM <= _gps_data.fix.mode) {
            _gps_data.fix.mode = 0;
        }
#ifdef VERBOSE  // not an error condition -- acceptable for normal operation
        Utils::LogFmt("Fix mode: %s (%d)", mode_str[_gps_data.fix.mode].c_str(),
                      _gps_data.fix.mode);
#endif
        if (TIME_SET == (TIME_SET & _gps_data.set)) {
            // not 32 bit safe
#ifdef VERBOSE  // not an error condition
            Utils::LogFmt("Time: %ld.%09ld ", _gps_data.fix.time.tv_sec,
                          _gps_data.fix.time.tv_nsec);
#endif
        } else {
#ifdef VERBOSE
            Utils::LogFmt("Time: n/a");
#endif
        }
        if (isfinite(_gps_data.fix.latitude) &&
            isfinite(_gps_data.fix.longitude)) {
            _lastFix = _gps_data.fix;
            // Display data from the GPS receiver if valid.
#ifdef VERBOSE  // not an error condition
            Utils::LogFmt("Lat %.6f Lon %.6f\n", _gps_data.fix.latitude,
                          _gps_data.fix.longitude);
#endif
        } else {
#ifdef VERBOSE
            Utils::LogFmt("Lat n/a Lon n/a\n");
#endif
        }
    }
}

void GPS::Disconnect() {
    (void)gps_stream(&_gps_data, WATCH_DISABLE, NULL);
    (void)gps_close(&_gps_data);
}

void GPS::ReportState(std::string prefix) {
    prefix += "GPS/";
    StateReporter::GetInstance().UpdateKey(prefix + "latitude", std::to_string(_lastFix.latitude));
    StateReporter::GetInstance().UpdateKey(prefix + "longitude", std::to_string(_lastFix.longitude));
    StateReporter::GetInstance().UpdateKey(prefix + "altitude", _lastFix.altMSL);
}