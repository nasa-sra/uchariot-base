#pragma once

#include <iostream>
#include <string>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstdlib>
#include <cstring>

#include "Utils.h"

struct MsgBuffer
{
    long _type;
    char _content[16];
};

class MessageQueue
{

public:
    MessageQueue(const std::string &name);
    void Write(std::string data);

private:
    key_t _key;
    const char *_name;
    MsgBuffer _msg;
    int _msgid;
};
