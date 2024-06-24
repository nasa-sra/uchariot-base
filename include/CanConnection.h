#pragma once

#include <stdint.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#ifndef SIMULATION
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

#include "Utils.h"

struct CanFrame {
    uint32_t arb_id;
    uint8_t* data;
    size_t len;

    CanFrame(uint32_t arb_id, uint8_t* data, size_t len) : arb_id(arb_id), data(data), len(len) {};
};

class CanConnection {
public:
    static CanConnection& GetInstance() {
        static CanConnection _instance; // Guaranteed to be destroyed.
        return _instance;
    }

    void Send(CanFrame* frame);
    void CloseConnection();

private:

    void LogFrame(CanFrame* frame);

    CanConnection();

    int _socket;
};