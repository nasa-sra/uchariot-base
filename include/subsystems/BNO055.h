// Used for this board https://www.adafruit.com/product/4646

#pragma once

#include "subsystems/IMUBase.h"
#include "Utils.h"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <cmath>
extern "C" {
    #include <linux/i2c-dev.h>
    #include </usr/include/i2c/smbus.h>
}

class BNO055 : public IMUBase {
  public:

    BNO055();

    void Update(double dt) override;
    int readRegister(uint8_t register_add);
    int readDataRegister(uint8_t lsb_register_add);
    int writeRegister(uint8_t register_addr, uint8_t value);

  private:
    
    int _imuFd;
    int _adapter_nr {1};
};
