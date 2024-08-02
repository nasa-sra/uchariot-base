#pragma once

#include "ControllerBase.h"
#include "subsystems/Vision.h"

class FollowingController : public ControllerBase {
public:
    FollowingController(Vision* vision);

    void Load() override {};
    void Unload() override {}

    ControlCmds Run(ControlCmds cmds = ControlCmds()) override;
    void ReportState(std::string prefix = "/");

private:

    Vision* _vision {nullptr};
    Detection _target;
    double _targetFilteredDistance {0.0};
    double _distanceFilterAlpha {0.5};

};