#include "pathgen/PathGenerator.h"

#include "tinyxml2.h"

using namespace tinyxml2;

uint16_t PathGenerator::_pathgenSize = 50;
std::vector<Point> PathGenerator::_pathPointsRaw = {};

void PathGenerator::SetPathSize(uint16_t size) {
    PathGenerator::_pathgenSize = size;
}

int PathGenerator::GeneratePath(double speed_ms, double radius_m, std::string filename) {
    std::vector<GenPoint> n_points;

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

    std::vector<Eigen::Vector3d> points;

    while (pStream >> coord) {
        std::stringstream ss(coord);
        std::string lat, long_g;
        std::getline(ss, lat, ',');
        std::getline(ss, long_g, ',');

        Eigen::Vector3d point(std::stod(lat), std::stod(long_g), 0);
        points.push_back(point);
    }

    double speed_lat = (speed_ms * 2.23693629) / 60;
    double radius_lat = (radius_m / 1609.344) / 60;

    std::ofstream ptFile;
    ptFile.open("Points.txt");

    std::cout << ptFile.is_open() << " OPENSTATUS\n";
    ptFile << "x, y" << std::endl;

    for (int i = 0; i < points.size(); ++i) {
        ptFile << points[i].x() << ", " << points[i].y() << std::endl;
        if (i == 0 || i == (points.size()) - 1) {
            n_points.push_back(GenPoint(points[i], false));
            continue;
        }

        Eigen::Vector3d prevVector = points[i] - (points[i] - points[i - 1]).normalized() * radius_m;
        Eigen::Vector3d postVector = points[i] + (points[i + 1] - points[i]).normalized() * radius_m;

        n_points.push_back(GenPoint(prevVector, false));
        n_points.push_back(GenPoint(points[i], true));
        n_points.push_back(GenPoint(postVector, false));
    }

    ptFile.close();

    std::vector<Eigen::Vector3d> finPoints;

    for (int i = 0; i < n_points.size(); ++i) { 
        std::cout << n_points[i].toString() << std::endl; 
    }

    for (int i = 0; i < n_points.size() - 1; ++i) {
        if (n_points[i + 1].control || n_points[i].control || i == (n_points.size() - 1)) {
            finPoints.push_back(n_points[i].toVector());
            continue;
        }

        Eigen::Vector3d halfway = n_points[i].toVector() + (n_points[i + 1].toVector() - n_points[i].toVector()) * 0.5;
        finPoints.push_back(n_points[i].toVector());
        finPoints.push_back(halfway);
    }

    Curve* curve = new Bezier();
    curve->set_steps(_pathgenSize);

    for (int i = 0; i < finPoints.size(); ++i) {
        curve->add_way_point(finPoints[i]);
    }

    Utils::PrintLnFmt("Nodes: %i", curve->node_count());
    std::cout << curve->total_length() << "\n";

    std::ofstream pathFile;
    pathFile.open("PathGenerated.txt");

    std::cout << pathFile.is_open() << " OPENSTATUS\n";

    pathFile << "x, y" << std::endl;

    std::string pathFinalString;

    for (int i = 0; i < curve->node_count(); ++i) {
        Point tempPoint = Point(Eigen::Vector3d(0, 0, 0), 0, 0);

        if (i == curve->node_count() - 1) {
            tempPoint = Point(finPoints[finPoints.size() - 1], curve->total_length(), speed_lat);
            pathFinalString.append(tempPoint.ToPointString());
        } else {
            tempPoint = Point(curve->node(i), curve->length_from_starting_point(i), speed_lat);
            pathFinalString.append(tempPoint.ToPointString() + " ");
        }
        PathGenerator::_pathPointsRaw.push_back(tempPoint);

        pathFile << tempPoint.x << ", " << tempPoint.y << ", " << tempPoint.time << std::endl;
    }

    FILE *fp = fopen(("paths/" + filename + ".xml").c_str(), "w");
    if (fp == NULL) {
        Utils::LogFmt("Write Failed");
        return -3;
    }
    XMLPrinter printer(fp);

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
        pathFile << finPoints[i].x() << ", " << finPoints[i].y() << std::endl;
    }

    pathFile.close();

    delete curve;
    return 0;
}