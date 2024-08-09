#include "CanConnection.h"

#include <fcntl.h>

CanConnection::CanConnection() {
    Utils::LogFmt("Setting up can0");

#ifndef SIMULATION
    // system("sudo modprobe can");
    // system("sudo modprobe can_raw");
    // system("sudo modprobe mttcan");
    // system("sudo ip link set can0 up type can bitrate 500000 restart-ms 100");
    // system("pkill slcand");
    // system("sudo slcand -o -s6 -S 1152000 /dev/ttyACM1");
    system("sudo ip link set up can0");
    system("sudo ifconfig can0 txqueuelen 1000");

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
    ret = bind(_socket, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        Utils::ErrFmt("CAN socket bind failed");
    }

    setsockopt(_socket, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    int flags = fcntl(_socket, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(_socket, F_SETFL, flags);

#else
#endif
}

void CanConnection::Start() {
    _receiveThread = std::thread(&CanConnection::Recieve, this);
}

void CanConnection::RegisterPacketHandler(
    uint16_t id, std::function<void(CanFrame)> handler) {
    struct can_filter filter;
    filter.can_id = id;            // Set the CAN ID for the filter
    filter.can_mask = 0x000000FF;  // Set the mask for the filter. This will
                                   // match any CAN ID with the same lower byte.

    _filters.push_back(filter);  // Add the filter to the list of filters

    // Apply the list of filters to the CAN socket using setsockopt function
    setsockopt(_socket, SOL_CAN_RAW, CAN_RAW_FILTER, _filters.data(),
               sizeof(can_filter) * _filters.size());

    // Store the provided callback function in the _callbacks map for later use
    _callbacks[id] = handler;
}

void CanConnection::Send(CanFrame in_frame) {
#ifndef SIMULATION
    struct can_frame frame;
    frame.can_id = in_frame.arb_id | CAN_EFF_FLAG;
    frame.len = in_frame.len;
    memcpy(frame.data, in_frame.data, in_frame.len);

    // Send message
    int bytesleft = sizeof(frame);
    int count = 0;
    while (bytesleft > 0) {
        int nbytes = write(_socket, &frame, bytesleft);
        if (nbytes == -1) {
            Utils::LogFmt("CanConnection::Send Error on write - %s",
                          std::strerror(errno));
            break;
        }
        bytesleft -= nbytes;
        if (count > 100) {
            Utils::LogFmt("Failed to send can frame");
            break;
        }
        count++;
    }

#else
    // LogFrame(in_frame);
#endif
}

/**
 * @brief This function is responsible for receiving CAN frames from the CAN
 * bus.
 *
 * The function continuously listens for incoming CAN frames on the CAN bus.
 * When a frame is received, it extracts the CAN ID and checks if there is a
 * registered callback function for that ID. If a callback function is found, it
 * is invoked with the received CAN frame as a parameter.
 *
 * @return This function does not return a value. It runs in an infinite loop
 * until the _running flag is set to false.
 */
void CanConnection::Recieve() {
    int nbytes;
    struct can_frame frame;

    fd_set fds;
    struct timeval tv;

    while (_running) {
#ifndef SIMULATION

        tv.tv_sec = 1;
        tv.tv_usec = 0;
        FD_ZERO(&fds);
        FD_SET(_socket, &fds);

        if (select(_socket + 1, &fds, NULL, NULL, &tv) == -1) {
            Utils::ErrFmt("CanConnection:Recieve - Error on select");
        }
        if (FD_ISSET(_socket, &fds)) {
            nbytes = read(_socket, &frame, sizeof(frame));
            if (nbytes > 0) {
                uint16_t id = frame.can_id & 0x000000FF;
                auto callback = _callbacks.find(id);
                if (callback != _callbacks.end()) {
                    callback->second(CanFrame(frame));
                }
                // printf("can_id = 0x%X\r\ncan_dlc = %d \r\n", frame.can_id,
                // frame.can_dlc); int i = 0; for(i = 0; i < 8; i++)
                //     printf("data[%d] = %d\r\n", i, frame.data[i]);
            }
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
    _running = false;
    _receiveThread.join();
#ifndef SIMULATION
    close(_socket);
    system("sudo ifconfig can0 down");
    // system("sudo slcand -c -s6 -S 1152000 /dev/ttyACM1");
#endif
}