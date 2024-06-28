#pragma once

#include <chrono>
#include <thread>
#include <cstdio>

#include "rapidjson/document.h"

#include "controllers/EmptyController.h"
#include "controllers/TeleopController.h"
#include "StateReporter.h"

#ifndef SIMULATION
#include "subsystems/LSM6DSOX.h"
#else
#include "subsystems/IMUBase.h"
#endif

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

#ifndef SIMULATION
    LSM6DSOX _imu;
#else
    SimIMU _imu;
#endif

    std::string _active_controller_name, _last_controller_name;
    ControllerBase* _active_controller;

};