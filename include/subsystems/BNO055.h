// Used for this board https://www.adafruit.com/product/4646

#pragma once

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cmath>
#include <iostream>

#include "Utils.h"
#include "subsystems/IMUBase.h"
extern "C" {
#include </usr/include/i2c/smbus.h>
#include <linux/i2c-dev.h>
}

class BNO055 : public IMUBase {
   public:
    BNO055();

    void Update(double dt) override;
    int ReadRegister8(uint8_t register_add);
    int ReadRegister16(uint8_t lsb_register_add);
    int writeRegister(uint8_t register_addr, uint8_t value);

   private:
    int _imuFd;
    int _adapter_nr{1};
};
