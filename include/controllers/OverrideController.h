#pragma once

#include "ControllerBase.h"
#include "subsystems/Vision.h"

class OverrideController : public ControllerBase {
public:
    OverrideController(Vision* vision);

    void Configure(tinyxml2::XMLElement* xml) override;
    void Load() override {};
    void Unload() override {};

    ControlCmds Run(ControlCmds cmds = ControlCmds());

    void EStop() { _eStopped = true;}

    void ReportState(std::string prefix = "/");

private:

    Vision* _vision;

    bool _eStopped{false};
    bool _obstacleBlocked{false};

    double _stoppingDistance{1.0};

};