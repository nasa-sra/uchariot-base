#pragma once

#include <chrono>
#include <thread>
#include <cstdio>

#include "rapidjson/document.h"
#include "subsystems/DriveBase.h"
#include "subsystems/GPS.h"

#include "controllers/EmptyController.h"
#include "controllers/TeleopController.h"
#include "StateReporter.h"
#include "subsystems/IMU.h"

// This structure is just a container for all the various
// controllers to keep code organized.
struct Controllers {
    EmptyController* empty;
    TeleopController* teleop;

    Controllers() :
        empty(&EmptyController::GetInstance()),
        teleop(&TeleopController::GetInstance())
        {}

    ControllerBase* FromName(const std::string& name);
};

// The main robot class is resonsible for:
// - timing update loops
// - switching active controllers
// - dispatching network handles
class Robot {
public:
    Robot();

    void Run(int rate, bool& running);

    void HandleNetCmd(const std::string& cmd, rapidjson::Document& doc);

private:

    void ManageController();

    Controllers* _controllers;
    
    DriveBase _driveBase;
    IMU _imu;
    GPS _gps;

    std::string _active_controller_name, _last_controller_name;
    ControllerBase* _active_controller;

};