#include <cstdio>
#include <stdexcept>
#include <csignal>
#include <thread>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <iostream>

#include "pathgen/Vector.h"
#include "pathgen/PathGenerator.h"
#include "CanConnection.h"
#include "Robot.h"
#include "NetworkManager.h"
#include "Utils.h"
#include "StateReporter.h"

using std::ofstream;
using std::ios;

#if PATH_TEST

int main() {
    std::vector<Vector> points {
        Vector(29.560013, -95.084279, 0),
        Vector(29.560461, -95.084150,0),
        Vector(29.561629, -95.083475,0),
        Vector(29.562560, -95.082183,0),
        Vector(29.562488, -95.080737,0),
        Vector(29.562757, -95.080201, 0)
    };

    PathGenerator::SetPathSize(3);

    PathGenerator::GeneratePath("AutonPath", 2.0, 3.0);

    // ofstream Bob("Bob.txt");
    // std::cout << Bob.is_open();
    // Bob << "Bilbo" << std::endl;
    // Bob.close();

    return 0;
}

#else

bool running = true;

void HandleSigInt(int s) { running = false; }

int main() {

    signal(SIGINT, HandleSigInt);

    CanConnection* can = &CanConnection::GetInstance();
    can->Start(running);
    
    Robot robot;
    NetworkManager::GetInstance().Start(8001, [&robot](std::string cmd, rapidjson::Document& doc){robot.HandleNetCmd(cmd, doc);});

    StateReporter::GetInstance().EnableLogging();
    StateReporter::GetInstance().EnableTelemetry();

    robot.Run(50, running);
    
    can->CloseConnection();
    NetworkManager::GetInstance().CloseConnections();
    StateReporter::GetInstance().Close();
    Utils::LogFmt("Shutdown");

    return 0;
}

#endif

