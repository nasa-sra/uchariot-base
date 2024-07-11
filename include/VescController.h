#pragma once

#include <string>

#include "CanConnection.h"
#include "StateReporter.h"

class VescController {
public:
    enum Mode {
        DUTY_CYCLE = 0,
        VELOCITY,
        POSITION,
        CURRENT
    };

    VescController(uint16_t id);

    void Update();

    void SetMode(Mode mode);
    void SetCmd(float cmd);
    void SetScale(float scale) {_scale = scale;}

    double GetVoltage() {return _voltageIn;}

    void packetHandler(CanFrame frame);
    void ReportState(std::string prefix = "/");

private:
    enum CAN_CMD_PACKET_ID {
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

    enum CAN_STATUS_PACKET_ID {
        CAN_PACKET_STATUS = 9, //	ERPM, Current, Duty Cycle
        CAN_PACKET_STATUS_2 = 14, // Ah Used, Ah Charged
        CAN_PACKET_STATUS_3, // Wh Used, Wh Charged
        CAN_PACKET_STATUS_4, // Temp Fet, Temp Motor, Current In, PID position
        CAN_PACKET_STATUS_5	= 27, // Tachometer, Voltage In
        CAN_PACKET_STATUS_6, //ADC1, ADC2, ADC3, PPM
    };

    void readStatus1Packet(uint8_t* data);
    void readStatus5Packet(uint8_t* data);

    void sendDutyCycle(float dc);
    void sendRPM(float rpm);
    void sendPosition(float deg);
    void sendCurrent(float amps);

    CanConnection* _can;
    uint16_t _can_id;
    float _scale {1.0};
    int _disconnectTimer{0};

    bool _connected{false};
    int _warningCounter{0};
    Mode _mode {DUTY_CYCLE};
    double _cmdDutyCycle{0.0};
    double _cmdVelocity{0.0};
    double _cmdPosition{0.0};
    double _cmdCurrent{0.0};

    double _velocity{0.0};
    double _current{0.0};
    double _output{0.0};
    double _voltageIn{0.0};
};