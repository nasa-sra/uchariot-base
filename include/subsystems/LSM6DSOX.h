#pragma once

#include "subsystems/IMUBase.h"
#include "Utils.h"

#define LSM6DSOX_ADDR 0x6A
#define ACCEL_REG 0x10
#define GYRO_REG 0x11

#define GYRO_X_LOW 0x22
#define GYRO_Y_LOW 0x24
#define GYRO_Z_LOW 0x26

#define ACCEL_X_LOW 0x28
#define ACCEL_Y_LOW 0x2A
#define ACCEL_Z_LOW 0x2C

class LSM6DSOX : public IMUBase {
  public:

    LSM6DSOX();

    void Update(double dt);

    Utils::Vector3 getAccelerations() {return _accelerations;}
    Utils::Vector3 getGyroRates() {return _gyroRates;}

  private:

    int setAccConfig(int config_num);
    int setGyroConfig(int config_num);

    int _imuFd;
    
    int _acc_lsb_to_g;
    int _gyro_lsb_to_degsec;

    Utils::Vector3 _accelerations;
    Utils::Vector3 _gyroRates;

};
