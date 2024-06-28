
#include <wiringPiI2C.h>

#include "StateReporter.h"
#include "subsystems/LSM6DSOX.h"

#define LSM6DSOX_ADDR 0x6A
#define ACCEL_REG 0x10
#define GYRO_REG 0x11

#define GYRO_X_LOW 0x22
#define GYRO_Y_LOW 0x24
#define GYRO_Z_LOW 0x26

#define ACCEL_X_LOW 0x28
#define ACCEL_Y_LOW 0x2A
#define ACCEL_Z_LOW 0x2C

LSM6DSOX::LSM6DSOX() : IMUBase() {

    _imuFd = wiringPiI2CSetup(LSM6DSOX_ADDR);
    if (_imuFd == -1) {
        Utils::LogFmt("LSM6DSOX IMU failed to init i2c communication");
    }

    setAccConfig(0);
    setGyroConfig(0);
}

int LSM6DSOX::setAccConfig(int config_num) {
    int status;

    switch(config_num){
        case 0: // range = +- 2 g
            _acc_lsb_to_g = 0.061;
            status = wiringPiI2CWriteReg8(_imuFd, ACCEL_REG, 0b10100010);
            break;
        case 1: // range = +- 4 g
            _acc_lsb_to_g = 0.122;
            status = wiringPiI2CWriteReg8(_imuFd, ACCEL_REG, 0b10101010);
            break;
        case 2: // range = +- 8 g
            _acc_lsb_to_g = 0.244;
            status = wiringPiI2CWriteReg8(_imuFd, ACCEL_REG, 0b10101110);
            break;
        case 3: // range = +- 16 g
            _acc_lsb_to_g = 0.488;
            status = wiringPiI2CWriteReg8(_imuFd, ACCEL_REG, 0b10100110);
            break;
        default: // error
            status = 1;
            break;
    }
    return status;
}

int LSM6DSOX::setGyroConfig(int config_num){
    int status;

    switch(config_num){
        case 0:  // range = +- 250 deg/s
            _gyro_lsb_to_degsec = 8.75;
            status = wiringPiI2CWriteReg8(_imuFd, GYRO_REG, 0b10100000);
            break;
        case 1:  // range = +- 500 deg/s
            _gyro_lsb_to_degsec = 17.5;
            status = wiringPiI2CWriteReg8(_imuFd, GYRO_REG, 0b10100100);
            break;
        case 2: // range = +- 1000 deg/s
            _gyro_lsb_to_degsec = 35;
            status = wiringPiI2CWriteReg8(_imuFd, GYRO_REG, 0b10101000);
            break;
        case 3: // range = +- 2000 deg/s
            _gyro_lsb_to_degsec = 70;
            status = wiringPiI2CWriteReg8(_imuFd, GYRO_REG, 0b10101100);
            break;
        default:
            status = 1;
            break;
    }
    return status;
}

void LSM6DSOX::Update(double dt) {

    int16_t x, y, z;
    x = wiringPiI2CReadReg16(_imuFd, GYRO_X_LOW);
    y = wiringPiI2CReadReg16(_imuFd, GYRO_Y_LOW);
    z = wiringPiI2CReadReg16(_imuFd, GYRO_Z_LOW);

    _gyroRates.x = ((float)x) * _gyro_lsb_to_degsec / 1000; // - gyroXoffset;
    _gyroRates.y = ((float)y) * _gyro_lsb_to_degsec / 1000; // - gyroYoffset;
    _gyroRates.z = ((float)z) * _gyro_lsb_to_degsec / 1000; // - gyroZoffset;
    
    x = wiringPiI2CReadReg16(_imuFd, ACCEL_X_LOW);
    y = wiringPiI2CReadReg16(_imuFd, ACCEL_Y_LOW);
    z = wiringPiI2CReadReg16(_imuFd, ACCEL_Z_LOW);

    _accelerations.x = ((float)x) * _acc_lsb_to_g / 1000; // - accXoffset;
    _accelerations.y = ((float)y) * _acc_lsb_to_g / 1000; // - accYoffset;
    _accelerations.z = ((float)z) * _acc_lsb_to_g / 1000; // - accZoffset;

    float driftFactor = 0.0075;
    _gyroAngles.x += _gyroRates.x * dt + driftFactor; // integrate angular rotation for angles
    _gyroAngles.y += _gyroRates.y * dt + driftFactor; // integrate angular rotation for angles
    _gyroAngles.z += _gyroRates.z * dt + driftFactor; // integrate angular rotation for angles

 }