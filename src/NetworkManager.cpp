
#include "NetworkManager.h"

NetworkManager::NetworkManager(int port) {

    FD_ZERO(&_fds);

    _net_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_net_socket == -1) {
        Utils::LogFmt("NetworkManager failed to create socket");
    }
    _net_addr.sin_family = AF_INET;
    _net_addr.sin_port = htons(port);
    _net_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_net_socket, (struct sockaddr*)&_net_addr, sizeof (_net_addr)) == -1) {
        Utils::LogFmt("NetworkManager Socket Bind Failed");
    }

    Utils::LogFmt("Listening on port %i", port);
    listen(_net_socket, 10);
    FD_SET(_net_socket, &_fds);
    _running = true;
}

void NetworkManager::Start(PacketCallback packetCallback) {
    _packetCallback = packetCallback;
    _serverThread = std::thread(&NetworkManager::Run, this);
}

// This is mostly from here https://beej.us/guide/bgnet/html/#select
void NetworkManager::Run() {

    fd_set read_fds;
    FD_ZERO(&read_fds);
    _fdmax = _net_socket;

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // Utils::LogFmt("Server running");
    while (_running) {

        read_fds = _fds;
        if (select(_fdmax+1, &read_fds, NULL, NULL, &tv) == -1) {
            Utils::ErrFmt("NetworkManager - Error on select");
        }

        for (int i = 0; i <= _fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == _net_socket) {
                    acceptConnection();
                } else {
                    receivePacket(i);
                }
            }
        }
    }
}

void NetworkManager::acceptConnection() {
    struct sockaddr_storage remoteaddr;
    socklen_t addrlen = sizeof remoteaddr;
    int conn = accept(_net_socket, (struct sockaddr*) &remoteaddr, &addrlen);
    if (conn == -1) {
        Utils::LogFmt("NetworkManager - Error on accept");
    } else {
        FD_SET(conn, &_fds);
        if (conn > _fdmax) {
            _fdmax = conn;
        }
        if (_cmdClient == -1) {
            _cmdClient = conn;
            Utils::LogFmt("New commanding client %i connected", conn);
        } else {
            Utils::LogFmt("New client %i connected", conn);
        }
        _clientNum++;
    }
}

void NetworkManager::receivePacket(int fd) {
    const int BUFFER_SIZE = 2048;
    char buffer[BUFFER_SIZE] = {0};
    ssize_t nbytes = recv(fd, buffer, sizeof(buffer), 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            Utils::LogFmt("Client %i Disconencted", fd);
        } else { 
            Utils::LogFmt("NetworkManager - Error on recv");
        }
        _clientNum--;
        close(fd);
        FD_CLR(fd, &_fds);
        if (fd == _fdmax) {
            _fdmax--;
        }
        if (fd == _cmdClient) {
            if (_clientNum > 0) {
                _cmdClient = _fdmax;
                Utils::LogFmt("Switched commanding client to %i", _cmdClient);
            } else {
                _cmdClient = -1;
                Utils::LogFmt("Lost commanding client");
            }
        }
    } else if (fd == _cmdClient) {
        handlePacket(buffer, BUFFER_SIZE);
    }
}

void NetworkManager::handlePacket(char* buffer, size_t len) {
    if (buffer[0] == '\0') return; // no data avialable
    if (buffer[0] != '[') return;  // not in valid format
    std::string raw_input(buffer, len);   // convert to C++ str
    // Utils::LogFmt("Received valid input: %s\n", raw_input.c_str());

    size_t delim_index = raw_input.find(']');
    std::string cmd = raw_input.substr(1, delim_index - 1);
    std::string data = raw_input.substr(delim_index + 1, std::string::npos);
    Utils::LogFmt("CMD: %s DATA: %s", cmd.c_str(), data.c_str());

    rapidjson::Document document;
    document.Parse(data.c_str());
    _packetCallback(cmd, document);
}

void NetworkManager::CloseConnections() {
    Utils::LogFmt("Closing connections");
    _running = false;

    for (int i = 0; i <= _fdmax; i++) {
        close(i);
    }

    _serverThread.join();
}