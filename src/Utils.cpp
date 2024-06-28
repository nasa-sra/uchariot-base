
#include <thread>

#include "Utils.h"

void Utils::BufferAppendInt16(uint8_t* buffer, int16_t number, int32_t *index) {
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}

void Utils::BufferAppendInt32(uint8_t* buffer, int32_t number, int32_t *index) {
	buffer[(*index)++] = number >> 24;
	buffer[(*index)++] = number >> 16;
	buffer[(*index)++] = number >> 8;
	buffer[(*index)++] = number;
}

void Utils::BufferAppendFloat16(uint8_t* buffer, float number, float scale, int32_t *index) {
    BufferAppendInt16(buffer, (int16_t)(number * scale), index);
}

void Utils::BufferAppendFloat32(uint8_t* buffer, float number, float scale, int32_t *index) {
    BufferAppendInt32(buffer, (int32_t)(number * scale), index);
}

std::string Utils::CurrentDateTimeStr(const char* fmt) {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), fmt, &tstruct);
    return buf;
}

// Used to enforce a rate (Hz) on a loop, returns dt in s
double Utils::ScheduleRate(int rate, std::chrono::high_resolution_clock::time_point start_time) {
    int dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
    if (dt < 1000 / rate) {
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0 / rate - dt - 2)));
    } else {
        return dt / 1000.0;
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count() / 1000.0;
}