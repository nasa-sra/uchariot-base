#pragma once

#include "Base.h"
#include "VescController.h"
#include "subsystems/DriveBase.h"
#include "rapidjson/document.h"

class TeleopController : public ControllerBase {

    double _turn, _fwd, _speed;
    DriveBase* _drive_base;

public:
    TeleopController() : _turn(0), _fwd(0), _speed(0), _drive_base(&DriveBase::GetInstance()) {}

    inline void Load() override {
        _turn = 0;
        _fwd = 0;
        _speed = 0;
    }; 

    inline void Unload() override {
        Load(); // shit
    }

    void Update() override;

    void HandleNetworkInput(rapidjson::Document& doc);

    static TeleopController& GetInstance() {
        static TeleopController _instance;
        return _instance;
    }
};