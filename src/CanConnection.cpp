#include "CanConnection.h"

CanConnection::CanConnection() {

    Utils::LogFmt("Setting up can0\r\n");

#ifdef linux
    
    system("sudo ip link set can0 type can bitrate 500000");
    system("sudo ifconfig can0 up");

    Utils::LogFmt("Connecting to can0");

    // 1.Create socket
    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket < 0) {
        Utils::ErrFmt("CAN socket PF_CAN failed");
    }
    
    // 2.Specify can0 device
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "can0");
    int ret = ioctl(_socket, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        Utils::ErrFmt("CAN socket ioctl failed");
    }
    
    // 3.Bind the socket to can0
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        Utils::ErrFmt("CAN socket bind failed");
    }

    setsockopt(_socket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

#endif

}

void CanConnection::Send(CanFrame* frame) {

#ifdef linux
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
#else
    // LogFrame(frame);
#endif
}

void CanConnection::LogFrame(CanFrame* frame) {
    printf("CAN Frame id=%08x data=", frame->arb_id);
    for (int i = 0; i < frame->len; i++) {
        printf("%02x ", frame->data[i]);
    }
    printf("\r\n");
}

void CanConnection::CloseConnection() {
#ifdef linux
    close(_socket);
    system("sudo ifconfig can0 down");
#endif
}