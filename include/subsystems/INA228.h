#pragma once

#include <cstdint>

#include "Utils.h"
#include "subsystems/VoltageMeterBase.h"

extern "C" {
#include </usr/include/i2c/smbus.h>
#include <linux/i2c-dev.h>
}

namespace Register {
    constexpr uint8_t CONFIG = 0x00;
    constexpr uint8_t ADC_CONFIG = 0x01;
    constexpr uint8_t SHUNT_CAL = 0x02;
    constexpr uint8_t SHUNT_TEMP_CO = 0x03;

    constexpr uint8_t VSHUNT = 0x04;
    constexpr uint8_t VBUS = 0x05;
    constexpr uint8_t DIE_TEMP = 0x06;
    constexpr uint8_t CURRENT = 0x07;
    constexpr uint8_t POWER = 0x08;
    constexpr uint8_t ENERGY = 0x09;
    constexpr uint8_t CHARGE = 0x0A;

    constexpr uint8_t DIAG_ALRT = 0x0B;

    constexpr uint8_t SOVL = 0x0C;
    constexpr uint8_t SUVL = 0x0D;
    constexpr uint8_t BOVL = 0x0E;
    constexpr uint8_t BUVL = 0x0F;
    constexpr uint8_t TEMP_LIMIT = 0x10;
    constexpr uint8_t PWR_LIMIT = 0x11;

    constexpr uint8_t MFG_UID = 0x3E;
    constexpr uint8_t DEVICE_UID = 0x3F;
}  // namespace Register

class INA228 : public VoltageMeterBase {
   public:
    INA228();

    bool InitializeINA228();
    void Update(double dt) override;

    float GetVoltage() override;
    float GetCurrent() override;
    float GetPower() override;
    float GetTemperature() override;
    
    float GetBusVoltage();
    float GetShuntVOltage();
    float GetEnergy();
    float GetCharge();

   private:
    int _ina228Fd{-1};
    int _adapter_nr{1};

    int ReadRegister16(uint8_t register_addr);
    int ReadRegister8(uint8_t register_addr);
    int writeRegister(uint8_t register_addr, uint8_t value);
};