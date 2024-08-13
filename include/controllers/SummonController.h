#pragma once

#include "ControllerBase.h"
#include "subsystems/Vision.h"
#include "subsystems/Localization.h"

class SummonController : public ControllerBase {
public:
    SummonController(Localization* localization, Vision* vision);

    void Configure(tinyxml2::XMLElement* xml) override;
    void Load() override;
    void Unload() override {}

    ControlCmds Run(ControlCmds cmds = ControlCmds()) override;
    void Summon(Utils::GeoPoint target);
    void ReportState(std::string prefix = "/") override;

private:

    Vision* _vision {nullptr};
    Localization* _localization {nullptr};

    bool _summoned {false};
    bool _runningPath {false};
    bool _runningTracking {false};
    Eigen::Vector2d _targetPos;
    double _distanceToWaypoint {0.0};
    double _targetHeading {0.0};
    Detection _targetDet;

    double _driveVelocity {1.0};
    double _velocityKp {0.5};
    double _headingKp {-1.0};
    double _transitionDistance {4.0};
    double _stopDistance {2.0};
    std::string _targetName {"person"};

};