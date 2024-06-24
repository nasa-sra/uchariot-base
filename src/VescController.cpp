#include <stdio.h>

#include "VescController.h"

#include "Utils.h"

void VescController::SetMode(Mode mode) {
    _mode = mode;
}

void VescController::SetCmd(float cmd) {
    switch (_mode)
    {
    case DUTY_CYCLE:
        sendDutyCycle(cmd);
        break;
    case VELOCITY:
        sendRPM(cmd * _scale);
        break;
    case POSITION:
        sendPosition(cmd * _scale);
        break;
    case CURRENT:
        sendCurrent(cmd);
        break;
    default:
        break;
    }
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

