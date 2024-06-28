
#include "tinyxml2.h"
#include "controllers/PathingController.h"

void PathingController::Load() {
    _runningPath = false;
}
void PathingController::Unload() {};

ControlCmds PathingController::Run() {
    ControlCmds cmds;

    if (!_runningPath && _pathName != "") {
        _runningPath = loadPath(_pathName);
        /*
        TODO
            Path needs to get shifted by starting point 
            then projected into the plane normal to the starting point vector from the center of the earth
            (this would ignore the effects effects of the curvature of the earth)
        */
    }

    if (_runningPath) {
        // run path with peer pursuit
    }

    return cmds;
}

void PathingController::HandleNetworkInput(rapidjson::Document& doc) {
    _pathName = doc["path"].GetString();
}

bool PathingController::loadPath(std::string filePath) {

    _path.clear();

    printf("Loading Auton... \n");

    tinyxml2::XMLDocument doc;
	if (doc.LoadFile(filePath.c_str()) != tinyxml2::XML_SUCCESS) {
		Utils::LogFmt("PathingContoller::loadPath - Could not load file %s", filePath);
		return false;
	}

	tinyxml2::XMLElement* path = doc.FirstChildElement("path");
	if (path == nullptr) {
        Utils::LogFmt("PathingContoller::loadPath - No path element found");
		return false;
	}

    float pathSpeed = 0.5;
	path->QueryFloatAttribute("speed", &pathSpeed);

    tinyxml2::XMLElement* line = nullptr;
	while (true) {
		if (line == nullptr) {
			line = path->FirstChildElement();
		} else {
			line = line->NextSiblingElement();
		}

		if (line == nullptr) {
			break;
		}

		if (std::strcmp(line->Name(), "coordinates") == 0) {

			double speed = 0.0;
			line->QueryDoubleAttribute("speed", &speed);

			if (speed == 0.0) {
				speed = pathSpeed;
			}

			const char* coords = line->GetText();

			std::string str(coords);
			std::stringstream ss(str);
			std::string coord;

			while (ss >> coord) {

				Utils::GeoPoint point = parseCoordinates(coord, false);
				Eigen::Vector3d pos = Utils::geoToEarthCoord(point);

                PathStep step;
				step.pos = pos;
				step.speed = speed;
				step.geoPoint = point;
				_path.push_back(step);

			}
		} else {
            Utils::LogFmt("PathingContoller::loadPath - Could not read line in XML path");
		}
	}
    return true;
}

Utils::GeoPoint PathingController::parseCoordinates(std::string coords, bool altitude) {

	std::stringstream ss(coords);
	Utils::GeoPoint point;

	std::string lat, lon, alt;

    std::getline(ss, lat, ',');
    std::getline(ss, lon, ',');

	point.lat = std::stod(lat);
	point.lon = std::stod(lon);

    if (altitude) {
        std::getline(ss, alt, ',');
        point.alt = std::stod(alt);
    }
	
	return point;
}