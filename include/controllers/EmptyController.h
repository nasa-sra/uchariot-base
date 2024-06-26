#pragma once

#include "ControllerBase.h"

// Represents an empty controller to populate if
// no other controllers are available to be loaded.
class EmptyController : public ControllerBase {
    EmptyController() = default;
public: 
    void Load() override {};
    ControlCmds Run() override {return ControlCmds();};
    void Unload() override {};

    static EmptyController& GetInstance() {
        static EmptyController _instance;
        return _instance;
    }
};