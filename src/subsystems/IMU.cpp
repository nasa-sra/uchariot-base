#include "IMU.h"
#include "Utils.h"

int IMU::setAccConfig(int config_num){
    int status;

    switch(config_num){
        case 0: // range = +- 2 g
            _acc_lsb_to_g = 0.061;
            status = accelGyro.writeRegister(_accel_register, 0b10100010);
            break;
        case 1: // range = +- 4 g
            _acc_lsb_to_g = 0.122;
            status = accelGyro.writeRegister(_accel_register, 0b10101010);
            break;
        case 2: // range = +- 8 g
            _acc_lsb_to_g = 0.244;
            status = accelGyro.writeRegister(_accel_register, 0b10101110);
            break;
        case 3: // range = +- 16 g
            _acc_lsb_to_g = 0.488;
            status = accelGyro.writeRegister(_accel_register, 0b10100110);
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
            status = accelGyro.writeRegister(_gyro_register, 0b10100000);
            break;
        case 1:  // range = +- 500 deg/s
            _gyro_lsb_to_degsec = 17.5;
            status = accelGyro.writeRegister(_gyro_register, 0b10100100);
            break;
        case 2: // range = +- 1000 deg/s
            _gyro_lsb_to_degsec = 35;
            status = accelGyro.writeRegister(_gyro_register, 0b10101000);
            break;
        case 3: // range = +- 2000 deg/s
            _gyro_lsb_to_degsec = 70;
            status = accelGyro.writeRegister(_gyro_register, 0b10101100);
            break;
        default:
            status = 1;
            break;
    }
    return status;
}

void IMU::fetchData(){
    int16_t X, Y, Z;
    X = accelGyro.readRegister(0x29) << 8 | accelGyro.readRegister(0x28);
    Y = accelGyro.readRegister(0x2B) << 8 | accelGyro.readRegister(0x2A);
    Z = accelGyro.readRegister(0x2D)  << 8 | accelGyro.readRegister(0x2C);

    accel[0] = ((float)X) * _acc_lsb_to_g / 1000; // - accXoffset;
    accel[1] = ((float)Y) * _acc_lsb_to_g / 1000; // - accYoffset;
    accel[2] = ((float)Z) * _acc_lsb_to_g / 1000; // - accZoffset;

    X = accelGyro.readRegister(0x23) << 8 | accelGyro.readRegister(0x24);
    Y = accelGyro.readRegister(0x25) << 8 | accelGyro.readRegister(0x24);
    Z = accelGyro.readRegister(0x27) << 8 | accelGyro.readRegister(0x26);

    gyro[0] = ((float)X) * _gyro_lsb_to_degsec / 1000; // - gyroXoffset;
    gyro[1] = ((float)Y) * _gyro_lsb_to_degsec / 1000; // - gyroYoffset;
    gyro[2] = ((float)Z) * _gyro_lsb_to_degsec / 1000; // - gyroZoffset;
 }

 float IMU::getAccel(int index){
    return accel[index];
 }

 float IMU::getGyro(int index){
    return gyro[index];
 }

 float* IMU::getAccelRaw(){
    return accel;
 }

 float* IMU::getGyroRaw(){
    return gyro;
 }