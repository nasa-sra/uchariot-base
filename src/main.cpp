#include <cstdio>
#include <stdexcept>
#include <csignal>
#include <thread>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <iostream>

#include "CanConnection.h"
#include "Robot.h"
#include "NetworkManager.h"
#include "Utils.h"
#include "StateReporter.h"

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