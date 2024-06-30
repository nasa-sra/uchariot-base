#pragma once

#include "subsystems/SubsystemBase.h"
#include "Utils.h"

#ifndef SIMULATION
#include <wiringPiI2C.h>

#define LSM6DSOX_ADDR 0x6A
#define ACCEL_REG 0x10
#define GYRO_REG 0x11

#define GYRO_X_LOW 0x22
#define GYRO_Y_LOW 0x24
#define GYRO_Z_LOW 0x26

#define ACCEL_X_LOW 0x28
#define ACCEL_Y_LOW 0x2A
#define ACCEL_Z_LOW 0x2C

class IMU : SubsystemBase {

  public:

    IMU();

    void Update(double dt) override;
    void ReportState(std::string prefix = "/") override;

    Utils::Vector3 getAccelerations();
    Utils::Vector3 getGyroRates();

    float getYaw();

  private:

    int setAccConfig(int config_num);
    int setGyroConfig(int config_num);

    int _imuFd;
    
    int _acc_lsb_to_g;
    int _gyro_lsb_to_degsec;

    Utils::Vector3 _accelerations;
    Utils::Vector3 _gyroRates;

    Utils::Vector3 _gyroAngles;
};
#else
class IMU : SubsystemBase {
  public:

    #warning "SMILUATION!!!!"

    IMU() {}

    void Update(double dt) override {}
    void ReportState(std::string prefix = "/") override {}

    Utils::Vector3 getAccelerations() {return Utils::Vector3(0.0, 0.0, 0.0);}
    Utils::Vector3 getGyroRates() {return Utils::Vector3(0.0, 0.0, 0.0);}

    float getYaw() {return -1.0;}
};

#endif