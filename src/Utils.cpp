
#include <thread>

#include "Utils.h"

/**
 * @brief Appends a 16-bit integer to a buffer at the specified index.
 *
 * This function takes a 16-bit integer, splits it into two 8-bit values, and appends them to a buffer at the specified
 * index. The index is then incremented by 2.
 *
 * @param buffer The buffer to which the integer will be appended.
 * @param number The 16-bit integer to be appended.
 * @param index A pointer to the index at which the integer will be appended. The index is updated after appending the
 * integer.
 */
void Utils::BufferAppendInt16(uint8_t* buffer, int16_t number, int32_t* index) {
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

/**
 * @brief Appends a 32-bit integer to a buffer at the specified index.
 *
 * This function takes a 32-bit integer, splits it into four 8-bit values, and appends them to a buffer at the specified
 * index. The index is then incremented by 4.
 *
 * @param buffer The buffer to which the integer will be appended. This buffer must be large enough to accommodate the
 * appended values.
 * @param number The 32-bit integer to be appended.
 * @param index A pointer to the index at which the integer will be appended. The index is updated after appending the
 * integer. This pointer must be valid and point to a valid memory location.
 *
 * @return void.
 */
void Utils::BufferAppendInt32(uint8_t* buffer, int32_t number, int32_t* index) {
    buffer[(*index)++] = number >> 24;
    buffer[(*index)++] = number >> 16;
    buffer[(*index)++] = number >> 8;
    buffer[(*index)++] = number;
}

/**
 * @brief Appends a 16-bit floating-point number to a buffer at the specified index, scaled by a given factor.
 *
 * This function takes a floating-point number, scales it by a given factor, converts it to a 16-bit integer, and
 * appends the resulting integer to a buffer at the specified index. The index is then incremented by 2.
 *
 * @param buffer The buffer to which the scaled floating-point number will be appended.
 * @param number The floating-point number to be appended.
 * @param scale The factor by which the floating-point number will be scaled before being converted to a 16-bit integer.
 * @param index A pointer to the index at which the scaled floating-point number will be appended. The index is updated
 * after appending the number.
 *
 * @return void.
 */
void Utils::BufferAppendFloat16(uint8_t* buffer, float number, float scale, int32_t* index) {
    BufferAppendInt16(buffer, (int16_t)(number * scale), index);
}

/**
 * @brief Appends a 32-bit floating-point number to a buffer at the specified index, scaled by a given factor.
 *
 * This function takes a floating-point number, scales it by a given factor, converts it to a 32-bit integer, and
 * appends the resulting integer to a buffer at the specified index. The index is then incremented by 4.
 *
 * @param buffer The buffer to which the scaled floating-point number will be appended. This buffer must be large enough
 * to accommodate the appended values.
 * @param number The floating-point number to be appended. This number will be multiplied by the scale factor before
 * being converted to a 32-bit integer.
 * @param scale The factor by which the floating-point number will be scaled before being converted to a 32-bit integer.
 * @param index A pointer to the index at which the scaled floating-point number will be appended. The index is updated
 * after appending the number. This pointer must be valid and point to a valid memory location.
 *
 * @return void.
 */
void Utils::BufferAppendFloat32(uint8_t* buffer, float number, float scale, int32_t* index) {
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

/**
 * @brief Enforces a rate (Hz) on a loop and returns the time elapsed since the last call.
 *
 * This function calculates the time elapsed since the last call and sleeps if necessary to enforce the specified rate.
 * If the elapsed time is greater than the specified rate, it returns the elapsed time in seconds.
 *
 * @param rate The desired rate in Hz.
 * @param start_time The time point from which to calculate the elapsed time.
 *
 * @return The time elapsed since the last call in seconds.
 */
double Utils::ScheduleRate(int rate, std::chrono::high_resolution_clock::time_point start_time) {
    int dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() -
                                                                   start_time)
                     .count();
    if (dt < 1000 / rate) {
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0 / rate - dt - 2)));
    } else {
        return dt / 1000.0;
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time)
                   .count() /
           1000.0;
}

/**
 * @brief Converts a geographic coordinate (latitude, longitude, altitude) to Earth-Centered, Earth-Fixed (ECEF)
 * coordinates.
 *
 * This function takes a geographic coordinate in the form of a latitude, longitude, and altitude, and converts it to
 * Earth-Centered, Earth-Fixed (ECEF) coordinates. The ECEF coordinate system is a Cartesian coordinate system used to
 * describe the 3D position of objects in space.
 * https://en.wikipedia.org/wiki/Earth-centered,_Earth-fixed_coordinate_system
 *
 * @param point A structure containing the geographic coordinate (latitude, longitude, altitude) to be converted.
 *
 * @return A 3D vector representing the ECEF coordinates of the input geographic coordinate. The x-coordinate
 * corresponds to the north direction, the y-coordinate corresponds to the east direction, and the z-coordinate
 * corresponds to the up direction.
 */
