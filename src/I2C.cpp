#include "I2C.h"
#include "Utils.h"

I2C::I2C(int addr, int adapter_nr){
    char filename[20];
    _addr = addr;

    snprintf(filename, 19, "/dev/i2c-%d", adapter_nr);
    _file = open(filename, O_RDWR);
    if (_file < 0) {
        Utils::ErrFmt("Failed to open communication for I2C address %d", _addr);
    }

    if (ioctl(_file, I2C_SLAVE, _addr) < 0) {
        Utils::ErrFmt("Failed to configure the parameters for I2C address %d", _addr);
    }
}

int I2C::readRegister(uint8_t register_add){
    int32_t res;

    res = i2c_smbus_read_word_data(_file, register_add);
    if (res < 0) {
        Utils::ErrFmt("Read from I2C address %d failed", _addr);
    } else {
        return res;
    }
}

int I2C::writeRegister(uint8_t register_addr, uint8_t value) {
    if(i2c_smbus_write_word_data(_file, register_addr, value) < 0){
        Utils::ErrFmt("Write to I2C address %d failed", _addr);
    }
}