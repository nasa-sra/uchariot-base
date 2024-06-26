#pragma once

#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <cmath>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>

class I2C {

public:

    I2C(int _addr, int _adapter_nr);

    int readRegister(uint8_t register_add);
    int writeRegister(uint8_t register_addr, uint8_t value);

private:

    int _file;
    int _addr;
};