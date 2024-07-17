#pragma once

#include "ControllerBase.h"
#include "VescController.h"
#include "rapidjson/document.h"

class TeleopController : public ControllerBase {

    double _fwdVelocity{0.0};
    double _angVelocity{0.0};

public:
    TeleopController() : ControllerBase("teleop") {}

    inline void Load() override {
        _fwdVelocity = 0.0;
        _angVelocity = 0.0;
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