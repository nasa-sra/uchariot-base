
#include <wiringPiI2C.h>

#include "StateReporter.h"
#include "subsystems/BNO055.h"

#define BNO055_ADDR 0x29

#define EUL_X_LOW_REG 0x1A
#define EUL_Y_LOW_REG 0x1C
#define EUL_Z_LOW_REG 0x1E

BNO055::BNO055() : IMUBase() {

    _imuFd = wiringPiI2CSetup(BNO055_ADDR);
    if (_imuFd == -1) {
        Utils::LogFmt("BNO055 IMU failed to init I2C communication");
    }

}

void BNO055::Update(double dt) {

    int16_t x, y, z;
    x = wiringPiI2CReadReg16(_imuFd, EUL_X_LOW_REG);
    y = wiringPiI2CReadReg16(_imuFd, EUL_Y_LOW_REG);
    z = wiringPiI2CReadReg16(_imuFd, EUL_Z_LOW_REG);

    _gyroAngles.x() = ((float)x) / 16;
    _gyroAngles.y() = ((float)y) / 16;
    _gyroAngles.z() = ((float)z) / 16;

 }