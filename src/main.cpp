#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "CanConnection.h"
#include "MessageQueue.h"
#include "NetworkManager.h"
#include "Robot.h"
#include "StateReporter.h"
#include "Utils.h"

bool running = true;

void HandleSigInt(int s) { running = false; }

int main() {
    signal(SIGINT, HandleSigInt);

    CanConnection *can = &CanConnection::GetInstance();
    can->Start();

    Robot robot;

    NetworkManager network("CmdServer", true);

    if (!network.Start(8000, [&robot](std::string cmd, rapidjson::Document &doc) { robot.HandleNetCmd(cmd, doc); })) {
        can->CloseConnection();
        return 0;
    }

    StateReporter::GetInstance().EnableLogging();
    StateReporter::GetInstance().EnableTelemetry(&network);

    robot.Run(50, running);

    robot.Shutdown();
    can->CloseConnection();
    network.CloseConnections();
    StateReporter::GetInstance().Close();
    Utils::LogFmt("Shutdown");

    return 0;
}