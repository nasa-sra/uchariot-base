#pragma once

#include <netinet/in.h>

class NetworkingManager {
public:
    NetworkingManager();

    void run_listen_thrd();
private:
    int piSocket;
    sockaddr_in piAddress;
    int clientConnection;

};
