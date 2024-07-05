#pragma once

#include <string>

// Baseclass for a subsystem module, defines init
// and update methods.
class SubsystemBase {
    virtual void Update(double dt) {};
    virtual void ReportState(std::string prefix = "/") = 0;
};
