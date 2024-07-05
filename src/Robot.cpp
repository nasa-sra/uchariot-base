#include "Robot.h"

Robot::Robot() {}

// Recive a network command and handle it appropriately.
void Robot::HandleNetCmd(const std::string& cmd, rapidjson::Document& doc) {
    if (cmd == "set_controller") { 
        _newMode = nameToMode(doc["name"].GetString());
    }

    if (cmd == "teleop_drive") {
        _teleopController.HandleNetworkInput(doc);
    } else if (cmd == "run_path") {
        _pathingController.HandleNetworkInput(doc);
    }
}

// The main robot process scheduler.
void Robot::Run(int rate, bool& running) {
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
                cmds = _pathingController.Run(_localization.getPose());
                break;
            
            default:
                break;
        }

        // Commmand subsystems
        _driveBase.SetCmds(cmds.drive);

        // Update subsystems
        _driveBase.Update(dt);
        _imu.Update(dt);
        _localization.Update(dt, _driveBase.GetVelocities());

        // Report state
        cmds.ReportState();
        _pathingController.ReportState();
        _driveBase.ReportState();
        _localization.ReportState();
        _imu.ReportState();
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
        Utils::LogFmt("Switching to active controller %s", modeToController(_mode).name);
    }
}

Robot::ControlMode Robot::nameToMode(std::string name) {
    if (name == _teleopController.name) return ControlMode::TELEOP;
    if (name == _pathingController.name) return ControlMode::PATHING;
    return ControlMode::DISABLED;
}

ControllerBase& Robot::modeToController(ControlMode mode) {
    static DisabledController dc;
    if (mode == TELEOP) return _teleopController;
    if (mode == PATHING) return _pathingController;
    return dc;
}