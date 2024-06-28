// Used for this board https://www.adafruit.com/product/4517

#pragma once

#include "subsystems/IMUBase.h"
#include "Utils.h"

class LSM6DSOX : public IMUBase {
  public:

    LSM6DSOX();

    void Update(double dt) override;

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
