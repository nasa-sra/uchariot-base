
#include "pathgen/PathGenerator.h"
#include "tinyxml2.h"

using namespace tinyxml2;

uint16_t PathGenerator::_pathgenSize = 50;
std::vector<Point> PathGenerator::_pathPointsRaw = {};

/**
 * @brief Sets the size of the path generator.
 *
 * This function sets the resolution of the path generator.
 * his is the number of points between two consecutive points in the path.
 * A higher value results in a more detailed path, while a lower value results in a less detailed path. The default
 * value is 50.
 *
 * @param size The new size of the path generator.
 */
void PathGenerator::SetPathSize(uint16_t size) {
    PathGenerator::_pathgenSize = size;
}

std::vector<Utils::GeoPoint> PathGenerator::GetRawPoints() {
    std::vector<Utils::GeoPoint> bob;

    for (int i = 0; i < _pathPointsRaw.size(); i++) {
        bob.push_back(Utils::GeoPoint(_pathPointsRaw[i].x, _pathPointsRaw[i].y));
    }

    return bob;
}

/**
 * @brief Generates a path from a KML file, applies a specified speed and radius, and saves the result as an XML file.
 *
 * This function reads a KML file, processes the path data, applies a specified speed and radius, and generates a new
 * path. The generated path is then saved as an XML file, along with additional attributes such as speed, tolerance, end
 * tolerance, velocity kp, and heading kp.
 *
 * @param filename The name of the KML file to load.
 * @param speed_ms The desired speed of the path in meters per second.
 * @param radius_m The radius to apply to the path, in meters.
 *
 * @return An integer representing the success of the operation:
 * - 0: The path was successfully generated and saved.
 * - -1: An error occurred while loading the KML file.
 * - -2: An error occurred while extracting the path data from the KML file.
 * - -3: An error occurred while writing the XML file.
 */
int PathGenerator::GeneratePath(std::string filename, double speed_ms, double radius_m) {
    std::vector<GenPoint> n_points;

    XMLDocument doc;
    int res = doc.LoadFile(("paths/" + filename.substr(0, filename.size() - 4) + ".kml").c_str());

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

    for (int i = 0; i < points.size(); ++i) {
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

    std::vector<Eigen::Vector3d> finPoints;

    for (int i = 0; i < n_points.size(); ++i) { std::cout << n_points[i].toString() << std::endl; }

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

    for (int i = 0; i < finPoints.size(); ++i) { curve->add_way_point(finPoints[i]); }

    Utils::PrintLnFmt("Nodes: %i", curve->node_count());
    std::cout << curve->total_length() << "\n";

    std::string pathFinalString = "";

    for (int i = 0; i < curve->node_count(); i++) {
        Point tempPoint = Point(Eigen::Vector3d(0, 0, 0), 0, 0);

        // print << i << " " << curve->node_count() << std::endl;

        if (i == (curve->node_count() - 1)) {
            tempPoint = Point(finPoints[finPoints.size() - 1], curve->total_length(), speed_lat);
            pathFinalString.append(tempPoint.ToPointString());
        } else {
            tempPoint = Point(curve->node(i), curve->length_from_starting_point(i), speed_lat);
            pathFinalString.append(tempPoint.ToPointString() + " ");
        }
        PathGenerator::_pathPointsRaw.push_back(tempPoint);
    }

    delete curve;

    FILE* fp = fopen(("paths/" + filename.substr(0, filename.size() - 4) + ".xml").c_str(), "w");
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

    printer.OpenElement("coordinates");
    printer.PushText(pathFinalString.c_str());
    printer.CloseElement();
    printer.CloseElement();

    return 0;
}