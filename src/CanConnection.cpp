#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <bitset>

#include "CanConnection.h"

CanConnection::CanConnection() {

#ifndef SIMULATION

    Utils::LogFmt("Connecting to serial port");
    
    _serialPort = open("/dev/ttyTHS0", O_RDWR);
    if (_serialPort < 0) {
        Utils::LogFmt("CanConnection - Failed to open serial port - %s", std::strerror(errno));
    }

    struct termios tty;
    if (tcgetattr(_serialPort, &tty) != 0) {
        Utils::LogFmt("CanConnection - Failed to read serial port config - %s", std::strerror(errno));
    }

    tty.c_cflag = 0b00000000000000000001100010111001;
    tty.c_lflag = 0b00000000000000001000101000110000;
    tty.c_iflag = 0b00000000000000000000000000000110;
    tty.c_oflag = 0b00000000000000000000000000000000;

    // tty.c_cflag &= ~PARENB; // No parity
    // tty.c_cflag |= CSTOPB; // One stop bit
    // tty.c_cflag &= ~CSIZE;
    // tty.c_cflag |= CS8; // 8 bits
    // tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS
    // tty.c_cflag |= CREAD | CLOCAL; // Turn on read and ignore ctrl lines
    // tty.c_lflag &= ~ICANON;
    // tty.c_lflag &= ~ECHO; // Disable echo
    // tty.c_lflag &= ~ECHOE; // Disable erasure
    // tty.c_lflag &= ~ECHONL; // Disable new-line echo
    // tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    // tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    // tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    // tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    // tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    tty.c_cc[VTIME] = 1; // 100ms timeout for read
    tty.c_cc[VMIN] = 0; // minimum number of bytes for read

    // Set in/out baud rate to be 1152000
    cfsetspeed(&tty, B1152000);

    if (tcsetattr(_serialPort, TCSANOW, &tty) != 0) {
        Utils::LogFmt("CanConnection - Failed to set serial port config - %s", std::strerror(errno));
    }

    Utils::LogFmt("Connected to serial port");

#else
#endif
}

void CanConnection::Start() {
    _receiveThread = std::thread(&CanConnection::Recieve, this);
}

void CanConnection::RegisterPacketHandler(uint32_t id, std::function<void(CanFrame)> handler) {
    // Store the provided callback function in the _callbacks map for later use
    _callbacks[id] = handler;
}

void CanConnection::Send(CanFrame in_frame) {

#ifndef SIMULATION

    // Constructs a message packet of there format: #[4 byte arb id][8 bytes of data with leading zero padding]
    char buffer[1+4+8];
    buffer[0] = '#';
    memcpy(buffer+1, &in_frame.arb_id, 4);
    memcpy(buffer+5, in_frame.data, in_frame.len);
    bzero(buffer+13-in_frame.len, 8 - in_frame.len);

    for (int i = 0; i < 13; i++)
    {
        printf("%02X ", buffer[i]);
    }
    std::cout << "\n";

    int bytesLeft = sizeof(buffer);
    int bytesSent = 0;
    int count = 0;
    while(bytesLeft > 0) {
        int nbytes = write(_serialPort, buffer + bytesSent, bytesLeft);
        if (nbytes == -1) {
            Utils::LogFmt("CanConnection::Send Error on write - %s", std::strerror(errno));
            break;
        }
        bytesLeft -= nbytes;
        bytesSent += nbytes;
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
 * @brief This function is responsible for receiving CAN frames from the CAN bus.
 *
 * The function continuously listens for incoming CAN frames on the CAN bus.
 * When a frame is received, it extracts the CAN ID and checks if there is a registered callback function for that ID.
 * If a callback function is found, it is invoked with the received CAN frame as a parameter.
 *
 * @return This function does not return a value. It runs in an infinite loop until the _running flag is set to false.
 */
void CanConnection::Recieve() {

    int nbytes;
    char buffer[256];
    int place = 0;
    int bytesChecked = 0;

    while (_running) {
#ifndef SIMULATION

        // Reads in can frames in the format #[4 byte arb id][8 bytes of data with leading zero padding]
        nbytes = read(_serialPort, buffer+place, sizeof(buffer) - place);
        if (nbytes > 0) {
            place += nbytes;
            bytesChecked = 0;

            // printf("Read %i bytes:", nbytes);
            // for (int i = 0; i < place; i++)
            // {
            //     printf("%02X ", buffer[i]);
            // }
            // std::cout << "\n";

            for (int i = 0; i < place; i++) { // Search buffer for frames
                if (buffer[i] == '#' && place - i >= 13) { // There is a full frame found

                    // Extract and handle can frame
                    CanFrame frame;
                    uint8_t idBytes[4];
                    memcpy(idBytes, buffer+i+1, 4);
                    for (int j = 0; j < 4; j++) {
                        frame.arb_id += idBytes[j] << j*8;    
                    }
                    frame.len = 8;
                    frame.data = (uint8_t*) buffer+i+5;
                    // std::cout << "I found a frame with id " << (((frame.arb_id & 0x0000FF00) >> 8)) << (frame.arb_id & 0x000000FF) << "\n";

                    auto callback = _callbacks.find(frame.arb_id & 0x000000FF);
                    if (callback != _callbacks.end()) { callback->second(frame); }

                    bytesChecked = i + 13;
                }
            }
            // Shift remaining data back, overwriting handled frames and any junk
            place -= bytesChecked;
            memmove(buffer, buffer+bytesChecked, place);
            if (place == sizeof(buffer)) {
                Utils::LogFmt("CanConnection::Recieve buffer overflow");
                place = 0;
            }

        } else if (nbytes < 0) {
            Utils::LogFmt("CanConnection::Recieve Error on read - %s", std::strerror(errno));
        }

#endif
    }
}

void CanConnection::LogFrame(CanFrame frame) {
    printf("CAN Frame id=%08x data=", frame.arb_id);
    for (int i = 0; i < frame.len; i++) { printf("%02x ", frame.data[i]); }
    printf("\r\n");
}

void CanConnection::CloseConnection() {
    Utils::LogFmt("Closing can network");
    _running = false;
    _receiveThread.join();
#ifndef SIMULATION
    close(_serialPort);
#endif
}
