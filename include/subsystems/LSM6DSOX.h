// Used for this board https://www.adafruit.com/product/4517

#pragma once

#include "subsystems/IMUBase.h"
#include "Utils.h"

class LSM6DSOX : public IMUBase {
  public:

    LSM6DSOX();

    void Update(double dt) override;

    Eigen::Vector3f getAccelerations() {return _accelerations;}
    Eigen::Vector3f getGyroRates() {return _gyroRates;}

  private:

    int setAccConfig(int config_num);
    int setGyroConfig(int config_num);

    int _imuFd;
    
    int _acc_lsb_to_g;
    int _gyro_lsb_to_degsec;

    Eigen::Vector3f _accelerations;
    Eigen::Vector3f _gyroRates;

};
