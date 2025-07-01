#include "Lidar.h"
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <cstring>
#include <iostream>

LidarLD06::LidarLD06(const std::string &port, int baudrate) : _fd(-1), _running(false) {
    _fd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (_fd < 0) {
        perror("Failed to open LIDAR port");
        return;
    }

    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(_fd, &tty) != 0) {
        perror("tcgetattr failed");
        close(_fd);
        _fd = -1;
        return;
    }

    cfsetospeed(&tty, B230400);
    cfsetispeed(&tty, B230400);
    tty.c_cflag |= (CLOCAL | CREAD);    // enable receiver
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;                 // 8-bit characters
    tty.c_cflag &= ~PARENB;             // no parity bit
    tty.c_cflag &= ~CSTOPB;             // only need 1 stop bit
    tty.c_cflag &= ~CRTSCTS;            // no hardware flow control
    tty.c_lflag = 0;                    // no signaling chars, no echo
    tty.c_oflag = 0;                    // no remapping, no delays
    tty.c_cc[VMIN]  = 0;                // non-blocking read
    tty.c_cc[VTIME] = 10;               // 1 sec read timeout

    tcsetattr(_fd, TCSANOW, &tty);
}

LidarLD06::~LidarLD06() {
    Stop();
    if (_fd >= 0) close(_fd);
}

void LidarLD06::Start() {
    _running = true;
    _readThread = std::thread(&LidarLD06::ReadLoop, this);
}

void LidarLD06::Stop() {
    _running = false;
    if (_readThread.joinable()) {
        _readThread.join();
    }
}

void LidarLD06::Update(double dt) {
    // Optional: Add filtering or data aggregation here
}

std::vector<LidarPoint> LidarLD06::GetLatestScan() {
    std::lock_guard<std::mutex> lock(_dataMutex);
    return _latestScan;
}

void LidarLD06::ReadLoop() {
    std::vector<uint8_t> buffer;
    buffer.resize(1024);
    uint8_t packet[47];

    while (_running && _fd >= 0) {
        int len = read(_fd, buffer.data(), buffer.size());
        if (len <= 0) continue;

        for (int i = 0; i < len; ++i) {
            static int idx = 0;
            packet[idx++] = buffer[i];

            // Check header (0x54 0x2C) and packet length
            if (idx >= 47 && packet[0] == 0x54 && packet[1] == 0x2C) {
                if (ParsePacket(packet, 47)) {
                    // Parsed successfully
                }
                idx = 0;
            }

            if (idx >= 47) {
                idx = 0;
            }
        }
    }
}

bool LidarLD06::ParsePacket(const uint8_t *buf, size_t length) {
    if (length < 47 || buf[0] != 0x54 || buf[1] != 0x2C) return false;

    uint8_t speed_l = buf[2];
    uint8_t speed_h = buf[3];
    float speed_rpm = ((speed_h << 8) | speed_l) / 64.0;

    int start_angle = ((buf[5] << 8) | buf[4]) / 100;
    int end_angle   = ((buf[43] << 8) | buf[42]) / 100;

    std::vector<LidarPoint> points;

    for (int i = 0; i < 12; ++i) {
        int offset = 6 + i * 3;
        uint16_t distance = (buf[offset+1] << 8) | buf[offset];
        uint8_t quality = buf[offset + 2];

        float angle = start_angle + (end_angle - start_angle) * (i / 11.0f);
        if (angle < 0) angle += 360;
        if (angle >= 360) angle -= 360;

        points.push_back(LidarPoint{
            .angle_deg = angle,
            .distance_m = distance / 1000.0f,
            .quality = quality
        });
    }

    std::lock_guard<std::mutex> lock(_dataMutex);
    _latestScan = points;
    return true;
}
