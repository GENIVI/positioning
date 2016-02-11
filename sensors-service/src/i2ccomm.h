/**************************************************************************
 * @brief Access library for I2C
 *
 * @details Encapsulate Linux I2C access via the user space device driver
 * @see https://www.kernel.org/doc/Documentation/i2c/dev-interface
 * @see https://www.kernel.org/doc/Documentation/i2c/smbus-protocol
 *
 * @author Helmut Schmidt <https://github.com/huirad>
 * @copyright Copyright (C) 2016, Helmut Schmidt
 *
 * @license MPL-2.0 <http://spdx.org/licenses/MPL-2.0>
 *
 **************************************************************************/

#ifndef INCLUDE_I2CCOMM
#define INCLUDE_I2CCOMM

#include <stdint.h>

/**
 * Minimalistic wrapper fo Linux I2C access.
 * Only the functions absolutely necessary to access
 * typical sensors are provided.
 * Behind the scenes, the Linux the user space device driver
 * is used.
 * If the #define I2C_NOT_AVAILABLE is set, all calls will
 * fail gracefully.
 */
class i2ccomm {

private:
    int _i2c_fd;
    uint8_t _i2c_addr;

public:
    /**
     * Constructor
     */
    i2ccomm(): _i2c_fd (-1), _i2c_addr(0) {};

    /**
     * Initialize a connection to an I2C slave
     * @param i2c_device device name of the I2C bus to which the I2C slave is attached, e.g. "/dev/i2c-0"
     * @param i2c_addr the I2C address (7bit) of the I2C slave
     * @return true on success
     */
    bool init(const char* i2c_device, uint8_t i2c_addr);

    /**
     * Close the I2C connection
     * @return true on success
     */
    bool deinit();

    /**
     * Write a 8 bit unsigned integer to a register
     * @param reg register address
     * @param data value to write to the register
     * @return true on success
     */
    bool write_uint8(uint8_t reg, uint8_t data);

    /**
     * Read a 8 bit unsigned integer from a register
     * @param reg register address
     * @param data returns value read from the register
     * @return true on success
     */
    bool read_uint8(uint8_t reg, uint8_t* data);

    /**
     * Read a block of 8 bit unsigned integers from several consecutive registers.
     * @param reg register start address
     * @param data returns values read from the registers, buffer must be at least size bytes large
     * @param size number of bytes to read
     * @return true on success
     */
    bool read_block(uint8_t reg, uint8_t* data, uint8_t size);
};

#endif //I2CCOMM