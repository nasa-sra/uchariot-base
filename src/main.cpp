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

    if (!NetworkManager::GetInstance().Start( 8000, [&robot](std::string cmd, rapidjson::Document &doc) { robot.HandleNetCmd(cmd, doc); })) {
        can->CloseConnection();
        return 0;
    }

    StateReporter::GetInstance().EnableLogging();
    StateReporter::GetInstance().EnableTelemetry();

    while(running) {
        CanFrame frame;
        frame.arb_id = 1;
        int n = 100000;
        frame.data = (uint8_t*)&n;
        frame.len = 4;
        can->Send(frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // robot.Run(50, running);
    
    robot.Shutdown();
    can->CloseConnection();
    NetworkManager::GetInstance().CloseConnections();
    StateReporter::GetInstance().Close();
    Utils::LogFmt("Shutdown");

    return 0;
}
