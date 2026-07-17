#include "subsystems/INA228.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace {

constexpr uint8_t INA228_ADDR = 0x40;  // Default I2C address

constexpr uint16_t INA228_MANUFACTURER_ID = 0x5449;

inline int32_t SignExtend20(uint32_t raw) {
    raw >>= 4;  // Move the 20-bit value down to bits [19:0]

    if (raw & (1UL << 19)) {
        raw |= 0xFFF00000;
    }

    return static_cast<int32_t>(raw);
}

}  // namespace

INA228::~INA228() {
    if (_ina228Fd >= 0) {
        close(_ina228Fd);
        _ina228Fd = -1;
    }
}

INA228::INA228() : _ina228Fd(-1) { InitializeINA228(); }

bool INA228::InitializeINA228() {
    if (_ina228Fd >= 0) {
        return true;
    }

    char filename[20];
    std::snprintf(filename, sizeof(filename), "/dev/i2c-%d", _adapter_nr);

    _ina228Fd = open(filename, O_RDWR);

    if (_ina228Fd < 0) {
        Utils::ErrFmt("Failed to open INA228 on %s", filename);
        return false;
    }

    if (ioctl(_ina228Fd, I2C_SLAVE, INA228_ADDR) < 0) {
        Utils::ErrFmt("Failed to configure INA228 address 0x%02X",
                      INA228_ADDR);

        close(_ina228Fd);
        _ina228Fd = -1;
        return false;
    }

    uint16_t manufacturer = ReadRegister16(Register::MFG_UID);

    if (manufacturer != INA228_MANUFACTURER_ID) {
        Utils::ErrFmt(
            "Unexpected INA228 manufacturer ID: 0x%04X",
            manufacturer);

        close(_ina228Fd);
        _ina228Fd = -1;
        return false;
    }

    uint16_t device = ReadRegister16(Register::DEVICE_UID);

    Utils::LogFmt(
        "Detected INA228 (Device ID: 0x%04X)",
        device);

    if (!SetShuntCalibration(0.015f, 10.0f)) {
        Utils::ErrFmt("Failed to configure INA228 calibration");
        return false;
    }

    Utils::LogFmt("Connected to INA228");

    return true;
}

void INA228::Update(double dt) {
    (void)dt;

    if (_ina228Fd < 0) {
        return;
    }

    int32_t voltage_raw = ReadRegister24(Register::VBUS);
    int32_t current_raw = ReadRegister24(Register::CURRENT);
    int32_t power_raw = ReadRegister24(Register::POWER);
    int temperature_raw = ReadRegister16(Register::DIE_TEMP);

    if (voltage_raw >= 0) {
        // VBUS: 20-bit unsigned in bits[23:4], 195.3125 µV/LSB
        _voltage = static_cast<float>(voltage_raw >> 4) * 195.3125e-6f;
        static bool vbus_logged = false;
        if (!vbus_logged) {
            Utils::LogFmt("INA228 VBUS raw=0x%06X counts=%d voltage=%.4fV",
                          voltage_raw, voltage_raw >> 4, _voltage);
            vbus_logged = true;
        }
    }
    if (current_raw >= 0) {
        // CURRENT: 20-bit signed; sign-extend from bit 23, then divide by 16 per Adafruit ref
        if (current_raw & 0x800000) current_raw |= 0xFF000000;
        _current = static_cast<float>(current_raw) / 16.0f * _current_lsb;
        static bool current_logged = false;
        if (!current_logged) {
            Utils::LogFmt("INA228 CURRENT raw=0x%06X current=%.4fA (current_lsb=%.6f)",
                          current_raw & 0xFFFFFF, _current, _current_lsb);
            current_logged = true;
        }
    }
    if (power_raw >= 0) {
        // POWER: 24-bit unsigned; scale = 3.2 * _current_lsb per Adafruit ref
        _power = static_cast<float>(power_raw) * 3.2f * _current_lsb;
        static bool power_logged = false;
        if (!power_logged) {
            Utils::LogFmt("INA228 POWER raw=0x%06X power=%.4fW", power_raw, _power);
            power_logged = true;
        }
    }
    if (temperature_raw >= 0) {
        // DIE_TEMP: 16-bit signed, 7.8125 m°C/LSB
        _temperature = static_cast<float>((int16_t)temperature_raw) * 0.0078125f;
        static bool temp_logged = false;
        if (!temp_logged) {
            Utils::LogFmt("INA228 TEMP raw=0x%04X temperature=%.2fC", temperature_raw, _temperature);
            temp_logged = true;
        }
    }
}

float INA228::GetVoltage() { return _voltage; }

float INA228::GetCurrent() { return _current; }

