#include <cstdio>
#include <thread>
#include <sys/socket.h>

#include "NetworkingManager.h"
#include "rapidjson/document.h"
#include "Robot.h"


NetworkingManager::NetworkingManager() {
    printf("Here");

    piSocket = socket(AF_INET, SOCK_STREAM, 0);
    piAddress.sin_family = AF_INET;
    piAddress.sin_port = htons(5000);
    piAddress.sin_addr.s_addr = INADDR_ANY;

    printf("Here1");


    if (bind(piSocket, (struct sockaddr*)&piAddress, sizeof (piAddress)) == -1) {
        printf("Socket Creation Failed");
        // std::terminate();
    }

    printf("Here2");

    listen(piSocket, 2);

    printf("Here3");

    clientConnection = accept(piSocket, nullptr, nullptr);

    printf("Here4");
}

void NetworkingManager::run_listen_thrd(Robot& robot) {
    while (true) {
        char buffer[2048] = {0};
        ssize_t val = recv(clientConnection, buffer, sizeof(buffer), 0);

        rapidjson::Document document;
        document.Parse("{ \"TYPE\": \"cmd\", \"CONTENTS\": \"drive_f\"}");
        robot.changeDriveMode(document["CONTENTS"].GetString());
        
        if (val == 0) break;
    }

    // close(piSocket);
}
