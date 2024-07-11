#pragma once

#include <stdint.h>
#include <unistd.h>
#include <functional>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#ifdef LINUX
#include <linux/can.h>
#include <linux/can/raw.h>
#endif

#define len can_dlc

#include "Utils.h"

struct CanFrame {
    uint32_t arb_id;
    uint8_t* data;
    size_t len;

    CanFrame(uint32_t arb_id, uint8_t* data, size_t len) : arb_id(arb_id), data(data), len(len) {};
    CanFrame(struct can_frame frame) : arb_id(frame.can_id), data(frame.data), len(frame.len) {};
};

class CanConnection {
public:
    static CanConnection& GetInstance() {
        static CanConnection _instance; // Guaranteed to be destroyed.
        return _instance;
    }

    void Start(bool& running);
    void RegisterPacketHandler(uint16_t id, std::function<void(CanFrame)> handler);
    void Send(CanFrame* frame);
    void Recieve(bool& running);
    void CloseConnection();

private:

    void LogFrame(CanFrame* frame);

    CanConnection();

    std::thread _recieveThread;

    int _socket;
    std::vector<struct can_filter> _filters;
    std::map<uint16_t, std::function<void(CanFrame)>> _callbacks;
};