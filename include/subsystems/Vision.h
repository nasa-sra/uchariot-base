#pragma once

#include <mutex>
#include <vector>

#include "subsystems/SubsystemBase.h"
#include "MessageQueue.h"
#include "Utils.h"

struct Detection {
    std::string name;
    Eigen::Vector3d pose;

    Detection() : name(""), pose({0.0, 0.0, 0.0}) {};
};

class Vision : SubsystemBase {
public:
    Vision();

    void Update(double dt);
    void Disconnect();
    void ReportState(std::string prefix = "/");

    double GetClosestDetectionDisance() { return _closestDetectionDistance; }
    std::vector<Detection> GetDetections() { return _detections; }

private:

    void updateDetections(std::string data);

    MessageQueue _messageQueue;

    std::vector<Detection> _detections;
    std::mutex _detectionsMutex;

    double _closestDetectionDistance{-1.0};
};