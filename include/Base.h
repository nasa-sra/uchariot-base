#pragma once

// Baseclass for a subsystem module, defines init
// and update methods.
class SubsystemBase {
    virtual void Init() = 0;
    virtual void Update() = 0;
};

// Baseclass for a controller module, defines init,
// deinit, and update methods.
class ControllerBase {
public:
    virtual void Init() = 0;
    virtual void Update(void) = 0;
    virtual void DeInit() = 0;
};