#include <filesystem>

#include "pathgen/PathGenerator.h"
#include "tinyxml2.h"

#include "controllers/PathingController.h"

PathingController::PathingController(Localization* localization) : ControllerBase("pathing") {
    _localization = localization;
}

void PathingController::Load() {
    _runningPath = false;
    _currentStep = 0;
}
void PathingController::Unload() {};

ControlCmds PathingController::Run() {
    ControlCmds cmds;

    if (!_runningPath && _pathName != "") {
        _runningPath = loadPath(_pathName);
        if (!_runningPath) {
            _pathName = "";
        } else {
            Utils::LogFmt("Running Path ...");
            _localization->SetOrigin(_origin);
        }
    }

    if (_runningPath) {
        Pose robotPose = _localization->GetPose();
        _nextWaypoint = _path[_currentStep].pos.head<2>();
        Eigen::Vector2d diff = _nextWaypoint - robotPose.pos;
        _distanceToWaypoint = diff.norm();
        double velocity = std::clamp((float)(_distanceToWaypoint * _velocityGain), -_path[_currentStep].speed,
                                     _path[_currentStep].speed);
        _targetHeading = atan2(diff.y(), diff.x());
        double headingErr = _targetHeading - robotPose.heading;
        double angularVelocity = headingErr * _headingGain;

        if (_currentStep == _path.size() - 1 && _distanceToWaypoint < 1.0) { angularVelocity = 0.0; }

        cmds.drive = {velocity, angularVelocity};

        if (_distanceToWaypoint < _path[_currentStep].tolerance) {
            _currentStep++;
            if (_currentStep == _path.size()) { _runningPath = false; }
        }
    }

    return cmds;
}

void PathingController::ReportState(std::string prefix) {
    prefix += "pathing_controller/";
    StateReporter::GetInstance().UpdateKey(prefix + "waypointX", _nextWaypoint.x());
    StateReporter::GetInstance().UpdateKey(prefix + "waypointY", _nextWaypoint.y());
    StateReporter::GetInstance().UpdateKey(prefix + "targetHeading", _targetHeading);
    StateReporter::GetInstance().UpdateKey(prefix + "distanceToWaypoint", _distanceToWaypoint);
    StateReporter::GetInstance().UpdateKey(prefix + "runningPath", _runningPath);
    StateReporter::GetInstance().UpdateKey(prefix + "currentStep", _currentStep);
}

void PathingController::HandleNetworkInput(rapidjson::Document& doc) {
    _pathName = doc["name"].GetString();
    // _pathResolution = std::stoi(doc["res"].GetString());
    // _pathSpeed = std::stof(doc["speed"].GetString());
    // _pathRadius = std::stof(doc["rad"].GetString());
}

bool PathingController::loadPath(std::string filePath) {
    std::filesystem::path p(filePath);
    if (p.extension() == ".xml") {
        return loadXMLPath(filePath);
    } else if (p.extension() == ".kml") {
        return loadKMLPath(filePath);
    }
    return false;
}

