#pragma once

#include <netinet/in.h>

#include "Robot.h"

class NetworkingManager {
public:
    NetworkingManager();

    void run_listen_thrd(Robot& robot);
private:
    int piSocket;
    sockaddr_in piAddress;
    int clientConnection;

};
