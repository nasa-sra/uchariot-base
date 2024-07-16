#include "CanConnection.h"

CanConnection::CanConnection() {

    Utils::LogFmt("Setting up can0");

#ifndef SIMULATION
    system("sudo modprobe can");
    system("sudo modprobe can_raw");
    system("sudo modprobe mttcan");
    system("sudo ip link set can0 up type can bitrate 500000 berr-reporting on");
    system("sudo ip link set can0 up");

    Utils::LogFmt("Connecting to can0");

    // 1.Create socket
    _socket = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (_socket < 0) { Utils::ErrFmt("CAN socket PF_CAN failed"); }

    // 2.Specify can0 device
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "can0");
    int ret = ioctl(_socket, SIOCGIFINDEX, &ifr);
    if (ret < 0) { Utils::ErrFmt("CAN socket ioctl failed"); }

    // 3.Bind the socket to can0
    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) { Utils::ErrFmt("CAN socket bind failed"); }

    setsockopt(_socket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
#else
#endif
}

void CanConnection::Start(bool& running) {
    _receiveThread = std::thread(&CanConnection::Recieve, this, std::ref(running));
}

void CanConnection::RegisterPacketHandler(uint16_t id, std::function<void(CanFrame)> handler) {
    struct can_filter filter;
    filter.can_id = id;
    filter.can_mask = 0x000000FF;
    _filters.push_back(filter);
    setsockopt(_socket, SOL_CAN_RAW, CAN_RAW_FILTER, _filters.data(), sizeof(can_filter) * _filters.size());

    _callbacks[id] = handler;
}

void CanConnection::Send(CanFrame in_frame) {

#ifndef SIMULATION
    struct can_frame frame;
    frame.can_id = in_frame.arb_id | CAN_EFF_FLAG;
    frame.len = in_frame.len;
    memcpy(frame.data, in_frame.data, in_frame.len);

    // Send message
    int nbytes = write(_socket, &frame, sizeof(frame));
    if (nbytes != sizeof(frame)) {
        printf("Failed to send can frame\r\n");
        // system("sudo ifconfig can0 down");
    }
#else
    // LogFrame(in_frame);
#endif
}

void CanConnection::Recieve(bool& running) {
    int nbytes;
    struct can_frame frame;
    while (running) {
#ifndef SIMULATION
        nbytes = read(_socket, &frame, sizeof(frame));
        if (nbytes > 0) {
            uint16_t id = frame.can_id & 0x000000FF;
            auto callback = _callbacks.find(id);
            if (callback != _callbacks.end()) { callback->second(CanFrame(frame)); }
            // printf("can_id = 0x%X\r\ncan_dlc = %d \r\n", frame.can_id, frame.can_dlc);
            // int i = 0;
            // for(i = 0; i < 8; i++)
            //     printf("data[%d] = %d\r\n", i, frame.data[i]);
        }
#endif
    }
}

void CanConnection::LogFrame(CanFrame frame) {
    printf("CAN Frame id=%08x data=", frame.arb_id);
    for (int i = 0; i < frame.len; i++) {
        printf("%02x ", frame.data[i]);
    }
    printf("\r\n");
}

void CanConnection::CloseConnection() {
    Utils::LogFmt("Closing can network");
    _receiveThread.join();
#ifndef SIMULATION
    close(_socket);
    system("sudo ifconfig can0 down");
#endif
}