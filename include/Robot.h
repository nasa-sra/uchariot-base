#pragma once

#include <chrono>
#include <cstdio>
#include <thread>

#include "MessageQueue.h"
#include "StateReporter.h"
#include "controllers/PathingController.h"
#include "controllers/TeleopController.h"
#include "rapidjson/document.h"
#include "subsystems/DriveBase.h"
#include "subsystems/GPS.h"
#include "subsystems/Localization.h"
#ifndef SIMULATION
#include "subsystems/BNO055.h"
#else
#include "subsystems/IMUBase.h"
#endif
#include "subsystems/Vision.h"

// The main robot class is resonsible for:
// - timing update loops
// - switching active controllers
// - dispatching network handles
class Robot {
   public:
    enum ControlMode { DISABLED, TELEOP, PATHING };

    Vision _vision;

    Robot();

    void Shutdown() {};

    void Run(int rate, bool &running);
    void HandleNetCmd(const std::string &cmd, rapidjson::Document &doc);

   private:
    void ManageController();
    ControlMode nameToMode(std::string name);
    ControllerBase &modeToController(ControlMode mode);

    ControlMode _mode{DISABLED};
    ControlMode _newMode{DISABLED};

    TeleopController _teleopController;
    PathingController _pathingController;

    DriveBase _driveBase;
#ifndef SIMULATION
    BNO055 _imu;
#else
    SimIMU _imu;
#endif
    GPS _gps;
    Localization _localization;

    std::string _active_controller_name, _last_controller_name;
    ControllerBase *_active_controller;
};