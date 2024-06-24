#include "controllers/TeleopController.h"

void TeleopController::HandleNetworkInput(rapidjson::Document& doc) {
    _left_speed = doc["left_speed"].GetDouble();
    _right_speed = doc["right_speed"].GetDouble();
}

void TeleopController::Update() {
    // Utils::LogFmt("TeleopController speeds: left: %f right: %f", _left_speed, _right_speed);
    _drive_base->SetOutput(DriveOutput(_left_speed, _right_speed));
}