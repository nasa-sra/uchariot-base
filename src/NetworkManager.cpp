#include "NetworkManager.h"

NetworkManager::NetworkManager() {
}

/**
 * @brief Initializes and starts the network manager.
 *
 * This function sets up a TCP socket, binds it to the specified port, and starts listening for incoming connections.
 * It also creates a new thread to handle incoming packets and invokes the provided packet callback function for each
 * received packet.
 *
 * @param port The port number to listen on.
 * @param packetCallback A callback function to be invoked for each received packet.
 *
 * @return true if the network manager was successfully started, false otherwise.
 *
 * @note If the socket creation, binding, or listening fails, an error message is logged, and the function returns
 * false.
 * @note The network manager runs in a separate thread, and the main thread continues execution.
 * @note The network manager is stopped when the CloseConnections() function is called.
 *
 * @see PacketCallback
 * @see CloseConnections
 */
bool NetworkManager::Start(int port, PacketCallback packetCallback) {

    FD_ZERO(&_fds);

    _net_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_net_socket == -1) {
        Utils::LogFmt("NetworkManager failed to create socket. Error: %s", strerror(errno));
        return false;
    }
    _net_addr.sin_family = AF_INET;
    _net_addr.sin_port = htons(port);
    _net_addr.sin_addr.s_addr = INADDR_ANY;
    int option = 1;
    setsockopt(_net_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&option, sizeof(option));

    if (bind(_net_socket, (struct sockaddr*)&_net_addr, sizeof(_net_addr)) == -1) {
        Utils::LogFmt("NetworkManager Socket Bind Failed. Error: %s", strerror(errno));
        close(_net_socket);
        return false;
    }

    Utils::LogFmt("Listening on port %i", port);
    listen(_net_socket, 10);
    FD_SET(_net_socket, &_fds);
    _running = true;

    _packetCallback = packetCallback;
    _serverThread = std::thread(&NetworkManager::run, this);
    return true;
}

// This is mostly from here https://beej.us/guide/bgnet/html/#select
void NetworkManager::run() {

    fd_set read_fds;
    FD_ZERO(&read_fds);
    _fdmax = _net_socket;

    struct timeval tv;

    // Utils::LogFmt("Server running");
    while (_running) {

        tv.tv_sec = 1;
        tv.tv_usec = 0;
        read_fds = _fds;
        if (select(_fdmax + 1, &read_fds, NULL, NULL, &tv) == -1) { Utils::ErrFmt("NetworkManager - Error on select"); }

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
    int conn = accept(_net_socket, (struct sockaddr*)&remoteaddr, &addrlen);
    if (conn == -1) {
        Utils::LogFmt("NetworkManager - Error on accept");
    } else {
        FD_SET(conn, &_fds);
        if (conn > _fdmax) { _fdmax = conn; }
        if (_cmdClient == -1) {
            _cmdClient = conn;
            Utils::LogFmt("New commanding client %i connected", conn);
        } else {
            Utils::LogFmt("New client %i connected", conn);
        }
        _clientNum++;
        _clientSockets.push_back(conn);
    }
}

/**
 * @brief Processes incoming packets from a specific client.
 *
 * This function receives data from a client identified by the given file descriptor (fd),
 * processes the received data, and performs necessary actions based on the received packet.
 *
 * @param fd The file descriptor of the client to receive data from.
 *
 * @return void
 *
 * @note If the number of bytes received (nbytes) is less than or equal to 0, the function logs
 * a disconnection message if nbytes is 0, or an error message if nbytes is less than 0.
 * It then removes the client from the list of connected clients, updates the client count,
 * closes the client's socket, clears the client's file descriptor from the set, and adjusts
 * the maximum file descriptor if necessary.
 *
 * If the received data is from the commanding client, the function processes the data by
 * searching for semicolons to delimit individual packets and invoking the handlePacket function
 * for each valid packet.
 *
 * @see handlePacket
 */
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
        _clientSockets.erase(std::remove(_clientSockets.begin(), _clientSockets.end(), fd), _clientSockets.end());
        _clientNum--;
        close(fd);
        FD_CLR(fd, &_fds);
        if (fd == _fdmax) { _fdmax--; }
        if (fd == _cmdClient) {
            if (_clientNum > 0) {
                _cmdClient = _fdmax;
                Utils::LogFmt("Switched commanding client to %i", _cmdClient);
            } else {
                _cmdClient = -1;
                // rapidjson::Document document;
                // document.Parse("{\"speed\":0, \"fwd\":0.0, \"turn\": 0.0}");
                // _packetCallback("teleop_drive", document);
                Utils::LogFmt("Lost commanding client");
            }
        }
    } else if (fd == _cmdClient) {
        int start = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (buffer[i] == ';') {
                handlePacket(buffer, start, i - start);
                start = i + 1;
            }
        }
    }
}

