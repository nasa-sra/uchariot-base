#pragma once

#include "rapidjson/document.h"
#include <cstdio>
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

#include "Utils.h"

// Define packet structure for client->server communication
typedef std::function<void(std::string, rapidjson::Document&)> PacketCallback;

class NetworkManager {
public:
    NetworkManager(std::string name, bool singleCommanding=false);

    bool Start(int port, PacketCallback packetCallback);
    void Send(int fd, const char* buffer, int len);
    void SendAll(const char* buffer, int len);
    void CloseConnections();

private:

    void run();
    void acceptConnection();
    void receivePacket(int fd);
    void handlePacket(char* buffer, int start, size_t len);

    std::string _name;
    bool _singleCmding{false};

    int _net_socket;
    sockaddr_in _net_addr;

    fd_set _fds;
    int _cmdClient{-1};
    int _fdmax{-1};
    int _clientNum{0};
    std::vector<int> _clientSockets;

    bool _running{false};
    std::thread _serverThread;

    PacketCallback _packetCallback;
};
