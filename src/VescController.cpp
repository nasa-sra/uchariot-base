#include <stdio.h>

#include "VescController.h"

#include "Utils.h"

VescController::VescController(uint16_t id) : 
    _can(&CanConnection::GetInstance()) {
    
    _can_id = id;
    _can->RegisterPacketHandler(id, [this](CanFrame frame) {this->packetHandler(frame);});
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
            _cmdVelocity = cmd * _scale;
            sendRPM(_cmdVelocity);
            break;
        case POSITION:
            _cmdPosition = cmd * _scale;
            sendPosition(_cmdPosition);
            break;
        case CURRENT:
            _cmdCurrent = cmd;
            sendCurrent(_cmdCurrent);
            break;
        default:
            break;
    }
}

void VescController::packetHandler(CanFrame frame) {
    uint8_t statusID = (frame.arb_id & 0x0000FF00) >> 8;

    // std::cout << statusID << ": 0x" << std::hex;
    // int i = 0;
    // for(i = 0; i < 8; i++)
    //     std::cout << frame.data[i];
    // std::cout << std::endl;
    
    switch (statusID) {
        case CAN_PACKET_STATUS:
            readStatus1Packet(frame.data);
            break;
        default:
            break;
    }
}

void VescController::readStatus1Packet(uint8_t* data) {
    memcpy(&_velocity, data, 4);
    _velocity /= _scale;
    memcpy(&_current, data+4, 2);
    _current /= 10;
    memcpy(&_output, data+6, 2);
    _output /= 1000;
    // Utils::LogFmt("id: %i velocity: %f", _can_id, _velocity);
}

void VescController::sendDutyCycle(float dc) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_DUTY << 8);
    int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t)(dc * 100000.0), &send_index);
    _can->Send(new CanFrame(arbId, buffer, send_index));
}

void VescController::sendRPM(float rpm) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_RPM << 8);
	int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t) rpm, &send_index);
    _can->Send(new CanFrame(arbId, buffer, send_index));
}

void VescController::sendPosition(float deg) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_POS << 8);
	int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t)(deg * 1000000.0), &send_index);
    _can->Send(new CanFrame(arbId, buffer, send_index));
}

void VescController::sendCurrent(float amps) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_CURRENT << 8);
	int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t) (amps * 1000.0), &send_index);
    _can->Send(new CanFrame(arbId, buffer, send_index));
}