/**
 * @brief Processes and handles incoming packets from clients.
 *
 * This function processes incoming packets from clients by parsing the raw data and invoking the
 * provided callback function with the extracted command and data.
 *
 * @param buffer A pointer to the raw data received from the client.
 * @param start The starting index of the valid data within the buffer.
 * @param len The length of the valid data within the buffer.
 *
 * @return void
 *
 * @note If the buffer contains no data or does not start with a '[' character, the function returns immediately.
 * @note The function extracts the command and data from the raw input, parses the data into a rapidjson::Document,
 * and invokes the provided callback function with the command and document.
 */
void NetworkManager::handlePacket(char* buffer, int start, size_t len) {

    if (buffer[start] == '\0') return;          // no data available
    if (buffer[start] != '[') return;           // not in valid format
    std::string raw_input(buffer + start, len); // convert to C++ str
    // Utils::LogFmt("Received valid input: %s\n", raw_input.c_str());

    size_t delim_index = raw_input.find(']');
    std::string cmd = raw_input.substr(1, delim_index - 1);
    std::string data = raw_input.substr(delim_index + 1, std::string::npos);
    // Utils::LogFmt("CMD: %s DATA: %s", cmd.c_str(), data.c_str());

    rapidjson::Document document;
    document.Parse(data.c_str());
    _packetCallback(cmd, document);
}

/**
 * @brief Sends data to all connected clients.
 *
 * This function iterates through all connected clients and sends the provided data to each one.
 * It utilizes the Send() function to send the data to each client.
 *
 * @param buffer A pointer to the data to be sent.
 * @param len The length of the data to be sent.
 *
 * @return void
 *
 * @note This function does not return any value.
 * @note If an error occurs during the send operation for any client, it logs an error message and continues sending
 * data to the remaining clients.
 */
void NetworkManager::SendAll(const char* buffer, int len) {
    for (int fd : _clientSockets) { Send(fd, buffer, len); }
}

/**
 * @brief Sends data to a specific client using the provided file descriptor.
 *
 * This function sends the data contained in the buffer to the client identified by the given file descriptor.
 * It uses a while loop to ensure all data is sent, even if it spans multiple send calls.
 *
 * @param fd The file descriptor of the client to send data to.
 * @param buffer A pointer to the data to be sent.
 * @param len The length of the data to be sent.
 *
 * @return void
 *
 * @note If an error occurs during the send operation, it logs an error message and returns immediately.
 */
void NetworkManager::Send(int fd, const char* buffer, int len) {
    int total = 0;
    int bytesleft = len;
    int n;

    while (total < len) {
        n = send(fd, buffer + total, bytesleft, 0);
        if (n == -1) {
            Utils::LogFmt("NetworkManger - Error on send");
            return;
        }
        total += n;
        bytesleft -= n;
    }
}

void NetworkManager::CloseConnections() {
    Utils::LogFmt("Closing connections");
    _running = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    for (int fd : _clientSockets) { close(fd); }
    close(_net_socket);

    _serverThread.join();
}