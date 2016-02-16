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


/** ===================================================================
 * 1.) INCLUDES
 */

 //provided interface
#include "i2ccomm.h"

//linux i2c access
#ifndef I2C_NOT_AVAILABLE
//RPi: located in /usr/include/linux/i2c-dev.h - all functions inline
#include <linux/i2c-dev.h>
//Unfortunately, there are two files named "i2c-dev.h" out there 
//Only the one provided by packages libi2c-dev or i2c-tools
//provides all necessary definitions
//  http://lxr.free-electrons.com/source/Documentation/i2c/dev-interface?v=3.7
//  http://raspberrypi.stackexchange.com/questions/37689/how-do-i-use-the-i2c-bus
//The original header file is from the standard package linux-glibc-devel
//which comes along with gcc and does neither define I2C_M_RD
//nor provide the i2c_smbus_ functions
//However I2C_M_RD is defined in /usr/include/linux/i2c.h,
//also from linux-glibc-devel
//
//Some more details on this mess:
//The i2c-dev.h comes originally from lm-sensors.org, the home of i2c-tools
//Since a while they have fixed the problem that i2c-dev.h is overwritten
//See https://web.archive.org/web/20150319191005/http://lm-sensors.org/wiki/I2CTools_4_Plan
//See also http://www.lm-sensors.org/changeset/6054/
//where the i2c_smbus_ function are moved to a new header file /us/include/i2c/smbus.h
//Unfortunately
//a) http://www.lm-sensors.org is down since late 2015
//parts orf the content are still available at archive.org
//e.g. https://web.archive.org/web/20150614020413/http://www.lm-sensors.org/browser/i2c-tools/trunk/include/linux/i2c-dev.h)
//b) the fix has not been backported by many distributions
//So the current situation is that
//i) Debian/Raspbian Jessie still have the buggy solution where i2c-dev.h is overwritten
//See https://packages.debian.org/jessie/libi2c-dev
//Same is to be expected for Ubuntu
//See http://packages.ubuntu.com/search?keywords=libi2c-dev
//Same is for openSUSE 13.2
//See https://build.opensuse.org/package/show?project=openSUSE%3A13.2&package=i2c-tools
//ii) on openSUSE 42.1, i2c-dev.h is no longer overwritten, instead smbus.h is provided
//See https://build.opensuse.org/package/show?project=openSUSE%3ALeap%3A42.1&package=i2c-tools
//See https://build.opensuse.org/request/show/339965
//See http://lists.opensuse.org/archive/opensuse-commit/2015-10/msg00918.html
//Same is for Arch Linux
//See https://aur.archlinux.org/packages/i2c-tools-svn/
//
//The final solution could be to get I2C_M_RD from /usr/include/linux/i2c.h
//and avoid using the i2c_smbus_ functions
//
//For now we have to test explicitly and set #define I2C_NOT_AVAILABLE
//if the wrong header is available
#ifndef I2C_M_RD
//seems that the wrong i2c-dev.h version has been included
#define I2C_NOT_AVAILABLE
#warning "i2c-dev.h wrong version - disabling I2C functionality during runtime"
#warning "Please install correct version, typically available in packages libi2c-dev or i2c-tools"
#endif
#endif

//standard c library functions
#include <unistd.h>
#include <fcntl.h>


bool i2ccomm::write_uint8(uint8_t reg, uint8_t data)
{
    bool result = false;
#ifndef I2C_NOT_AVAILABLE
    __s32 i2c_result;

    if (_i2c_fd < 0)
    {
        /* Invalid file descriptor */
    }
    else
    {
        i2c_result = i2c_smbus_write_byte_data(_i2c_fd, reg, data);
        if (i2c_result < 0)
        {
        /* ERROR HANDLING: i2c transaction failed */
        }
        else
        {
        result = true;
        }
    }
#endif
    return result;
}

bool i2ccomm::read_uint8(uint8_t reg, uint8_t* data)
{
    bool result = false;
#ifndef I2C_NOT_AVAILABLE
    __s32 i2c_result;

    if (_i2c_fd < 0)
    {
        /* Invalid file descriptor */
    }
    else
    {
        /* Using SMBus commands */
        i2c_result = i2c_smbus_read_byte_data(_i2c_fd, reg);
        if (i2c_result < 0)
        {
            /* ERROR HANDLING: i2c transaction failed */
        }
        else
        {
            *data = (uint8_t) i2c_result;
            //printf("Register 0x%02X: %08X = %d\n", reg, i2c_result, *data);
            result = true;
        }
    }
#endif
    return result;
}

/** Read a block of 8 bit unsigned integers from two consecutive registers
 *  Variant using 1 single ioctl() call instead of 1 read() followed by 1 write()
 *  See https://www.kernel.org/doc/Documentation/i2c/dev-interface
 *  on ioctl(file, I2C_RDWR, struct i2c_rdwr_ioctl_data *msgset).
 *  See also
 *    [i2c_rdwr_ioctl_data] (http://lxr.free-electrons.com/source/include/uapi/linux/i2c-dev.h#L64)
 *    [i2c_msg] (http://lxr.free-electrons.com/source/include/uapi/linux/i2c.h#L68)
 * Seems to be marginally faster than  read() followed by 1 write(): Ca 1% when reading 8 bytes
 */
bool i2ccomm::read_block(uint8_t reg, uint8_t* data, uint8_t size)
{
    bool result = false;
#ifndef I2C_NOT_AVAILABLE
    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg msg[2];
    int i2c_result;

    if (_i2c_fd < 0)
    {
        /* Invalid file descriptor */
    }
    else
    {
        i2c_data.msgs = msg;
        i2c_data.nmsgs = 2;     // two i2c_msg

        i2c_data.msgs[0].addr = _i2c_addr;
        i2c_data.msgs[0].flags = 0;         // write
        i2c_data.msgs[0].len = 1;           // only one byte
        i2c_data.msgs[0].buf = (char*)&reg; // typecast to char*: see i2c-dev.h

        i2c_data.msgs[1].addr = _i2c_addr;
        i2c_data.msgs[1].flags = I2C_M_RD;  // read command
        i2c_data.msgs[1].len = size;
        i2c_data.msgs[1].buf = (char*)data; // typecast to char*: see i2c-dev.h

        i2c_result = ioctl(_i2c_fd, I2C_RDWR, &i2c_data);

        if (i2c_result < 0)
        {
            /* ERROR HANDLING: i2c transaction failed */
        }
        else
        {
            result = true;
        }
    }
#endif
    return result;
}


bool i2ccomm::init(const char* i2c_device, uint8_t i2c_addr)
{
    bool result = false;
#ifndef I2C_NOT_AVAILABLE
    _i2c_fd = open(i2c_device, O_RDWR);
    if (_i2c_fd < 0)
    {
        /* ERROR HANDLING; you can check errno to see what went wrong */
    }
    else
    {
        if (ioctl(_i2c_fd, I2C_SLAVE, i2c_addr) < 0)
        {
            /* ERROR HANDLING; you can check errno to see what went wrong */
        }
        else
        {
            _i2c_addr = i2c_addr;
            result = true;
        }
    }
#endif
    return result;
}

bool i2ccomm::deinit()
{
    bool result = false;
#ifndef I2C_NOT_AVAILABLE
    if (_i2c_fd < 0)
    {
        /* Invalid file descriptor */
    }
    else
    {
        close(_i2c_fd);
        _i2c_fd = -1;
        _i2c_addr = 0;
        result = true;
    }
#endif
    return result;
}

