#include <stdio.h>

#include "VescController.h"

#include "Utils.h"

VescController::VescController(uint16_t id) : 
    _can(&CanConnection::GetInstance()) {
    
    _can_id = id;
    _can->RegisterPacketHandler(id, [this](CanFrame frame) {this->packetHandler(frame);});
}

void VescController::Update() {
    _connected = _disconnectTimer < 10;
    _disconnectTimer++;

#ifndef SIMULATION
    if (!_connected) {
        _warningCounter++;
        if (_warningCounter == 10) {
            Utils::LogFmt("Warning: Vesc Controller Id %i is not connected", _can_id);
            _warningCounter = 0;
        }
    }
#endif
}

void VescController::SetMode(Mode mode) {
    _mode = mode;
}

void VescController::SetCmd(float cmd) {
    switch (_mode) {
        case DUTY_CYCLE:
            _cmdDutyCycle = cmd;
            sendDutyCycle(_cmdDutyCycle);
            break;
        case VELOCITY:
            _cmdVelocity = cmd;
            sendRPM(_cmdVelocity * _scale);
            break;
        case POSITION:
            _cmdPosition = cmd;
            sendPosition(_cmdPosition * _scale);
            break;
        case CURRENT:
            _cmdCurrent = cmd;
            sendCurrent(_cmdCurrent);
            break;
        default:
            break;
    }
}

static void print_buf(const char *title, const unsigned char *buf, size_t buf_len)
{
    size_t i = 0;
    for(i = 0; i < buf_len; ++i)
    fprintf(stdout, "%02X%s", buf[i],
             ( i + 1 ) % 16 == 0 ? "\r\n" : " " );

}

void VescController::packetHandler(CanFrame frame) {
    uint8_t statusID = (frame.arb_id & 0x0000FF00) >> 8;
    
    switch (statusID) {
        case CAN_PACKET_STATUS:
            readStatus1Packet(frame.data);
            break;
        case CAN_PACKET_STATUS_5:
            readStatus5Packet(frame.data);
            break;
        default:
            break;
    }

    _disconnectTimer = 0;
}

void VescController::readStatus1Packet(uint8_t* data) {
    std::reverse(data, data + 8); // Convert to little endian
    int16_t buf;
    memcpy(&buf, data, 2);
    _output = buf / 1000;
    memcpy(&buf, data+2, 2);
    _current = buf / 10;
    int32_t lBuf;
    memcpy(&lBuf, data+4, 4);
    _velocity = lBuf / _scale;
}

void VescController::readStatus5Packet(uint8_t* data) {
    std::reverse(data, data + 8); // Convert to little endian
    int32_t buf;
    memcpy(&buf, data+2, 2);
    _voltageIn = buf / 10;
    // ignore tachometer
}

void VescController::sendDutyCycle(float dc) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_DUTY << 8);
    int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t)(dc * 100000.0), &send_index);
    _can->Send(CanFrame(arbId, buffer, send_index));
}

void VescController::sendRPM(float rpm) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_RPM << 8);
	int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t) rpm, &send_index);
    _can->Send(CanFrame(arbId, buffer, send_index));
}

void VescController::sendPosition(float deg) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_POS << 8);
	int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t)(deg * 1000000.0), &send_index);
    _can->Send(CanFrame(arbId, buffer, send_index));
}

void VescController::sendCurrent(float amps) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_CURRENT << 8);
	int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t) (amps * 1000.0), &send_index);
    _can->Send(CanFrame(arbId, buffer, send_index));
}

void VescController::ReportState(std::string prefix) {
    StateReporter::GetInstance().UpdateKey(prefix + "connected", _connected);
    StateReporter::GetInstance().UpdateKey(prefix + "mode", modeToString(_mode));
    // StateReporter::GetInstance().UpdateKey(prefix + "cmd_duty_cycle", _cmdDutyCycle);
    StateReporter::GetInstance().UpdateKey(prefix + "cmd_velocity", _cmdVelocity);
    // StateReporter::GetInstance().UpdateKey(prefix + "cmd_position", _cmdPosition);
    // StateReporter::GetInstance().UpdateKey(prefix + "cmd_current", _cmdCurrent);
    StateReporter::GetInstance().UpdateKey(prefix + "velocity", _velocity);
    StateReporter::GetInstance().UpdateKey(prefix + "current", _current);
    StateReporter::GetInstance().UpdateKey(prefix + "output", _output);
    // StateReporter::GetInstance().UpdateKey(prefix + "voltage_in", _voltageIn);
}

std::string VescController::modeToString(VescController::Mode mode) {
    static std::string map[4] = {"duty cycle", "velocity", "position", "current"};
    return map[mode];
}
