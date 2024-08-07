#pragma once

#include "ControllerBase.h"
#include "subsystems/Vision.h"

class FollowingController : public ControllerBase {
public:
    FollowingController(Vision* vision);

    void Configure(tinyxml2::XMLElement* xml) override;
    void Load() override {};
    void Unload() override {}

    ControlCmds Run(ControlCmds cmds = ControlCmds()) override;
    void ReportState(std::string prefix = "/") override;

private:

    Vision* _vision {nullptr};
    Detection _target;
    double _targetFilteredDistance {0.0};

    const char* _targetName {"person"};
    double _maxFollowDistance {6.0};
    double _minFollowDistance {2.0};
    double _distanceFilterAlpha {0.5};
    double _driveVelocity {0.5};
    double _headingKp {-1.0};
};