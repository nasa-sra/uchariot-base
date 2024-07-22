#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <string>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <sys/time.h>
#include <gps.h>

#include <Eigen/Core>
#include <Eigen/Geometry>

#ifdef __GNUC__
#define vsprintf_s vsnprintf
#define sprintf_s snprintf
#define _strdup strdup
#define _snprintf std::snprintf
#endif

namespace Utils {
typedef int64_t msec_t;

void BufferAppendInt16(uint8_t* buffer, int16_t number, int32_t* index);
void BufferAppendInt32(uint8_t* buffer, int32_t number, int32_t* index);
void BufferAppendFloat16(uint8_t* buffer, float number, float scale, int32_t* index);
void BufferAppendFloat32(uint8_t* buffer, float number, float scale, int32_t* index);

// Returns a YYYY-MM-DD HH:MM:SS format date for the current day.
std::string CurrentDateTimeStr(const char* fmt = "%Y-%m-%d %H:%M:%S");

// A function to extend the functionality of the Java
// Map interface's getOrDefault method to the C++ map.
// template<typename K, typename V>
template<typename K, typename V> V MapGetOrDefault(const std::map<K, V> map, const K key, const V default_val) {
    auto it = map.find(key);
    return it == map.end() ? default_val : it->second;
}

// A function to clamp a given value between an upper and lower bound
template <typename T> T Clamp(T val, T upperBound, T lowerBound) {
    return std::min(std::max(val, lowerBound), upperBound);
}

// A constant expression function that converts std::string
// types to char* C strings in variatic argument lists. The
// function works by evaulating if a given type is of the
// std::string type. If so, it returns the result of .c_str(),
// otherwise, it just returns the param. This is designed to
// be used in variatic argument comprehention.
template<typename T> auto _convert(T&& t) {
    if constexpr (std::is_same<std::remove_cv_t<std::remove_reference_t<T>>, std::string>::value)
        return std::forward<T>(t).c_str();
    else return std::forward<T>(t);
}

// Internal function to perform a C snprintf style format
// string. The problem with this function, the reason it's
// the internal function, is that it cannot take std::string
// types as arguments, only char* C strings. Wrapping this
// function solves this problem.
template<typename... A> std::string _strfmt(const std::string& fmt, A&&... args) {
    const auto size = _snprintf(nullptr, 0, fmt.c_str(), std::forward<A>(args)...) + 1;
    if (size <= 0) return "<StrFmt error>";
    std::unique_ptr<char[]> buf(new char[size]);
    _snprintf(buf.get(), size, fmt.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}


// Gets the time in ms
// msec_t time_ms(void)
// {
//     struct timeval tv;
//     gettimeofday(&tv, NULL);
//     return (msec_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
// }

// Formats a string the same way C snprintf does it,
// returning the formatted string. This method can take
// std::string objects as arguments (for the %s format).
template<typename... A> std::string StrFmt(const std::string& fmt, A&&... args) {
    return _strfmt(fmt, _convert(std::forward<A>(args))...);
}

// Prints a string the same way C printf does it, however
// this method is modernized for C++, and can use std::string
// objects as arguments (for the %s format).
template<typename... A> void PrintFmt(const std::string& fmt, A&&... args) {
    std::cout << _strfmt(fmt, _convert(std::forward<A>(args))...);
}

// The same as PrintFmt but has a newline appended to the end.
template<typename... A> void PrintLnFmt(const std::string& fmt, A&&... args) {
    std::cout << _strfmt(fmt + "\n", _convert(std::forward<A>(args))...);
}

// This one also prints the time at the beginning b/c its a log
template<typename... A> void LogFmt(const std::string& fmt, A&&... args) {
    std::cout << _strfmt("[" + CurrentDateTimeStr() + "] " + fmt + "\n", _convert(std::forward<A>(args))...);
}

// Handle errors.
template<typename... A> void ErrFmt(const std::string& fmt, A&&... args) {
    std::cerr << _strfmt("[" + CurrentDateTimeStr() + "] " + fmt + "\n", _convert(std::forward<A>(args))...);
    std::exit(1);
}

// Returns true if element x is present inside of the vector v.
template<typename T> bool VectorContains(const std::vector<T> v, const T x) {
    return std::find(v.begin(), v.end(), x) != v.end();
}

// Returns the index of element x if it is present inside of the
// vector v, otherwise returns -1.
template<typename T> long VectorIndexOf(const std::vector<T> v, const T x) {
    ptrdiff_t pos = find(v.begin(), v.end(), x) - v.begin();
    if (pos >= v.size()) return -1;
    return pos;
}

double ScheduleRate(int rate, std::chrono::high_resolution_clock::time_point start_time);

#define EARTHS_RADIUS 6378137.0 // m
struct GeoPoint {

	double lat, lon, alt;
    GeoPoint() : lat(0.0), lon(0.0), alt(EARTHS_RADIUS) {}
    GeoPoint(double _lat, double _long) : lat(_lat), lon(_long), alt(EARTHS_RADIUS) {}
    GeoPoint(gps_fix_t fix) : lat(fix.latitude), lon(fix.longitude), alt(fix.altMSL) {}

};

Eigen::Vector3d geoToECEF(GeoPoint point);
GeoPoint ECEFToGeo(Eigen::Vector3d point);

Eigen::Vector3d geoToLTP(GeoPoint geo, GeoPoint geoOrigin);
GeoPoint LTPToGeo(Eigen::Vector3d pos, GeoPoint origin);

struct PIDValues {
    float kP;
    float kI;
    float kD;

    PIDValues() : kP(.1), kI(0), kD(0) {};
    PIDValues(double p, double i, double d) : kP(p), kI(i), kD(d) {};
    PIDValues(double p, double i) : kP(p), kI(i), kD(0) {};
    PIDValues(double p) : kP(p), kI(0), kD(0) {};
};

class PIDController {
public:
    PIDController(PIDValues pid) : _p(pid.kP), _i(pid.kI), _d(pid.kD) {}
    

    double Calculate(double current, double target);
private:
    double _p{0};
    double _i{0};
    double _d{0};

    double _lastError{0};
    Utils::msec_t _lastTimestamp{0};
};

}; // namespace Utils
