#pragma once

#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

struct LidarPoint {
    float angle_deg;
    float distance_m;
    int quality;
};

class LidarLD06 {
public:
    LidarLD06(const std::string &port = "/dev/ttyUSB0", int baudrate = 230400);
    ~LidarLD06();

    void Start();
    void Stop();
    void Update(double dt); // Could trigger additional processing
    std::vector<LidarPoint> GetLatestScan();

private:
    void ReadLoop();
    bool ParsePacket(const uint8_t *buffer, size_t length);

    int _fd;
    std::thread _readThread;
    std::atomic<bool> _running;
    std::mutex _dataMutex;

    std::vector<LidarPoint> _latestScan;
};
