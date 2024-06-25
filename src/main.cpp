#include <cstdio>
#include <stdexcept>
#include <csignal>
#include <thread>
#include <cstdlib>

#include "CanConnection.h"
#include "Robot.h"
#include "NetworkManager.h"
#include "Utils.h"

bool running = true;

void HandleSigInt(int s) { running = false; }

int main() {

    signal(SIGINT, HandleSigInt);

    CanConnection* can = &CanConnection::GetInstance(); // Threadsafe singleton
    can->Start(running);
    
    Robot robot;
    NetworkManager network_manager;

    std::thread network_worker(&NetworkManager::Listener, &network_manager, std::ref(robot), std::ref(running));

    robot.Run(50, running);
    
    can->CloseConnection();

    network_worker.join();
    network_manager.CloseConnection();

    return 0;
}