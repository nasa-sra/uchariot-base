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

    const std::map<const std::string,
                   std::function<void(rapidjson::Document &doc)>>
        _netHandlers = {{"set_controller",
                         [this](rapidjson::Document &doc) -> void {
                             _newMode = nameToMode(doc["name"].GetString());
                         }},
                        {"teleop_drive",
                         [this](rapidjson::Document &doc) -> void {
                             _teleopController.HandleNetworkInput(doc);
                         }},
                        {"run_path",
                         [this](rapidjson::Document &doc) -> void {
                             _pathingController.HandleNetworkInput(doc);
                         }},
                        {"reset_heading",
                         [this](rapidjson::Document &doc) -> void {
                             _localization.ResetHeading();
                         }},
                        {"reset_pose",
                         [this](rapidjson::Document &doc) -> void {
                             _localization.ResetPose();
                         }},
                        {"stop_path",
                         [this](rapidjson::Document &doc) -> void {
                             _pathingController.Stop();
                         }},
                        {"pause_path",
                         [this](rapidjson::Document &doc) -> void {
                             _pathingController.Pause();
                         }},
                        {"set_obstacle_avoidance",
                         [this](rapidjson::Document &doc) -> void {
                             _driveBase.usingVisionObstacleAvoidance =
                                 doc["enabled"].GetBool();
                         }}};
};
