#pragma once


#include <cstdio>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include "rapidjson/document.h"
#include "Robot.h"

class NetworkManager {
public:
    NetworkManager();
    void Listener(Robot& robot);
private:
    int _net_socket;
    sockaddr_in _net_addr;
    int _client_conn;
};
