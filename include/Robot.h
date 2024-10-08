#pragma once

#include <chrono>
#include <cstdio>
#include <thread>

#include "rapidjson/document.h"
#include "MessageQueue.h"
#include "StateReporter.h"

#include "controllers/OverrideController.h"
#include "controllers/PathingController.h"
#include "controllers/TeleopController.h"
#include "controllers/FollowingController.h"
#include "controllers/SummonController.h"

#include "subsystems/DriveBase.h"
#include "subsystems/GPS.h"
#include "subsystems/Vision.h"
#include "subsystems/Localization.h"
#ifndef SIMULATION
#include "subsystems/BNO055.h"
#else
#include "subsystems/IMUBase.h"
#endif

class Robot {
   public:
    enum ControlMode { DISABLED, TELEOP, PATHING, FOLLOWING, SUMMON };

    Vision _vision;

    Robot();

    void Run(int rate, bool &running);
    void HandleNetCmd(const std::string &cmd, rapidjson::Document &doc);

    void Shutdown();

   private:
   
    void ManageController();
    ControlMode nameToMode(std::string name);
    ControllerBase &modeToController(ControlMode mode);
    bool loadConfig(std::string filePath);

    ControlMode _mode{DISABLED};
    ControlMode _newMode{DISABLED};

    OverrideController _overrideController;
    TeleopController _teleopController;
    PathingController _pathingController;
    FollowingController _followingController;
    SummonController _summonController;

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

    std::map<const std::string, std::function<void(rapidjson::Document &doc)>> _netHandlers;

    double _runTime {0.0};
    bool _enabled {false};
};
