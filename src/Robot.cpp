#include "Robot.h"

Robot::Robot()
    : _vision(),
      _localization(&_driveBase, &_imu, &_vision, &_gps),
      _overrideController(&_vision),
      _pathingController(&_localization)
    {
    _netHandlers["set_controller"] = [this](rapidjson::Document &doc) {
            if (!doc.HasMember("name") || !doc["name"].IsString()) throw std::runtime_error("no name");
            _newMode = nameToMode(doc["name"].GetString());
        };
    _netHandlers["teleop_drive"] = [this](rapidjson::Document &doc) {
            if (!doc.HasMember("velocity") || !doc["velocity"].IsDouble()) throw std::runtime_error("no velocity");
            if (!doc.HasMember("rotation") || !doc["rotation"].IsDouble()) throw std::runtime_error("no rotation");
            _teleopController.SetDriveInput(doc["velocity"].GetDouble(), doc["rotation"].GetDouble());
        };
    _netHandlers["run_path"] = [this](rapidjson::Document &doc) {
            if (!doc.HasMember("name") || !doc["name"].IsString()) throw std::runtime_error("no name");
            _pathingController.SetPathName(doc["name"].GetString());
        };
    _netHandlers["set_stop_distance"] = [this](rapidjson::Document &doc) {
            if (!doc.HasMember("distance") || !doc["distance"].IsDouble()) throw std::runtime_error("no distance");
            _overrideController.SetStopDistance(doc["distance"].GetDouble());
        };
    _netHandlers["reset_heading"] = [this](rapidjson::Document &doc) { _localization.ResetHeading(); };
    _netHandlers["reset_pose"] = [this](rapidjson::Document &doc) { _localization.ResetPose(); };
    _netHandlers["stop_path"] = [this](rapidjson::Document &doc) { _pathingController.Stop(); };
    _netHandlers["pause_path"] = [this](rapidjson::Document &doc) { _pathingController.Pause(); };
    _netHandlers["estop"] = [this](rapidjson::Document &doc) { _overrideController.EStop(); };
}

// The main robot process scheduler.
void Robot::Run(int rate, bool &running) {
    auto start_time = std::chrono::high_resolution_clock::now();

    double dt = 1.0 / rate;
    Utils::LogFmt("Robot Running");
    while (running) {
        start_time = std::chrono::high_resolution_clock::now();

        // Swap out controllers if it is changed via network manager
        ManageController();

        // Run the active controller
        ControlCmds cmds;
        switch (_mode) {
            case ControlMode::DISABLED:
                cmds = ControlCmds();
                break;
            case ControlMode::TELEOP:
                cmds = _teleopController.Run();
                break;
            case ControlMode::PATHING:
                cmds = _pathingController.Run();
                break;
            default:
                break;
        }
        cmds = _overrideController.Run(cmds);

        // Commmand subsystems
        _driveBase.SetCmds(cmds.drive);

        // Update subsystems
        _driveBase.Update(dt);
        _imu.Update(dt);
        _gps.Update(dt);
        _vision.Update(dt);
        _localization.Update(dt);

        // Report state
        StateReporter::GetInstance().UpdateKey("/controller", modeToController(_mode).name);

        cmds.ReportState();
        _overrideController.ReportState();
        _pathingController.ReportState();
        _driveBase.ReportState();
        _localization.ReportState();
        _gps.ReportState();
        _imu.ReportState();
        _vision.ReportState();
        StateReporter::GetInstance().PushState();

        // Handle periodic update scheduling
        dt = Utils::ScheduleRate(rate, start_time);
        if (dt > 1.0 / rate) {
            Utils::LogFmt("Robot Run overran by %f s", dt);
        }
    }
}

// Recive a network command and handle it appropriately.
void Robot::HandleNetCmd(const std::string &cmd, rapidjson::Document &doc) {
    try {
        _netHandlers.at(cmd)(doc);
    } catch (std::exception& e) {
        Utils::LogFmt("Robot::HandleNetCmd - Could not parse command %s: %s", cmd, e.what());
    }
}

void Robot::Shutdown() {
    _vision.Disconnect();
}

void Robot::ManageController() {
    if (_newMode != _mode) {
        modeToController(_mode).Unload();
        _mode = _newMode;
        modeToController(_mode).Load();
        Utils::LogFmt("Switching to active controller %s",
                      modeToController(_mode).name);
    }
}

Robot::ControlMode Robot::nameToMode(std::string name) {
    if (name == _teleopController.name) return ControlMode::TELEOP;
    if (name == _pathingController.name) return ControlMode::PATHING;
    return ControlMode::DISABLED;
}

ControllerBase &Robot::modeToController(ControlMode mode) {
    static DisabledController dc;
    if (mode == TELEOP) return _teleopController;
    if (mode == PATHING) return _pathingController;
    return dc;
}