Eigen::Vector3d Utils::geoToECEF(GeoPoint point) {

    point.lat *= M_PI / 180;
    point.lon *= M_PI / 180;

    double distance = point.alt + EARTHS_RADIUS;
    return Eigen::Vector3d(cos(point.lon) * distance * cos(point.lat), sin(point.lon) * distance * cos(point.lat),
                           distance * sin(point.lat));
}

/**
 * @brief Converts Earth-Centered, Earth-Fixed (ECEF)
 * coordinates to a geographic coordinate (latitude, longitude, altitude).
 *
 * This function takes a point in Earth-Centered, Earth-Fixed (ECEF) coordinates  and converts 
 * it to a geographical coordinate in the form of a latitude, longitude, and altitude.
 * https://en.wikipedia.org/wiki/Earth-centered,_Earth-fixed_coordinate_system
 *
 * @param point A 3D vector representing the ECEF coordinates of the input geographic coordinate. The x-coordinate
 * corresponds to the north direction, the y-coordinate corresponds to the east direction, and the z-coordinate
 * corresponds to the up direction.
 * 
 * @return A structure containing the geographic coordinate (latitude, longitude, altitude).
 *
 */
Utils::GeoPoint Utils::ECEFToGeo(Eigen::Vector3d point) {

    Utils::GeoPoint geo;

    double distance = point.norm();
    geo.lat = asin(point.z() / distance) * 180 / M_PI;
    geo.lon = atan2(point.y(), point.x()) * 180 / M_PI;
    geo.alt = distance - EARTHS_RADIUS;

    return geo;
}

/**
 * @brief Converts a geographic coordinate (latitude, longitude, altitude) to Local Tangent Plane (LTP) coordinates.
 *
 * This function takes a geographic coordinate in the form of a latitude, longitude, and altitude, and converts it to
 * Local Tangent Plane (LTP) coordinates. The LTP coordinate system is a Cartesian coordinate system used to describe
 * the 3D position of objects relative to a reference point.
 * https://en.wikipedia.org/wiki/Local_tangent_plane_coordinates
 *
 * @param geo A structure containing the geographic coordinate (latitude, longitude, altitude) to be converted.
 * @param geoOrigin A structure containing the geographic coordinate (latitude, longitude, altitude) of the reference
 * point.
 *
 * @return A 3D vector representing the LTP coordinates of the input geographic coordinate. The x-coordinate corresponds
 * to the north direction, the y-coordinate corresponds to the west direction, and the z-coordinate corresponds to the
 * up direction.
 */
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


/**
 * @brief Converts a Local Tangent Plane (LTP) coordinates to a geographic coordinate (latitude, longitude, altitude).
 *
 * This function takes a geographic coordinate in the form of a latitude, longitude, and altitude, and converts it to
 * Local Tangent Plane (LTP) coordinates. The LTP coordinate system is a Cartesian coordinate system used to describe
 * the 3D position of objects relative to a reference point.
 * https://en.wikipedia.org/wiki/Local_tangent_plane_coordinates
 *
 * @param pos A 3D vector representing the LTP coordinates of the input geographic coordinate. The x-coordinate corresponds
 * to the north direction, the y-coordinate corresponds to the west direction, and the z-coordinate corresponds to the
 * up direction.
 * @param geoOrigin A structure containing the geographic coordinate (latitude, longitude, altitude) of the reference
 * point.
 *
 * @return A structure containing the geographic coordinate (latitude, longitude, altitude).
 */
Utils::GeoPoint Utils::LTPToGeo(Eigen::Vector3d pos, Utils::GeoPoint geoOrigin) {

    Eigen::Vector3d origin = Utils::geoToECEF(geoOrigin);

    // This should all be cached
    Eigen::Vector3d planeNormal = origin.normalized();
    Eigen::Matrix3d planeBasis;
    planeBasis.col(2) = planeNormal;                                 // up
    planeBasis.col(1) = planeNormal.cross(Eigen::Vector3d(0, 0, 1)); // west
    planeBasis.col(0) = planeBasis.col(1).cross(planeNormal);        // north

    Eigen::Vector3d posECEF = planeBasis * pos;
    return Utils::ECEFToGeo(posECEF + origin);
}

double Utils::PIDController::Calculate(double current, double target) {
    return _p * (target - current);
}