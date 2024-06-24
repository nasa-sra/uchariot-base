#include "Robot.h"

// Resolves a given controller from a name. If the controller
// is not found by name it returns the empty controller.
ControllerBase* Controllers::FromName(const std::string& name) {
    if (name == "teleop") return teleop;
    return empty;
}

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

void Robot::ScheduleNextIter(int rate, std::chrono::high_resolution_clock::time_point start_time) {
    int dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
    if (dt < 1000 / rate) {
        std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0 / rate - dt)));
    } else {
        Utils::LogFmt("Robot overrun of %i ms", dt);
    }
}

// The main robot process scheduler.
void Robot::Run(int rate, bool& running) {
    auto start_time = std::chrono::high_resolution_clock::now();

    while (running) {
        start_time = std::chrono::high_resolution_clock::now();

        // Swap out controllers if it is changed via network manager
        ManageController();

        // Update the active controller 
        _active_controller->Update();

        // Update subsystems
        _subsystems->drive->Update();

        // Handle periodic update scheduling 
        ScheduleNextIter(rate, start_time);
    }
}
