#include "pathgen/PathGenerator.h"

#include "tinyxml2.h"

using namespace tinyxml2;

uint16_t PathGenerator::_pathgenSize = 50;
vector<Point> PathGenerator::_pathPointsRaw = {};

void PathGenerator::SetPathSize(uint16_t size) {
    PathGenerator::_pathgenSize = size;
}

vector<Vector> PathGenerator::_ScaleVector(vector<Vector> points, double scale_factor) {
    for (int i = 0; i < points.size(); ++i) { points[i] = (points[i] - points[i].TruncateDouble()) * scale_factor; }

    return points;
}

int PathGenerator::GeneratePath(std::string filename, double speed_ms, double radius_m) {
    vector<GenPoint> n_points;

    // points = PathGenerator::_ScaleVector(points, scale_factor);

    XMLDocument doc;
    int res = doc.LoadFile(("paths/" + filename + ".kml").c_str());

    if (res != tinyxml2::XML_SUCCESS) {
        Utils::LogFmt("PathingContoller::loadPath - Could not load file %s, Err Code: %i", filename, res);
        return -1;
    }

    XMLElement* path = doc.FirstChildElement("kml")
                               ->FirstChildElement("Document")
                               ->FirstChildElement("Folder")
                               ->FirstChildElement("Placemark")
                               ->FirstChildElement("LineString")
                               ->FirstChildElement("coordinates");

    if (path == nullptr) {
        Utils::LogFmt("PathingContoller::loadPath - Could not load data, Err Code: %i", res);
        return -2;
    }

    std::string pText = std::string(path->GetText());
    std::stringstream pStream(pText);
    std::string coord;

    vector<Vector> points;

    while (pStream >> coord) {
        std::stringstream ss(coord);
        std::string lat, long_g;
        std::getline(ss, lat, ',');
        std::getline(ss, long_g, ',');

        points.push_back(Vector(std::stod(lat), std::stod(long_g)));
    }

    double speed_lat = (speed_ms * 2.23693629) / 60;
    double radius_lat = (radius_m / 1609.344) / 60;

    ofstream ptFile;
    ptFile.open("Points.txt");

    print << ptFile.is_open() << " OPENSTATUS\n";
    ptFile << "x, y" << std::endl;

    for (int i = 0; i < points.size(); ++i) {
        ptFile << points[i].x << ", " << points[i].y << std::endl;
        if (i == 0 || i == (points.size()) - 1) {
            n_points.push_back(GenPoint(points[i], false));
            continue;
        }

        Vector prevVector = points[i] - (points[i] - points[i - 1]).normalize() * radius_m;
        Vector postVector = points[i] + (points[i + 1] - points[i]).normalize() * radius_m;

        n_points.push_back(GenPoint(prevVector, false));
        n_points.push_back(GenPoint(points[i], true));
        n_points.push_back(GenPoint(postVector, false));
    }

    ptFile.close();

    vector<Vector> finPoints;

    for (int i = 0; i < n_points.size(); ++i) { print << n_points[i].toString() << std::endl; }

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

    for (int i = 0; i < finPoints.size(); ++i) {
        curve->add_way_point(finPoints[i]);
        Utils::PrintLnFmt("%s", finPoints[i].toString());
    }

    Utils::PrintLnFmt("Nodes: %i", curve->node_count());
    print << curve->total_length() << "\n";

    ofstream pathFile;
    pathFile.open("PathGenerated.txt");

    print << pathFile.is_open() << " OPENSTATUS\n";

    pathFile << "x, y" << std::endl;

    std::string pathFinalString = "";

    for (int i = 0; i < curve->node_count(); i++) {
        Point tempPoint = Point(Vector(0, 0, 0), 0, 0);

        // print << i << " " << curve->node_count() << std::endl;

        if (i == (curve->node_count() - 1)) {
            tempPoint = Point(finPoints[finPoints.size() - 1], curve->total_length(), speed_lat);
            pathFinalString.append(tempPoint.ToPointString());
        } else {
            tempPoint = Point(curve->node(i), curve->length_from_starting_point(i), speed_lat);
            pathFinalString.append(tempPoint.ToPointString() + " ");
        }
        PathGenerator::_pathPointsRaw.push_back(tempPoint);

        pathFile << tempPoint.x << ", " << tempPoint.y << ", " << tempPoint.time << std::endl;

        // std::cout << "node #" << i << ": " << tempPoint.toString() << std::endl;
    }

    delete curve;

    FILE *fp = fopen(("paths/" + filename + ".xml").c_str(), "w");
    if (fp == NULL) {
        Utils::LogFmt("Write Failed");
        return -3;
    }

    XMLPrinter printer(fp);

    printer.PushHeader(false, true);
    // printer.PushAttribute("encoding", "utf-8");

    printer.OpenElement("path");
    printer.PushAttribute("name", filename.c_str());
    printer.PushAttribute("speed", std::to_string(speed_ms).c_str());
    printer.PushAttribute("tolerance", "1.0");
    printer.PushAttribute("endTolerance", "0.1");
    printer.PushAttribute("velocitykp", "0.5");
    printer.PushAttribute("headingkp", "1.0");

    printer.OpenElement("geoCoordinates");
    printer.PushText(pathFinalString.c_str());
    printer.CloseElement();
    printer.CloseElement();

    for (int i = 0; i < finPoints.size(); ++i) {
        pathFile << finPoints[i].x << ", " << finPoints[i].y << std::endl;
    }

    pathFile.close();

    return 0;
}