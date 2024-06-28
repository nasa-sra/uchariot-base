
#include "StateReporter.h"
#include "subsystems/IMU.h"

IMU::IMU() {

    _imuFd = wiringPiI2CSetup(LSM6DSOX_ADDR);
    if (_imuFd == -1) {
        Utils::LogFmt("IMU failed to init i2c communication");
    }

    setAccConfig(0);
    setGyroConfig(0);
}

int IMU::setAccConfig(int config_num) {
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

int IMU::setGyroConfig(int config_num){
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

void IMU::Update(double dt) {

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

 void IMU::ReportState(std::string prefix) {
    // StateReporter::GetInstance().UpdateKey(prefix + "roll", _gyroRates.x);
    // StateReporter::GetInstance().UpdateKey(prefix + "pitch", _gyroRates.y);
    StateReporter::GetInstance().UpdateKey(prefix + "yaw", _gyroAngles.z);
}

 Utils::Vector3 IMU::getAccelerations() {
    return _accelerations;
 }

 Utils::Vector3 IMU::getGyroRates() {
    return _gyroRates;
 }

float IMU::getYaw() {
    return _gyroAngles.z * 1.3;
}