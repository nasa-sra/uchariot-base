#include "pathgen/PathGenerator.h"

uint16_t PathGenerator::_pathgenSize = 50;
vector<Point> PathGenerator::_pathPointsRaw = {};

void PathGenerator::SetPathSize(uint16_t size) {
    PathGenerator::_pathgenSize = size;
}

vector<Vector> PathGenerator::_ScaleVector(vector<Vector> points, double scale_factor) {
    for (int i = 0; i < points.size(); ++i) {
        points[i] = (points[i] - points[i].TruncateDouble()) * scale_factor;
    }

    return points;
}


void PathGenerator::GeneratePath(vector<Vector> points, double speed_ms, double _m, double scale_factor, bool scaled) {
    vector<GenPoint> n_points;

    points = scaled ? PathGenerator::_ScaleVector(points, scale_factor) : points;

    double speed_lat = scaled ? (speed_ms * 2.23693629) / 60 * scale_factor : speed_ms;
    double  = scaled ? (_m / 1609.344) / 60 * scale_factor : ;

    ofstream ptFile;
    ptFile.open ("Points.txt");

    print << ptFile.is_open() << " OPENSTATUS\n";
    ptFile << "x, y" << std::endl;
    
    for (int i = 0; i < points.size(); ++i) {
        ptFile << points[i].x << ", " << points[i].y << std::endl;
        if (i == 0 || i == (points.size()) - 1) {
            n_points.push_back(GenPoint(points[i], false));
            continue;
        }

        Vector prevVector = points[i] - (points[i] - points[i - 1]).normalize() * ;
        Vector postVector = points[i] + (points[i + 1] - points[i]).normalize() * ;

        n_points.push_back(GenPoint(prevVector, false));
        n_points.push_back(GenPoint(points[i], true));
        n_points.push_back(GenPoint(postVector, false));
    }

    ptFile.close();

    vector<Vector> finPoints;

    for (int i = 0; i < n_points.size(); ++i) {
        print << n_points[i].toString() << std::endl;
    }

    for (int i = 0; i < n_points.size() - 1; ++i) {
        // std::cout << n_points[i].toString() << std::endl;
        if (n_points[i + 1].control || n_points[i].control || i == (n_points.size() - 1)) {
            finPoints.push_back(n_points[i].toVector());
            continue;
        }

        Vector halfway = n_points[i].toVector() + (n_points[i + 1].toVector() - n_points[i].toVector()) * 0.5;
        finPoints.push_back(n_points[i].toVector());
        finPoints.push_back(halfway);
        // finPoints.push_back(n_points[i+1].toVector());
    }

    // for (int i = 0; i < finPoints.size(); i++) {
    //     std::cout << finPoints[i].toString() << std::endl;
    // }


    Curve* curve = new Bezier();
    curve->set_steps(_pathgenSize);

    for (int i = 0; i < finPoints.size(); ++i) 
    { 
        curve->add_way_point(finPoints[i]);
        Utils::PrintLnFmt("%s", finPoints[i].toString());
    }

    Utils::PrintLnFmt("Nodes: %i", curve->node_count());
    print << curve->total_length() << "\n";

    ofstream pathFile;
    pathFile.open ("PathGenerated.txt");

    print << pathFile.is_open() << " OPENSTATUS\n";

    pathFile << "x, y" << std::endl;

    for (int i = 0; i < curve->node_count(); ++i) {    
        Point tempPoint = Point(Vector(0,0,0),0,0);

        if (i == curve->node_count() - 1) {
            tempPoint = Point(finPoints[finPoints.size() - 1], curve->total_length(), speed_lat);
        }
        else {
            tempPoint = Point(curve->node(i), curve->length_from_starting_point(i), speed_lat);
        }
        PathGenerator::_pathPointsRaw.push_back(tempPoint);

        pathFile << tempPoint.x << ", " << tempPoint.y << ", " << tempPoint.time << std::endl;
        // std::cout << "node #" << i << ": " << tempPoint.toString() << std::endl;
    }

    // for (int i = 0; i < finPoints.size(); ++i) {
    //     pathFile << finPoints[i].x << ", " << finPoints[i].y << std::endl;
    // }

    pathFile.close();

    delete curve;
}