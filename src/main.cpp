#include <cstdio>
#include <stdexcept>
#include <csignal>
#include <thread>
#include <cstdlib>

#include "CanConnection.h"
#include "Robot.h"
#include "NetworkingManager.h"

bool running = true;

void handleSigInt(int s) { running = false; }

int main() {

    signal(SIGINT, handleSigInt);

    CanConnection* can;
    try {
        can = CanConnection::getInstance();
    } catch (std::runtime_error& e) {
        printf("Error connecting can - %s", e.what());
        return 1;
    }
    printf("Here");

    NetworkingManager networkingManager;

    Robot robot;

    printf("Here");

    networkingManager.run_listen_thrd();
    // std::thread network_thrd(&NetworkingManager::run_listen_thrd, &networkingManager);

    robot.run(50, running);

    can->closeConnection();

    return 0;
}