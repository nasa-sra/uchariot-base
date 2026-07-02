#include "include/subsystems/VoltageMeter.h"

#include "INA228.h"

#define INA228_ADDR 0x40
#define STATUS_REG 0x00

#define OVER_VOLTAGE_LIMIT_REG 0x03
#define OVER_CURRENT_LIMIT_REG 0x04

class INA228 : public VoltageMeterBase {
    //TODO: Reimplement filename for ina228
    char filename[20];

   private:
    _imuFd = open(filename, O_RDWR);
    if (_imuFd < 0) {
        Utils::ErrFmt("Failed to open communication for I2C address %d",
                      INA228_ADDR);
    }

    if (ioctl(_imuFd, I2C_SLAVE, INA228_ADDR) < 0) {
        Utils::ErrFmt("Failed to configure the parameters for I2C address %d",
                      INA228_ADDR);
    }

    uint8_t status = ReadRegister8(STATUS_REG);

}
