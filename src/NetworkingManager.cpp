#include <thread>
#include <sys/socket.h>
#include <print>

#include "NetworkingManager.h"


NetworkingManager::NetworkingManager() {
    piSocket = socket(AF_INET, SOCK_STREAM, 0);
    piAddress.sin_family = AF_INET;
    piAddress.sin_port = htons(5000);
    piAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(piSocket, (struct sockaddr*)&piAddress, sizeof (piAddress)) == -1) {
        printf("Socket Creation Failed");
        std::terminate();
    }

    listen(piSocket, 2);

    clientConnection = accept(piSocket, nullptr, nullptr);
}

void NetworkingManager::run_listen_thrd() {
    while (true) {
        char buffer[2048] = {0};
        ssize_t val = recv(clientConnection, buffer, sizeof(buffer), 0);

        if (val == 0) break;

    }

    close(piSocket);
}
