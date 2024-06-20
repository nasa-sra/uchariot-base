#pragma once

#include "CanConnection.h"

class VescController {
public:

    VescController(int id);

    void setCmd(float cmd);

private:

    void sendDutyCycle();

    enum CAN_PACKET_ID {
        CAN_PACKET_SET_DUTY = 0,
        CAN_PACKET_SET_CURRENT,
        CAN_PACKET_SET_CURRENT_BRAKE,
        CAN_PACKET_SET_RPM,
        CAN_PACKET_SET_POS,
        CAN_PACKET_SET_CURRENT_REL = 10,
        CAN_PACKET_SET_CURRENT_BRAKE_REL,
        CAN_PACKET_SET_CURRENT_HANDBRAKE,
        CAN_PACKET_SET_CURRENT_HANDBRAKE_REL,
        CAN_PACKET_MAKE_ENUM_32_BITS = 0xFFFFFFFF,
    };

    CanConnection* _can;
    int _canId;

    double _cmdDutyCycle;

};