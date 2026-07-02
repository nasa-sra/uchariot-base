#pragma once

#include "subsystems/VoltageMeter.h"

extern "C" {
#include </usr/include/i2c/smbus.h>
#include <linux/i2c-dev.h>
}
class INA228 : public VoltageMeterBase {
   public:
    INA228();
    void Update(double dt) override;


    
    int ReadRegister16(uint8_t lsb_register_add);

    //TODO: Implement ReadRegister20 for INA228 (I have no clue if this actually works as intended)
    int ReadRegister20(uint8_t msb_register_add);

    int writeRegister(uint8_t register_addr, uint8_t value);



    private: 
        int _ina228Fd;
        int _adapter_nr{1};

};