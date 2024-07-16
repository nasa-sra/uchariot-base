#pragma once

#include "ControllerBase.h"
#include "VescController.h"
#include "rapidjson/document.h"

class TeleopController : public ControllerBase {

    double _turn, _fwd, _speed;

public:
    TeleopController() : ControllerBase("teleop"), _turn(0), _fwd(0), _speed(0) {}

    inline void Load() override {
        _turn = 0;
        _fwd = 0;
        _speed = 0;
    }; 

    inline void Unload() override {
        Load();
    }

    ControlCmds Run();

    void HandleNetworkInput(rapidjson::Document& doc);

    static TeleopController& GetInstance() {
        static TeleopController _instance;
        return _instance;
    }
};