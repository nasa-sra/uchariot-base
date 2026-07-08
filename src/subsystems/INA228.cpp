#include "subsystems/INA228.h"

#include <cstdio>

namespace {
constexpr uint8_t INA228_ADDR = 0x40; // default address
}  // namespace

INA228::INA228() : _ina228Fd(-1) {}

bool INA228::InitializeINA228() {
    if (_ina228Fd >= 0) {
        return true;
    }

    char filename[20];
    std::snprintf(filename, sizeof(filename), "/dev/i2c-%d", _adapter_nr);
    _ina228Fd = open(filename, O_RDWR);
    if (_ina228Fd < 0) {
        Utils::ErrFmt("Failed to open communication for INA228 on %s",
                      filename);
        return false;
    }

    if (ioctl(_ina228Fd, I2C_SLAVE, INA228_ADDR) < 0) {
        Utils::ErrFmt("Failed to configure the parameters for INA228 at 0x%02X",
                      INA228_ADDR);
        close(_ina228Fd);
        _ina228Fd = -1;
        return false;
    }

    int status = ReadRegister16(Register::CONFIG);
    if (status < 0) {
        Utils::LogFmt("INA228 initialization did not read a valid status register");
        return false;
    }

    Utils::LogFmt("Connected to INA228 sensor");
    return true;
}

void INA228::Update(double dt) {
    (void)dt;

    if (_ina228Fd < 0) {
        return;
    }

    int voltage_raw = ReadRegister16(Register::VBUS);
    int current_raw = ReadRegister16(Register::CURRENT);
    int power_raw = ReadRegister16(Register::POWER);

    if (voltage_raw >= 0) {
        _voltage = static_cast<float>(voltage_raw) * 0.00125f;
    }
    if (current_raw >= 0) {
        _current = static_cast<float>(current_raw) * 0.0001f;
    }
    if (power_raw >= 0) {
        _power = static_cast<float>(power_raw) * 0.025f;
    }
}

float INA228::GetVoltage() { return _voltage; }

float INA228::GetCurrent() { return _current; }

float INA228::GetPower() { return _power; }

int INA228::ReadRegister8(uint8_t register_addr) {
    if (_ina228Fd < 0) {
        Utils::LogFmt("INA228 device is not initialized");
        return -1;
    }

    int res = i2c_smbus_read_byte_data(_ina228Fd, register_addr);
    if (res < 0) {
        Utils::LogFmt("INA228 read byte from register 0x%02X failed", register_addr);
        return -1;
    }

    return res;
}

int INA228::ReadRegister16(uint8_t register_addr) {
    if (_ina228Fd < 0) {
        Utils::LogFmt("INA228 device is not initialized");
        return -1;
    }

    int res = i2c_smbus_read_word_data(_ina228Fd, register_addr);
    if (res < 0) {
        Utils::LogFmt("INA228 read word from register 0x%02X failed", register_addr);
        return -1;
    }

    return res;
}

int INA228::writeRegister(uint8_t register_addr, uint8_t value) {
    if (_ina228Fd < 0) {
        Utils::LogFmt("INA228 device is not initialized");
        return -1;
    }

    int res = i2c_smbus_write_byte_data(_ina228Fd, register_addr, value);
    if (res < 0) {
        Utils::LogFmt("INA228 write to register 0x%02X failed", register_addr);
        return -1;
    }

    return res;
}

//TODO: Get actual scaling factors for values below
float INA228::GetBusVoltage() {
    int voltage_raw = ReadRegister16(Register::VBUS);
    if (voltage_raw < 0) {
        return 0.0f;
    }
    return static_cast<float>(voltage_raw) * 0.00125f; // Convert to volts 
}

// GetShuntVoltage()
float INA228::GetShuntVOltage() {
    int shunt_voltage_raw = ReadRegister16(Register::VSHUNT);
    if (shunt_voltage_raw < 0) {
        return 0.0f;
    }
    return static_cast<float>(shunt_voltage_raw) * 0.0001f; // Convert to volts
}

// GetPower()
float INA228::GetPower() {
    int power_raw = ReadRegister16(Register::POWER);
    if (power_raw < 0) {
        return 0.0f;
    }
    return static_cast<float>(power_raw) * 0.025f; // Convert to watts
}

float INA228::GetEnergy() {
    int energy_raw = ReadRegister16(Register::ENERGY);
    if (energy_raw < 0) {
        return 0.0f;
    }
    return static_cast<float>(energy_raw) * 0.0001f; // Convert to watt-hours
}

float INA228::GetCharge() {
    int charge_raw = ReadRegister16(Register::CHARGE);
    if (charge_raw < 0) {
        return 0.0f;
    }
    return static_cast<float>(charge_raw) * 0.0001f; // Convert to coulombs
}

// SetShuntCalibration()

// SetBusVoltageLimit()

// SetShuntVoltageLimit()








