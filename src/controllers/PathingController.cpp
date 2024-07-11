
#include "tinyxml2.h"
#include <Eigen/Geometry>
#include "pathgen/PathGenerator.h"

#include "controllers/PathingController.h"

void PathingController::Load() {
    _runningPath = false;
	_currentStep = 0;
}
void PathingController::Unload() {};

ControlCmds PathingController::Run(Pose robotPose) {
    ControlCmds cmds;

    if (!_runningPath && _pathName != "") {
        _runningPath = loadPath(_pathName);
		if (!_runningPath)
			_pathName = "";
    }

    if (_runningPath) {
		_nextWaypoint = _path[_currentStep].pos.head<2>();
        Eigen::Vector2d diff = _nextWaypoint - robotPose.pos;
		double velocity = std::clamp((float) (diff.norm() * _velocityGain), -_path[_currentStep].speed, _path[_currentStep].speed);
		double headingErr = atan2(diff.y(), diff.x()) - robotPose.heading;
		double angularVelocity = headingErr * _headingGain;

		if (_currentStep == _path.size() - 1 && diff.norm() < 1.0) {
			angularVelocity = 0.0;
		}

		cmds.drive = DriveBaseCmds(angularVelocity, velocity);

		if (diff.norm() < _path[_currentStep].tolerance) {
			_currentStep++;
			if (_currentStep == _path.size()) {
				_runningPath = false;
			}
		}
    }

    return cmds;
}

void PathingController::ReportState(std::string prefix) {
	prefix += "pathing_controller/";
    StateReporter::GetInstance().UpdateKey(prefix + "waypointX", _nextWaypoint.x());
    StateReporter::GetInstance().UpdateKey(prefix + "waypointY", _nextWaypoint.y());
}

void PathingController::HandleNetworkInput(rapidjson::Document& doc) {
    _pathName = doc["name"].GetString();
	_pathResolution = doc["res"].GetInt(); 
}

bool PathingController::loadPath(std::string filePath) {

    _path.clear();

	tinyxml2::XMLDocument doc;

	Utils::LogFmt("Generating Auton...");
	PathGenerator::SetPathSize(_pathResolution);
	int res = PathGenerator::GeneratePath(2.0, 3.5, 1, true, filePath);

	if (res < 0) {
		Utils::LogFmt("Unable to locate correctly formatted .kml file.");
		return -1;
	}

    Utils::LogFmt("Loading Auton... ");

    
	res = doc.LoadFile(("paths/" + filePath + ".xml").c_str());
	if (res != tinyxml2::XML_SUCCESS) {
		Utils::LogFmt("PathingContoller::loadPath - Could not load file %s, Err Code: %i", filePath, res);
		return false;
	}

	tinyxml2::XMLElement* path = doc.FirstChildElement("path");
	if (path == nullptr) {
        Utils::LogFmt("PathingContoller::loadPath - No path element found");
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

		if (line == nullptr) {
			break;
		}

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
				Utils::GeoPoint origin = _path.size() > 0 ? _path[0].geoPoint : point;
				Eigen::Vector3d pos = geoToPathCoord(point, origin);

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
				step.pos = parseCoordinates(coord);;
				step.speed = speed;
				step.tolerance = tolerance;
				_path.push_back(step);

			}
		} else {
            Utils::LogFmt("PathingContoller::loadPath - Could not read line in XML path");
		}
	}
	_path.back().tolerance = _endTolerance;
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

Eigen::Vector3d PathingController::geoToPathCoord(Utils::GeoPoint geo, Utils::GeoPoint geoOrigin) {

	Eigen::Vector3d pos = Utils::geoToEarthCoord(geo);
	Eigen::Vector3d origin = Utils::geoToEarthCoord(geoOrigin);
	
	// This is the solution to the intersection of each step pos vector with a plane normal to and thru the starting point
	double t = origin.dot(origin) / origin.dot(pos);
	// This rescale step.pos to be on the plane, then transforms its origin to be from the starting point
	pos = t * pos - origin;

	// This should all be cached
	Eigen::Vector3d planeNormal = origin.normalized();
	Eigen::Matrix3d planeBasis;
	planeBasis.col(2) = planeNormal; // up
	planeBasis.col(1) = planeNormal.cross(Eigen::Vector3d(0, 0, 1)); // west
	planeBasis.col(0) = planeBasis.col(1).cross(planeNormal); // north

	Eigen::Matrix3d rotation = planeBasis.inverse(); // used identity for target basis
	// Rotate from the plane basis to a standard one, x=north, y=west, z=up
	return rotation * pos;

}
