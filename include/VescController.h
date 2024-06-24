#pragma once

#include "CanConnection.h"

class VescController {
public:
    enum Mode {
        DUTY_CYCLE = 0,
        VELOCITY,
        POSITION,
        CURRENT
    };

    VescController(uint16_t id) : _can(&CanConnection::GetInstance()), _can_id(id) {}

    void SetMode(Mode mode);
    void SetCmd(float cmd);

    void SetScale(float scale) {_scale = scale;}

private:
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

    void sendDutyCycle(float dc);
    void sendRPM(float rpm);
    void sendPosition(float deg);
    void sendCurrent(float amps);

    CanConnection* _can;
    uint16_t _can_id;
    Mode _mode {DUTY_CYCLE};
    float _scale {1.0};
};