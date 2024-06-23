#pragma once

#include "Base.h"
#include "VescController.h"
#include "subsystems/DriveBase.h"
#include "rapidjson/document.h"

class TeleopController : public ControllerBase {

    double _left_speed, _right_speed;
    DriveBase* _drive_base;

public:
    TeleopController() : _left_speed(0), _right_speed(0), _drive_base(&DriveBase::GetInstance()) {}

    inline void Load() override {
        _left_speed = 0;
        _right_speed = 0;
    }; 
    inline void Unload() override {
        _left_speed = 0;
        _right_speed = 0;
    }

    void Update() override;

    void HandleNetworkInput(rapidjson::Document& doc);

    static TeleopController& GetInstance() {
        static TeleopController _instance;
        return _instance;
    }
};