bool PathingController::loadXMLPath(std::string filePath) {

    Utils::LogFmt("Loading XML Auton... ");

    _path.clear();
    tinyxml2::XMLDocument doc;

    filePath = "paths/" + filePath;

    int res = doc.LoadFile(filePath.c_str());
    if (res != tinyxml2::XML_SUCCESS) {
        Utils::LogFmt("PathingContoller::loadXMLPath - Could not load file %s, Err Code: %i", filePath.c_str(), res);
        return false;
    }

    tinyxml2::XMLElement* path = doc.FirstChildElement("path");
    if (path == nullptr) {
        Utils::LogFmt("PathingContoller::loadXMLPath - No path element found");
        return false;
    }

    float pathSpeed = 0.5;
    path->QueryFloatAttribute("speed", &pathSpeed);
    _velocityGain = 5.0;
    path->QueryDoubleAttribute("velocitykp", &_velocityGain);
    _headingGain = 1.0;
    path->QueryDoubleAttribute("headingkp", &_headingGain);
    float pathTolerance = 3.0;
    path->QueryFloatAttribute("tolerance", &pathTolerance);
    _endTolerance = 0.1;
    path->QueryFloatAttribute("endTolerance", &_endTolerance);

    Utils::GeoPoint origin;

    tinyxml2::XMLElement* line = nullptr;
    while (true) {
        if (line == nullptr) {
            line = path->FirstChildElement();
        } else {
            line = line->NextSiblingElement();
        }

        if (line == nullptr) { break; }

        if (std::strcmp(line->Name(), "coordinates") == 0) {

            double speed = 0.0;
            line->QueryDoubleAttribute("speed", &speed);
            float tolerance = 0.0;
            path->QueryFloatAttribute("tolerance", &tolerance);

            if (speed == 0.0) { speed = pathSpeed; }
            if (tolerance == 0.0) { tolerance = pathTolerance; }

            const char* coords = line->GetText();

            std::string str(coords);
            std::stringstream ss(str);
            std::string coord;

            while (ss >> coord) {

                Utils::GeoPoint point = parseGeoCoordinates(coord, false, true);
                if (_path.size() == 0) { _origin = point; }
                Eigen::Vector3d pos = geoToLTP(point, _origin);

                PathStep step;
                step.pos = pos;
                step.speed = speed;
                step.geoPoint = point;
                step.tolerance = tolerance;
                _path.push_back(step);
            }
        } else if (std::strcmp(line->Name(), "points") == 0) {

            double speed = 0.0;
            line->QueryDoubleAttribute("speed", &speed);
            float tolerance = 0.0;
            path->QueryFloatAttribute("tolerance", &tolerance);

            if (speed == 0.0) { speed = pathSpeed; }
            if (tolerance == 0.0) { tolerance = pathTolerance; }

            const char* coords = line->GetText();

            std::string str(coords);
            std::stringstream ss(str);
            std::string coord;

            while (ss >> coord) {

                PathStep step;
                step.pos = parseCoordinates(coord);
                ;
                step.speed = speed;
                step.tolerance = tolerance;
                _path.push_back(step);
            }
        } else {
            Utils::LogFmt("PathingContoller::loadXMLPath - Could not read line in XML path");
        }
    }
    _path.back().tolerance = _endTolerance;
    Utils::LogFmt("Loaded Auton with %i points", _path.size());
    return true;
}

void PathingController::Stop() {
    _runningPath = false;
    _pathName = "";
}

bool PathingController::loadKMLPath(std::string filePath) {

    Utils::LogFmt("Loading KML Auton... ");

    if (PathGenerator::GeneratePath(filePath, _pathSpeed, _pathRadius) != 0) return false;
    std::vector<Utils::GeoPoint> points = PathGenerator::GetRawPoints();

    _path.clear();
    for (Utils::GeoPoint& point : points) {
        Utils::GeoPoint origin = _path.size() > 0 ? _path[0].geoPoint : point;
        Eigen::Vector3d pos = Utils::geoToLTP(point, origin);

        PathStep step;
        step.pos = pos;
        step.speed = _pathSpeed;
        step.geoPoint = point;
        step.tolerance = 1.0;
        _path.push_back(step);
    }
    Utils::LogFmt("Loaded Auton");
    return true;
}

Eigen::Vector3d PathingController::parseCoordinates(std::string coords) {

    std::stringstream ss(coords);

    Eigen::Vector3d point;
    std::string x, y;

    std::getline(ss, x, ',');
    std::getline(ss, y, ',');

    point.x() = std::stod(x);
    point.y() = std::stod(y);

    return point;
}

Utils::GeoPoint PathingController::parseGeoCoordinates(std::string coords, bool altitude, bool flipped) {

    std::stringstream ss(coords);
    Utils::GeoPoint point;

    std::string lat, lon, alt;

    if (flipped) {
        std::getline(ss, lon, ',');
        std::getline(ss, lat, ',');
    } else {
        std::getline(ss, lat, ',');
        std::getline(ss, lon, ',');
    }

    point.lat = std::stod(lat);
    point.lon = std::stod(lon);

    if (altitude) {
        std::getline(ss, alt, ',');
        point.alt = std::stod(alt);
    }

    return point;
}
