#pragma once

#include <stdint.h>
#include <unistd.h>

class CanConnection {
public:

    static CanConnection* getInstance();

    void send(uint32_t arbId, uint8_t* buffer, size_t len);
    void closeConnection();

private:

    CanConnection();

    static CanConnection* _instance;

    int _socket;

};