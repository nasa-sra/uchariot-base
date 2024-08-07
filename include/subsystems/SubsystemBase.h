#pragma once

#include <string>

#include "StateReporter.h"

// Baseclass for a subsystem module, defines init
// and update methods.
class SubsystemBase {
   protected:
    StateReporter* _sr = &StateReporter::GetInstance();

   public:
    virtual void Update(double dt) = 0;
    virtual void ReportState(std::string prefix = "/") = 0;
};
