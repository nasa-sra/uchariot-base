#pragma once

// Baseclass for a subsystem module, defines init
// and update methods.
class SubsystemBase {
    virtual void Update() = 0;
};

// Baseclass for a controller module, defines init,
// deinit, and update methods.
class ControllerBase {
public:
    virtual void Load() = 0;
    virtual void Update(void) = 0;
    virtual void Unload() = 0;
};