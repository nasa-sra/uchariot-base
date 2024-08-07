#pragma once

#include <unistd.h>
#include <functional>
#include <thread>
#include <vector>
#include <map>

#include "Utils.h"
struct CanFrame {
    uint32_t arb_id;
    uint8_t* data;
    size_t len;

    CanFrame() {}
    CanFrame(uint32_t arb_id, uint8_t* data, size_t len) : arb_id(arb_id), data(data), len(len){};
};

class CanConnection {
   public:
    static CanConnection& GetInstance() {
        static CanConnection _instance;  // Guaranteed to be destroyed.
        return _instance;
    }

    void Start();
    void RegisterPacketHandler(uint32_t id, std::function<void(CanFrame)> handler);
    void Send(std::string msg);
    void SendCan(uint32_t id, int32_t data);
    void Recieve();
    void CloseConnection();

   private:
    void LogFrame(CanFrame frame);

    CanConnection();

    std::thread _receiveThread;
    bool _running{true};

    int _serialPort;
    std::map<uint32_t, std::function<void(CanFrame)>> _callbacks;
};