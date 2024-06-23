#pragma once

#include "Base.h"

// Represents an empty controller to populate if
// no other controllers are available to be loaded.
class EmptyController : public ControllerBase {
    EmptyController() = default;
public: 
    void Load() override {};
    void Update() override {};
    void Unload() override {};

    static EmptyController& GetInstance() {
        static EmptyController _instance;
        return _instance;
    }
};