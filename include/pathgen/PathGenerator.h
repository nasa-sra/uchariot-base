#include "Utils.h"
#include "pathgen/BSpline.h"
#include "pathgen/Bezier.h"
#include "pathgen/Vector.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#include <math.h>

using std::ofstream;
using std::vector;

#define print std::cout

struct GenPoint {
    double x;
    double y;
    bool control;

    GenPoint(Vector point, bool isControl) : x(point.x), y(point.y), control(isControl) {};
    GenPoint(double x, double y, bool isControl) : x(x), y(y), control(isControl) {};

    Vector toVector() {
        return Vector(x, y, 0);
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
    Point(Vector vector, double dist, double speed) : x(vector.x), y(vector.y), distFromStart(dist), time(dist / speed) {};

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
    static void GeneratePath(double speed_ms, double radius_m, double scale_factor, bool scaled, std::string filename);
    static void SetPathSize(uint16_t size);

private:
    static uint16_t _pathgenSize;
    static vector<Point> _pathPointsRaw;

    static vector<Vector> _ScaleVector(vector<Vector> points, double scale_factor);
};