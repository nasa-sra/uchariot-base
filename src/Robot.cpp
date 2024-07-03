#include "Robot.h"

// Resolves a given controller from a name. If the controller
// is not found by name it returns the empty controller.
ControllerBase* Controllers::FromName(const std::string& name) {
    if (name == "teleop") return teleop;
    return empty;
}

Robot::Robot() :
    _controllers(new Controllers()),
    _active_controller_name("teleop"),
    _active_controller((ControllerBase*)_controllers->teleop) {}

// Recive a network command and handle it appropriately.
void Robot::HandleNetCmd(const std::string& cmd, rapidjson::Document& doc) {
    // A set_controller command is handled directly by Robot, 
    // swapping out the active controller
    if (cmd == "set_controller") { 
        _active_controller_name = doc["name"].GetString();
    }

    if (cmd == "teleop_drive") {
        _controllers->teleop->HandleNetworkInput(doc);
    }
}

void Robot::ManageController() {
    if (_active_controller_name != _last_controller_name) {
        _active_controller->Unload();
        _active_controller = _controllers->FromName(_active_controller_name);
        _active_controller->Load();
        _last_controller_name = _active_controller_name;
        Utils::LogFmt("Switching to active controller %s", _active_controller_name);
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
        ControlCmds cmds = _active_controller->Run();

        // Commmand subsystems
        _driveBase.SetCmds(cmds.drive);

        // Update subsystems
        _driveBase.Update(dt);
        _imu.Update(dt);
        // _gps.Update(dt);

        // Report state
        cmds.ReportState();
        _driveBase.ReportState();
        // _imu.ReportState();
        StateReporter::GetInstance().PushState();

        // Handle periodic update scheduling 
        dt = Utils::ScheduleRate(rate, start_time);
        if (dt > 1.0 / rate) {
            Utils::LogFmt("Robot Run overran by %f s", dt);
        }
    }
}
