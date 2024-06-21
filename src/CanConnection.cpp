#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdexcept>

#include "CanConnection.h"

CanConnection* CanConnection::_instance = nullptr;

CanConnection::CanConnection() {

    printf("Setting up can0\r\n");
    
    system("sudo ip link set can0 type can bitrate 500000");
    system("sudo ifconfig can0 up");

    printf("Connecting to can0\r\n");

    //1.Create socket
    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket < 0) {
        throw std::runtime_error("Socket PF_CAN failed");
    }
    
    //2.Specify can0 device
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "can0");
    int ret = ioctl(_socket, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        throw std::runtime_error("ioctl failed");
    }
    
    //3.Bind the socket to can0
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        throw std::runtime_error("bind failed");
    }

    setsockopt(_socket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

}

CanConnection* CanConnection::getInstance() {
    if (_instance == nullptr) {
        _instance = new CanConnection();
    }
    return _instance;
}

void CanConnection::send(uint32_t arbId, uint8_t* buffer, size_t len) {
    struct can_frame frame;
    frame.can_id = arbId | CAN_EFF_FLAG;
    frame.len = len;
    memcpy(frame.data, buffer, len);

    // Send message
    int nbytes = write(_socket, &frame, sizeof(frame)); 
    if(nbytes != sizeof(frame)) {
        printf("Failed to send can frame\r\n");
        // system("sudo ifconfig can0 down");
    }
}


void CanConnection::closeConnection() {
    close(_socket);
    system("sudo ifconfig can0 down");
}