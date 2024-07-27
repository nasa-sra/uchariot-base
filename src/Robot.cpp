#include "Robot.h"

Robot::Robot()
    : _vision(&_driveBase),
      _localization(&_driveBase, &_imu, &_vision, &_gps),
      _pathingController(&_localization) {}

// Recive a network command and handle it appropriately.
void Robot::HandleNetCmd(const std::string &cmd, rapidjson::Document &doc) {
    try {
        _netHandlers.at(cmd)(doc);
    } catch (...) {
        Utils::LogFmt("Could not parse command");  // This still crashes
    }
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

        // Commmand subsystems
        _driveBase.SetCmds(cmds.drive);

        // Update subsystems
        _driveBase.Update(dt);
        _imu.Update(dt);
        _gps.Update(dt);
        _vision.Update(dt);
        _localization.Update(dt);

        // Report state
        StateReporter::GetInstance().UpdateKey("/controller",
                                               modeToController(_mode).name);

        cmds.ReportState();
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
