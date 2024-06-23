#pragma once

#include <chrono>
#include <thread>
#include <cstdio>

#include "rapidjson/document.h"
#include "Base.h"
#include "controllers/EmptyController.h"
#include "controllers/TeleopController.h"

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

// This structure is just a container for all the various 
// subsystems, also just to keep the code organized.
struct Subsystems {
    DriveBase* drive;

    Subsystems() : drive(&DriveBase::GetInstance()) {}
};

// The main robot class is resonsible for:
// - timing update loops
// - switching active controllers
// - dispatching network handles
class Robot {
public:
    Robot() :
        _controllers(new Controllers()),
        _subsystems(new Subsystems()),
        _active_controller_name("empty"),
        _active_controller((ControllerBase*)_controllers->empty) {}

    void Run(int rate, bool& running);

    void HandleNetCmd(const std::string& cmd, rapidjson::Document& doc);

private:

    Controllers* _controllers;
    Subsystems* _subsystems;

    std::string _active_controller_name, _last_controller_name;
    ControllerBase* _active_controller;
};