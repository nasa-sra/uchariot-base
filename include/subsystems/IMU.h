#pragma once

#include "I2C.h"

class IMU {
  public:

    IMU();

    int setAccConfig(int config_num);
    int setGyroConfig(int config_num);
    float getAccel(int index);
    float* getAccelRaw();
    float getGyro(int index);
    float* getGyroRaw();
    void fetchData();

  private:

    I2C accelGyro{0x6A, 20};
    I2C magnetometer{0x1C, 20};
    
    const uint8_t _accel_register = 0x10;
    const uint8_t _gyro_register = 0x11;

    int _acc_lsb_to_g;
    int _gyro_lsb_to_degsec;

    float accel[3];
    float gyro[3];
};