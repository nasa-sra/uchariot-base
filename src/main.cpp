#include <cstdio>
#include <stdexcept>
#include <csignal>
#include <thread>
#include <cstdlib>

#include "CanConnection.h"
#include "Robot.h"
#include "NetworkManager.h"
#include "Utils.h"
#include "StateReporter.h"

bool running = true;

void HandleSigInt(int s) { running = false; }

int main() {

    signal(SIGINT, HandleSigInt);

    CanConnection* can = &CanConnection::GetInstance(); // Threadsafe singleton
    can->Start(running);
    
    Robot robot;
    NetworkManager network_manager(8001);
    network_manager.Start([&robot](std::string cmd, rapidjson::Document& doc){robot.HandleNetCmd(cmd, doc);});

    StateReporter::GetInstance().SetLogging(true);

    robot.Run(50, running);
    
    can->CloseConnection();
    network_manager.CloseConnections();
    StateReporter::GetInstance().Close();

    return 0;
}