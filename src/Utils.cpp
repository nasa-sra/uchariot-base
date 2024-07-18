
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


Eigen::Vector3d Utils::geoToECEF(GeoPoint point) {

	point.lat *= M_PI / 180;
	point.lon *= M_PI / 180;

	double distance = point.alt + EARTHS_RADIUS;
	return Eigen::Vector3d(cos(point.lon) * distance * cos(point.lat), sin(point.lon) * distance * cos(point.lat), distance * sin(point.lat));
}

Eigen::Vector3d Utils::geoToLTP(Utils::GeoPoint geo, Utils::GeoPoint geoOrigin) {

    Eigen::Vector3d pos = Utils::geoToECEF(geo);
    Eigen::Vector3d origin = Utils::geoToECEF(geoOrigin);

    // This is the solution to the intersection of each step pos vector with a plane normal to and thru the starting
    // point
    double t = origin.dot(origin) / origin.dot(pos);
    // This rescale step.pos to be on the plane, then transforms its origin to be from the starting point
    pos = t * pos - origin;

    // This should all be cached
    Eigen::Vector3d planeNormal = origin.normalized();
    Eigen::Matrix3d planeBasis;
    planeBasis.col(2) = planeNormal;                                 // up
    planeBasis.col(1) = planeNormal.cross(Eigen::Vector3d(0, 0, 1)); // west
    planeBasis.col(0) = planeBasis.col(1).cross(planeNormal);        // north

    Eigen::Matrix3d rotation = planeBasis.inverse(); // used identity for target basis
    // Rotate from the plane basis to a standard one, x=north, y=west, z=up
    return rotation * pos;
}

double Utils::PIDController::Calculate(double current, double target) {
    return _p * (target - current);
}