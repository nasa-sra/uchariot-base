
#include <math.h>
#include "subsystems/GPS.h"

#define MODE_STR_NUM 4
static std::string mode_str[MODE_STR_NUM] = {
    "n/a",
    "None",
    "2D",
    "3D"
};

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
            // did not even get mode, nothing to see here
            continue;
        }
        if (0 > _gps_data.fix.mode || MODE_STR_NUM <= _gps_data.fix.mode) {
            _gps_data.fix.mode = 0;
        }
        printf("Fix mode: %s (%d) Time: ", mode_str[_gps_data.fix.mode].c_str(), _gps_data.fix.mode);
        if (TIME_SET == (TIME_SET & _gps_data.set)) {
            // not 32 bit safe
            printf("%ld.%09ld ", _gps_data.fix.time.tv_sec, _gps_data.fix.time.tv_nsec);
        } else {
            puts("n/a ");
        }
        if (isfinite(_gps_data.fix.latitude) && isfinite(_gps_data.fix.longitude)) {
            // Display data from the GPS receiver if valid.
            printf("Lat %.6f Lon %.6f\n", _gps_data.fix.latitude, _gps_data.fix.longitude);
        } else {
            printf("Lat n/a Lon n/a\n");
        }
    }
}

void GPS::Disconnect() {
    (void)gps_stream(&_gps_data, WATCH_DISABLE, NULL);
    (void)gps_close(&_gps_data);
}

void GPS::ReportState(std::string prefix) {
    prefix += "GPS/";
//   StateReporter::GetInstance().UpdateKey(prefix + "yaw", _gyroAngles.z());
}
