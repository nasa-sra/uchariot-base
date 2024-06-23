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

// The main robot process scheduler.
void Robot::Run(int rate, bool& running) {
    auto start_time = std::chrono::high_resolution_clock::now();

    _subsystems->drive->Init();

    while (running) {
        start_time = std::chrono::high_resolution_clock::now();

        // Swap out controllers if it is changed via network manager
        if (_active_controller_name != _last_controller_name) {
            _active_controller->DeInit();
            _active_controller = _controllers->FromName(_active_controller_name);
            _active_controller->Init();
            _last_controller_name = _active_controller_name;
            Utils::LogFmt("Switching to active controller %s", _active_controller_name);
        }

        // Update the active controller 
        _active_controller->Update();

        // Update subsystems
        _subsystems->drive->Update();

        // Handle periodic update scheduling 
        int dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start_time).count();
        if (dt < 1000 / rate) {
            std::this_thread::sleep_for(std::chrono::milliseconds(int(1000.0 / rate - dt)));
        } else {
            Utils::LogFmt("Robot overrun of %i ms", dt);
        }
    }
}
