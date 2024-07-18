#pragma once

#include "Utils.h"
#include "subsystems/SubsystemBase.h"

struct MsgBuffer { 
	long _type; 
	char _content[16]; 
}; 

class Vision : SubsystemBase {

    const char* name;
    MsgBuffer _msg;

    double _heading;

    double GetEntry(const std::string& entry);
public:

    Vision();

    void Update(double dt);
    void ReportState(std::string prefix = "/");

    inline double GetHeading() { return _heading; }

};