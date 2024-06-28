#pragma once

#include "ControllerBase.h"
#include "rapidjson/document.h"

class PathingController : public ControllerBase {

public:
    PathingController() : ControllerBase("pathing") {}

    void Load() override; 
    void Unload() override;
    ControlCmds Run();

};