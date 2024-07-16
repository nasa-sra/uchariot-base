#pragma once

#include <chrono>
#include <cstdio>
#include <thread>

#include "rapidjson/document.h"
#include "subsystems/DriveBase.h"

#include "StateReporter.h"
#include "controllers/EmptyController.h"
#include "controllers/TeleopController.h"

#include "subsystems/DriveBase.h"
#include "subsystems/GPS.h"
#ifndef SIMULATION
#include "subsystems/BNO055.h"
#else
#include "subsystems/IMUBase.h"
#endif

// This structure is just a container for all the various
// controllers to keep code organized.
struct Controllers {
    EmptyController* empty;
    TeleopController* teleop;

    Controllers() : empty(&EmptyController::GetInstance()), teleop(&TeleopController::GetInstance()) {
    }

    ControllerBase* FromName(const std::string& name);
};

// The main robot class is resonsible for:
// - timing update loops
// - switching active controllers
// - dispatching network handles
class Robot {
public:
    enum ControlMode { DISABLED, TELEOP, PATHING };

    Robot();

    void Run(int rate, bool& running);
    void Shutdown();

    void HandleNetCmd(const std::string& cmd, rapidjson::Document& doc);

private:
    void ManageController();
    ControlMode nameToMode(std::string name);
    ControllerBase& modeToController(ControlMode mode);

    ControlMode _mode{DISABLED};
    ControlMode _newMode{DISABLED};

    Controllers* _controllers;

    DriveBase _driveBase;
    GPS _gps;

    // #ifndef SIMULATION
    //     BNO055 _imu;
    // #else
    SimIMU _imu;
    // #endif

    std::string _active_controller_name, _last_controller_name;
    ControllerBase* _active_controller;
};