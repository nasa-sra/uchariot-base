#include <stdio.h>
#include <stdexcept>
#include <signal.h>
#include <stdlib.h>

#include "CanConnection.h"
#include "Robot.h"

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

    Robot robot;

    robot.run(50, running);

    can->closeConnection();

    return 0;
}