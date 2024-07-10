
#include <wiringPiI2C.h>

#include "StateReporter.h"
#include "subsystems/BNO055.h"

#define BNO055_ADDR 0x28

#define OPR_MODE_REG 0x3D
#define CALIB_STATUS_REG 0x35
#define STATUS_REG 0x39
#define ERR_REG 0x3A

#define EUL_X_LOW_REG 0x1A
#define EUL_Y_LOW_REG 0x1C
#define EUL_Z_LOW_REG 0x1D

BNO055::BNO055() : IMUBase() {

    _imuFd = wiringPiI2CSetup(BNO055_ADDR);
    uint8_t status = wiringPiI2CReadReg8(_imuFd, STATUS_REG);
    if (_imuFd < 0 || status > 6) {
        Utils::LogFmt("BNO055 IMU failed to init I2C communication");
    } else {
        wiringPiI2CWriteReg8(_imuFd, OPR_MODE_REG, 0b00001000); // Set IMU mode
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        uint8_t calib_status = wiringPiI2CReadReg8(_imuFd, CALIB_STATUS_REG);
        Utils::LogFmt("Connected to BNO055 IMU Status %i, Calibration %i", status, (calib_status & 0b11000000) >> 6);
        if (status == 1) {
            uint8_t err = wiringPiI2CReadReg8(_imuFd, ERR_REG);
            Utils::LogFmt("BNO055 Error Code %i", err);
        }
    }

}

void BNO055::Update(double dt) {

    int16_t x, y, z;
    x = wiringPiI2CReadReg16(_imuFd, EUL_Z_LOW_REG);
    y = wiringPiI2CReadReg16(_imuFd, EUL_Y_LOW_REG);
    z = wiringPiI2CReadReg16(_imuFd, EUL_X_LOW_REG);

    _gyroAngles.x() = ((float)x) / 16;
    _gyroAngles.y() = ((float)y) / 16;
    _gyroAngles.z() = ((float)z) / 16;

    // static int count = 0;
    // count++;
    // if (count % 10 == 0) {
    //     uint8_t status = wiringPiI2CReadReg8(_imuFd, STATUS_REG);
    //     uint8_t calib_status = wiringPiI2CReadReg8(_imuFd, CALIB_STATUS_REG);
    //     Utils::LogFmt("Connected to BNO055 IMU Status %i, Calibration %i", status, (calib_status & 0b11000000) >> 6);
    //     Utils::LogFmt("Roll: %f Pitch: %f Yaw: %f", _gyroAngles.x(), _gyroAngles.y(), _gyroAngles.z());
    // }

 }