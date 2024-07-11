
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
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", _adapter_nr);
    _imuFd = open(filename, O_RDWR);
    if (_imuFd < 0) {
        Utils::ErrFmt("Failed to open communication for I2C address %d", BNO055_ADDR);
    }

    if (ioctl(_imuFd, I2C_SLAVE, BNO055_ADDR) < 0) {
        Utils::ErrFmt("Failed to configure the parameters for I2C address %d", BNO055_ADDR);
    }
    uint8_t status = readRegister(STATUS_REG);

    writeRegister(OPR_MODE_REG, 0b00001000); // Set IMU mode
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uint8_t calib_status = readRegister(CALIB_STATUS_REG);
    Utils::LogFmt("Connected to BNO055 IMU Status %i, Calibration %i", status, (calib_status & 0b11000000) >> 6);
    if (status == 1) {
        uint8_t err = readRegister(ERR_REG);
        Utils::LogFmt("BNO055 Error Code %i", err);
    }
}

void BNO055::Update(double dt) {

    int16_t x, y, z;
    x = readRegister(EUL_Z_LOW_REG);
    y = readRegister(EUL_Y_LOW_REG);
    z = readRegister(EUL_X_LOW_REG);

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

 int BNO055::readRegister(uint8_t register_add){
    int32_t res;

    res = i2c_smbus_read_word_data(_imuFd, register_add);
    if (res < 0) {
        Utils::ErrFmt("Read from I2C address %d failed", BNO055_ADDR);
        return -1; //Do I need this?
    } else {
        return res;
    }
}

int BNO055::writeRegister(uint8_t register_addr, uint8_t value) {
    int toReturn = i2c_smbus_write_word_data(_imuFd, register_addr, value);
    if(toReturn < 0){
        Utils::ErrFmt("Write to I2C address %d failed", BNO055_ADDR);
        return -1;
    }

    return toReturn;
}