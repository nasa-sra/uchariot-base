
#include "subsystems/Vision.h"

#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

Vision::Vision() : 
    _network("VisionServer")
{
    _network.Start(9000, [this](std::string tag, rapidjson::Document &doc) {this->handleUpdate(tag, doc); });
};

void Vision::Update(double dt) {
    if (!_detectionsMutex.try_lock()) {
        Utils::LogFmt("Vision::Update failed to get detections lock");
        return;
    }

    _closestDetectionDistance = -1.0;
    for (Detection& det : _detections) {
        if (det.name == "closest") {
            _closestDetectionDistance = det.pose.z();
        }
    }
    _detectionsMutex.unlock();
}

void Vision::Disconnect() { _network.CloseConnections(); }

void Vision::handleUpdate(std::string tag, rapidjson::Document &doc) {
    std::vector<Detection> detections;

    try {
        if (doc.HasParseError())
            throw std::runtime_error(Utils::StrFmt(
                "JSON Parse Error offset %u: %s",
                (unsigned)doc.GetErrorOffset(),
                rapidjson::GetParseError_En(doc.GetParseError())));
        if (!doc.IsObject())
            throw std::runtime_error("JSON document malformed");
        if (!doc.HasMember("detections"))
            throw std::runtime_error("JSON missing detections");
        const rapidjson::Value& jsonDets = doc["detections"];
        if (!jsonDets.IsArray())
            throw std::runtime_error("JSON detections malformed");

        for (rapidjson::SizeType i = 0; i < jsonDets.Size(); ++i) {
            if (!jsonDets[i].HasMember("name") ||
                !jsonDets[i]["name"].IsString())
                throw std::runtime_error("JSON detection without proper name");
            if (!jsonDets[i].HasMember("x") || !jsonDets[i]["x"].IsDouble())
                throw std::runtime_error("JSON detection without proper x");
            if (!jsonDets[i].HasMember("y") || !jsonDets[i]["y"].IsDouble())
                throw std::runtime_error("JSON detection without proper y");

            Detection det;
            det.name = jsonDets[i]["name"].GetString();
            det.pose = Eigen::Vector3d(jsonDets[i]["x"].GetDouble(),
                                       jsonDets[i]["y"].GetDouble(),
                                       jsonDets[i]["z"].GetDouble());

            if (jsonDets[i].HasMember("confidence") && jsonDets[i]["confidence"].IsDouble()) {
                det.confidence = jsonDets[i]["confidence"].GetDouble();
            }
            if (jsonDets[i].HasMember("width") && jsonDets[i]["width"].IsDouble()) {
                det.width = jsonDets[i]["width"].GetDouble();
            }
            if (jsonDets[i].HasMember("height") && jsonDets[i]["height"].IsDouble()) {
                det.height = jsonDets[i]["height"].GetDouble();
            }
            detections.push_back(det);
        }
    } catch (std::exception& e) {
        Utils::LogFmt("Vision::updateDetections - Error: %s", e.what());
    }

    if (_detectionsMutex.try_lock()) {
        _detections = detections;
        _detectionsMutex.unlock();
    } else {
        Utils::LogFmt("Vision::updateDetections failed to get detections lock");
    }
}

void Vision::ReportState(std::string prefix) {
    prefix += "vision/";
    StateReporter::GetInstance().UpdateKey(prefix + "closest_detection_distance", _closestDetectionDistance);
    StateReporter::GetInstance().UpdateKey(prefix + "num_detections", int (_detections.size()));
}
