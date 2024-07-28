#pragma once

#include <sys/ipc.h>
#include <sys/msg.h>

#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <string>
#include <thread>

#include "Utils.h"

class MessageQueue {
   public:
    MessageQueue(const std::string &name,
                 std::function<void(std::string)> callback);

    void Close();

    std::string ReceiveMessage();

   private:
    void recieve();

    key_t _key;
    const char *_name;
    int _msgid;
    bool _running{true};

    std::thread _recieveThread;
    std::function<void(std::string)> _callback;
};
