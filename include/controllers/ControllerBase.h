#pragma once

#include "subsystems/DriveBase.h"

struct ControlCmds {
    DriveBaseCmds drive;

    void ReportState(std::string prefix="/") {
        prefix += "control_cmds/";
        drive.ReportState(prefix);
    }
};

// Baseclass for a controller module, defines init,
// deinit, and run methods.
class ControllerBase {
public:
    ControllerBase(std::string _name) : name(_name) {}
    virtual void Load() = 0;
    virtual ControlCmds Run(ControlCmds cmds = ControlCmds()) = 0;
    virtual void Unload() = 0;

    std::string name;
};

class DisabledController : public ControllerBase {
public:
    DisabledController() : ControllerBase("disabled") {}
     void Load() {}
     ControlCmds Run(ControlCmds cmds = ControlCmds()) {return ControlCmds();}
     void Unload() {}
};