
#include "subsystems/BNO055.h"
#include "StateReporter.h"

#define BNO055_ADDR 0x28

#define OPR_MODE_REG 0x3D
#define CALIB_STATUS_REG 0x35
#define STATUS_REG 0x39
#define ERR_REG 0x3A

#define EUL_X_LOW_REG 0x1E
#define EUL_Y_LOW_REG 0x1C
#define EUL_Z_LOW_REG 0x1A

BNO055::BNO055() : IMUBase() {
    char filename[20];

    snprintf(filename, 19, "/dev/i2c-%d", _adapter_nr);
    _imuFd = open(filename, O_RDWR);
    if (_imuFd < 0) { Utils::ErrFmt("Failed to open communication for I2C address %d", BNO055_ADDR); }

    if (ioctl(_imuFd, I2C_SLAVE, BNO055_ADDR) < 0) {
        Utils::ErrFmt("Failed to configure the parameters for I2C address %d", BNO055_ADDR);
    }
    uint8_t status = ReadRegister8(STATUS_REG);

    writeRegister(OPR_MODE_REG, 0b00001100); // Set IMU mode
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    uint8_t calib_status = ReadRegister8(CALIB_STATUS_REG);
    Utils::LogFmt("Connected to BNO055 IMU Status %i, Calibration %i", status, (calib_status & 0b11000000) >> 6);
    if (status == 1) {
        uint8_t err = ReadRegister8(ERR_REG);
        Utils::LogFmt("BNO055 Error Code %i", err);
    }
}

#define DEG2RAD (M_PI / 180.0) // Degrees to radians conversion factor

void BNO055::Update(double dt) {
    int16_t x, y, z;
    x = ReadRegister16(EUL_X_LOW_REG);
    y = ReadRegister16(EUL_Y_LOW_REG);
    z = ReadRegister16(EUL_Z_LOW_REG);

    _gyroAngles.x() = (((float)x) / 16.0) * DEG2RAD;
    _gyroAngles.y() = (((float)y) / 16.0) * DEG2RAD;
    _gyroAngles.z() = (((float)z) / 16.0) * DEG2RAD;
}

/**
 * @brief Reads a byte from a specified register of the BNO055 IMU.
 *
 * This function uses the Linux I2C interface to read a byte from a specified register of the BNO055 IMU.
 *
 * @param register_add The address of the register to read from.
 *
 * @return The result of the read operation.
 * - On success, the function returns the byte value read from the register.
 * - On failure, the function returns -1 and logs an error message.
 */
int BNO055::ReadRegister8(uint8_t register_add) {
    int32_t res;

    res = i2c_smbus_read_word_data(_imuFd, register_add);
    if (res < 0) {
        Utils::ErrFmt("Read from I2C address %d failed", BNO055_ADDR);
        return -1;
    } else {
        return res;
    }
}

/**
 * @brief Reads a 16-bit value from the BNO055 IMU by reading two consecutive 8-bit registers.
 *
 * This function reads a 16-bit value from the BNO055 IMU by reading two consecutive 8-bit registers.
 * The function first reads the byte from the register specified by the least significant byte (LSB) address,
 * then reads the byte from the next register (which is the most significant byte (MSB) address).
 * The function combines the two bytes into a 16-bit value using bitwise shifting and OR operation.
 *
 * @param lsb_register_add The address of the least significant byte (LSB) register to read from.
 *
 * @return The result of the read operation.
 * - On success, the function returns a 16-bit value combining the LSB and MSB bytes.
 * - On failure, the function returns -1 and logs an error message.
 */
int BNO055::ReadRegister16(uint8_t lsb_register_add) {
    return ReadRegister8(lsb_register_add + 0x01) << 8 | ReadRegister8(lsb_register_add);
}

/**
 * @brief Writes a byte to a specified register of the BNO055 IMU.
 *
 * This function uses the Linux I2C interface to write a byte to a specified register of the BNO055 IMU.
 *
 * @param register_addr The address of the register to write to.
 * @param value The byte value to write to the register.
 *
 * @return The result of the write operation.
 * - 0: The write operation was successful.
 * - -1: The write operation failed.
 */
int BNO055::writeRegister(uint8_t register_addr, uint8_t value) {
    int res = i2c_smbus_write_word_data(_imuFd, register_addr, value);
    if (res < 0) {
        Utils::ErrFmt("Write to I2C address %d failed", BNO055_ADDR);
        return -1;
    }

    return res;
}
