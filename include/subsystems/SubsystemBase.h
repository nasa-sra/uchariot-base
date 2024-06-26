#pragma once

// Baseclass for a subsystem module, defines init
// and update methods.
class SubsystemBase {
    virtual void Update() = 0;
};
