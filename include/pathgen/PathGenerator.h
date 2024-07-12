#include "Utils.h"
#include "pathgen/Bezier.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

struct GenPoint {
    double x;
    double y;
    bool control;

    GenPoint(Eigen::Vector3d point, bool isControl) : x(point.x()), y(point.y()), control(isControl) {};
    GenPoint(double x, double y, bool isControl) : x(x), y(y), control(isControl) {};

    Eigen::Vector3d toVector() {
        return Eigen::Vector3d(x, y, 0);
    };

    std::string toString() {
        std::ostringstream oss;
        oss << "[" << x << ", " << y << "]" << " : " << control;
        return oss.str();
    }
};

struct Point {
    double x;
    double y;
    double distFromStart;
    double time;

    Point(double x, double y, double dist, double speed) : x(x), y(y), distFromStart(dist), time(dist / speed) {};
    Point(Eigen::Vector3d vector, double dist, double speed) : x(vector.x()), y(vector.y()), distFromStart(dist), time(dist / speed) {};

    std::string ToString() {
        std::ostringstream oss;
        oss << "[" << x << ", " << y << "]" << " - " << distFromStart << "m - " << time << "s";
        return oss.str();
    }

    std::string ToPointString() {
        std::ostringstream oss;
        oss << x << "," << y;
        return oss.str();
    }
};

class PathGenerator {
public:
    static int GeneratePath(std::string filename, double speed_ms, double radius_m);
    static void SetPathSize(uint16_t size);

    static std::vector<Utils::GeoPoint> GetRawPoints();

private:
    static uint16_t _pathgenSize;
    static std::vector<Point> _pathPointsRaw;

    static std::vector<Eigen::Vector3d> _ScaleVector(std::vector<Eigen::Vector3d> points, double scale_factor);
};