float INA228::GetPower() { return _power; }

float INA228::GetTemperature() { return _temperature; }

bool INA228::ReadRegister(uint8_t reg, uint8_t* buffer, size_t length) {
    if (_ina228Fd < 0) {
        Utils::ErrFmt("INA228 device is not initialized");
        return false;
    }

    int bytesRead = i2c_smbus_read_i2c_block_data(
        _ina228Fd,
        reg,
        static_cast<uint8_t>(length),
        buffer);

    if (bytesRead != static_cast<int>(length)) {
        Utils::ErrFmt(
            "Failed reading %zu bytes from register 0x%02X",
            length,
            reg);
        return false;
    }

    return true;
}


uint8_t INA228::ReadRegister8(uint8_t reg) {
    uint8_t buffer[1];

    if (!ReadRegister(reg, buffer, sizeof(buffer))) {
        return 0xFF;
    }

    return buffer[0];
}

uint16_t INA228::ReadRegister16(uint8_t reg) {
    uint8_t buffer[2];

    if (!ReadRegister(reg, buffer, sizeof(buffer))) {
        return UINT16_MAX;
    }

    return (static_cast<uint16_t>(buffer[0]) << 8) |
           static_cast<uint16_t>(buffer[1]);
}

uint16_t INA228::ReadRegister16(uint8_t reg) {
    uint8_t buffer[2];

    if (!ReadRegister(reg, buffer, sizeof(buffer))) {
        return UINT16_MAX;
    }

    return (static_cast<uint16_t>(buffer[0]) << 8) |
           static_cast<uint16_t>(buffer[1]);
}

uint64_t INA228::ReadRegister40(uint8_t reg) {
    uint8_t buffer[5];

    if (!ReadRegister(reg, buffer, sizeof(buffer))) {
        return UINT64_MAX;
    }

    return (static_cast<uint64_t>(buffer[0]) << 32) |
           (static_cast<uint64_t>(buffer[1]) << 24) |
           (static_cast<uint64_t>(buffer[2]) << 16) |
           (static_cast<uint64_t>(buffer[3]) << 8) |
           static_cast<uint64_t>(buffer[4]);
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

int INA228::writeRegister16(uint8_t register_addr, uint16_t value) {
    if (_ina228Fd < 0) {
        Utils::LogFmt("INA228 device is not initialized");
        return -1;
    }

    // INA228 is big-endian; swap bytes before writing
    int res = i2c_smbus_write_word_data(_ina228Fd, register_addr,
                                        __builtin_bswap16(value));
    if (res < 0) {
        Utils::LogFmt("INA228 write word to register 0x%02X failed", register_addr);
    }

    return res;
}

bool INA228::SetShuntCalibration(float shunt_res, float max_current) {
    _shunt_res = shunt_res;
    _current_lsb = max_current / (float)(1UL << 19);
    // SHUNT_CAL = 13107.2 * 1e6 * shunt_res * current_lsb (ADCRANGE=0)
    uint16_t shunt_cal = (uint16_t)(13107.2f * 1000000.0f * _shunt_res * _current_lsb);
    return writeRegister16(Register::SHUNT_CAL, shunt_cal) >= 0;
}

float INA228::GetBusVoltage() {
    int32_t voltage_raw = ReadRegister24(Register::VBUS);
    if (voltage_raw < 0) {
        return 0.0f;
    }
    return static_cast<float>(voltage_raw >> 4) * 195.3125e-6f;
}

float INA228::GetShuntVoltage() {
    int32_t shunt_raw = ReadRegister24(Register::VSHUNT);
    if (shunt_raw < 0) {
        return 0.0f;
    }
    // VSHUNT: 20-bit signed; sign-extend from bit 23, 312.5 nV/LSB (ADCRANGE=0 default)
    if (shunt_raw & 0x800000) shunt_raw |= 0xFF000000;
    return static_cast<float>(shunt_raw) / 16.0f * 312.5e-9f;
}

float INA228::GetEnergy() {
    int energy_raw = ReadRegister16(Register::ENERGY);
    if (energy_raw < 0) {
        return 0.0f;
    }
    //TODO: Get actual scaling factors for values below
    return static_cast<float>(energy_raw) * 0.0001f; // Convert to watt-hours
}

float INA228::GetCharge() {
    int charge_raw = ReadRegister16(Register::CHARGE);
    if (charge_raw < 0) {
        return 0.0f;
    }
    //TODO: Get actual scaling factors for values below
    return static_cast<float>(charge_raw) * 0.0001f; // Convert to coulombs
}

// SetShuntCalibration()

// SetBusVoltageLimit()

// SetShuntVoltageLimit()








