#pragma once

#include "subsystems/DriveBase.h"

struct ControlCmds {
    DriveBaseCmds drive;
};

// Baseclass for a controller module, defines init,
// deinit, and run methods.
class ControllerBase {
public:
    virtual void Load() = 0;
    virtual ControlCmds Run(void) = 0;
    virtual void Unload() = 0;
};