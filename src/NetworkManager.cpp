


#include "NetworkManager.h"

NetworkManager::NetworkManager() {
    _net_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_net_socket == -1) {
        Utils::LogFmt("NetworkManager failed to create socket");
    }
    _net_addr.sin_family = AF_INET;
    _net_addr.sin_port = htons(8000);
    _net_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(_net_socket, (struct sockaddr*)&_net_addr, sizeof (_net_addr)) == -1) {
        Utils::LogFmt("NetworkManager Socket Bind Failed");
    }

    Utils::LogFmt("Listening on port 8000");
    listen(_net_socket, 2);
}

void NetworkManager::Listener(Robot& robot, bool& running) {

    _client_conn = accept(_net_socket, nullptr, nullptr);
    while (running) {
        char buffer[2048] = {0};
        ssize_t val = recv(_client_conn, buffer, sizeof(buffer), 0);
        if (buffer[0] == '\0') continue; // no data avialable
        if (buffer[0] != '[') continue;  // not in valid format
        std::string raw_input(buffer);   // convert to C++ str
        Utils::LogFmt("Received valid input: %s\n", raw_input.c_str());

        size_t delim_index = raw_input.find(']');
        std::string cmd = raw_input.substr(1, delim_index - 1);
        std::string data = raw_input.substr(delim_index + 1, std::string::npos);
        Utils::LogFmt("CMD: %s DATA: %s\n", cmd.c_str(), data.c_str());

        rapidjson::Document document;
        document.Parse(data.c_str());
        robot.HandleNetCmd(cmd, document);
    }

    // close(_net_socket);
}


void NetworkManager::CloseConnection() {
    close(_client_conn);
    close(_net_socket);
}