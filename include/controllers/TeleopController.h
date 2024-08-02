#pragma once

#include "ControllerBase.h"

class TeleopController : public ControllerBase {
public:
    TeleopController();

    inline void Load() override {
        _fwdVelocity = 0.0;
        _angVelocity = 0.0;
    }; 

    inline void Unload() override {
        Load();
    }

    ControlCmds Run(ControlCmds cmds = ControlCmds()) override;
    void SetDriveInput(double velocity, double rotation);

private:

    double _fwdVelocity{0.0};
    double _angVelocity{0.0};

};