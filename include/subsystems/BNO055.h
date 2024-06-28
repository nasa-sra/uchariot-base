// Used for this board https://www.adafruit.com/product/4646

#pragma once

#include "subsystems/IMUBase.h"
#include "Utils.h"

class BNO055 : public IMUBase {
  public:

    BNO055();

    void Update(double dt) override;

  private:

    int _imuFd;
    
};
