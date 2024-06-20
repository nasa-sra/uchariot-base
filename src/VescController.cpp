#include <stdio.h>
#include "Utils.h"

#include "VescController.h"

VescController::VescController(int id) {

    _can = CanConnection::getInstance();
    _canId = id;

    _cmdDutyCycle = 0;
}

void VescController::setCmd(float cmd) {
    _cmdDutyCycle = cmd;
    sendDutyCycle();
}

void VescController::sendDutyCycle() {
    uint32_t arbId = _canId | ((uint32_t) CAN_PACKET_SET_DUTY << 8);
    int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::buffer_append_int32(buffer, (int32_t)(_cmdDutyCycle * 100000.0), &send_index);

    _can->send(arbId, buffer, send_index);
}
