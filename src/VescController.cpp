#include <stdio.h>

#include "VescController.h"

#include "Utils.h"

void VescController::SetCmd(float cmd) {
    uint32_t arbId = _can_id | ((uint32_t) CAN_PACKET_SET_DUTY << 8);
    int32_t send_index = 0;
	uint8_t buffer[4];
	Utils::BufferAppendInt32(buffer, (int32_t)(cmd * 100000.0), &send_index);
    _can->Send(new CanFrame(arbId, buffer, send_index));
}
