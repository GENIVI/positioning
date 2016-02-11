/**************************************************************************
 * @brief Access library for LSM9DS1 inertial sensor
 *
 * @details Encapsulate I2C access to a LSM9DS1 sensor on a Linux machine
 * The LSM9DS1 from ST Microelectronics is a 6DOF inertial sensor
 * @see http://www.st.com/web/en/catalog/sense_power/FM89/SC1448/PF259998
 * Derived from mpu6050.cpp by cloning and then adapting.
 * Duplication of several helper functions is purpose to obtain
 * self contained code with minimum dependencies.
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
#include "lsm9ds1.h"

//linux i2c access
#include "i2ccomm.h"

//standard c library functions
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <pthread.h>



/** ===================================================================
 * 2.) LSM9DS1 magic numbers
 * Source: http://invensense.com/mems/gyro/documents/RM-MPU-6000A-00v4.2.pdf
 */

/** LSM9DS1 register addresses
 *  Accelerometer, temperature, and gyro readings are each 16bit signed integers
 *  stored in two consecutive registeres as 2's complement value
 *  The byte order can be selected by CTRL_REG8
 *  By default, the registers LSM9DS1_REG_OUT_TEMP ... LSM9DS1_REG_ACCEL_ZOUT
 *  each contain the low byte of the 16 bit. The high byte is in the next register
 *  --> byte order is different from MPU6050.
 *  Favourably, the accelerometer, and gyro registers
 *  are clustered in a fashion that is optimized for block reads - see datasheet section 3.3.
 *  The temperature register must be read separately.
 */

#define LSM9DS1_REG_OUT_TEMP        0x15    //int16_t - 12bit resolution
#define LSM9DS1_REG_OUT_X_G         0x18    //int16_t
#define LSM9DS1_REG_OUT_Y_G         0x1A    //int16_t
#define LSM9DS1_REG_OUT_Z_G         0x1C    //int16_t
#define LSM9DS1_REG_OUT_X_XL        0x28    //int16_t
#define LSM9DS1_REG_OUT_Y_XL        0x2A    //int16_t
#define LSM9DS1_REG_OUT_Z_XL        0x2C    //int16_t

#define LSM9DS1_REG_CTRL_REG1_G     0x10
#define LSM9DS1_REG_ORIENT_CFG_G    0x13
#define LSM9DS1_REG_CTRL_REG8       0x22
#define LSM9DS1_REG_WHO_AM_I        0x0F     //MPU6050 used different register 0x75

 /** LSM9DS1 register values
  */
#define LSM9DS1_CTRL_REG8__INIT     0xC    //BOOT|BDU|IF_ADD_INC
#define LSM9DS1_WHO_AM_I            0x68    //MPU6050 used same value 0x68

 /** LSM9DS1 conversion factors
  * Accelerometer scale at default +-2g range: 16384 LSB/g
  *    data sheet: 0.061 mg/LSB
  * Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/16
  *    data sheet: 16 LSB/°C, Output is 0 at 25°C
  *    own calibration measurement : Temperature 2.5°C too low
  *    ==> correct temperature bias to 27.5
  *    See also https://github.com/kriswiner/LSM9DS1/issues/3
  * Gyroscope scale at default +-245 deg/s range: 114 LSB/(deg/s)
  *    data sheet: 8.75 mdps/LSB
  */
#define LSM9DS1_ACCEL_SCALE  16384.0
#define LSM9DS1_TEMP_SCALE   16.0
#define LSM9DS1_TEMP_BIAS    27.5
#define LSM9DS1_GYRO_SCALE   114.3


/** ===================================================================
 * 3.) PRIVATE VARIABLES AND FUNCTIONS
 * Functions starting with conv_ convert the raw data to common measurement units
 */

/** LSM9DS1 reader thread control
 */
static volatile int _lsm9ds1_reader_loop = 0;
static pthread_t _reader_thread;
static uint64_t _sample_interval;
static uint16_t _num_samples;
static bool _average;

/** Callback function and associated mutex
 */
static pthread_mutex_t _mutex_cb  = PTHREAD_MUTEX_INITIALIZER;
static volatile LSM9DS1Callback _cb = 0;

static i2ccomm _i2ccomm;

static bool lsm9ds1_config()
{
    uint8_t whoami;
    bool result = true;
    //Reset the LSM9DS1
    result = _i2ccomm.write_uint8(LSM9DS1_REG_CTRL_REG8, LSM9DS1_CTRL_REG8__INIT);
    //wait 100ms to guarantee that sensor has rebooted at next read attempt
    usleep(100000);
    //Test the WHO_AM_I register
    if (result)
    {
        result = _i2ccomm.read_uint8(LSM9DS1_REG_WHO_AM_I, &whoami);
        result = result && (LSM9DS1_WHO_AM_I == whoami) ;
    }
    return result;
}

static bool lsm9ds1_setODR(ELSM9DS1OutputDataRate odr)
{
    bool result = true;
    result = _i2ccomm.write_uint8(LSM9DS1_REG_CTRL_REG1_G, odr);
    //wait 10ms to guarantee that sensor data is available at next read attempt
    usleep(10000);
    return result;
}


static float conv_accel(int16_t raw_accel)
{
    return raw_accel / LSM9DS1_ACCEL_SCALE;
}

static float conv_temp(int16_t raw_temp)
{
    return raw_temp / LSM9DS1_TEMP_SCALE + LSM9DS1_TEMP_BIAS;
}

static float conv_gyro(int16_t raw_gyro)
{
    return raw_gyro / LSM9DS1_GYRO_SCALE;
}

static uint64_t sleep_until(uint64_t wakeup)
{
    uint64_t start =  lsm9ds1_get_timestamp();

    if (wakeup > start)
    {
        uint64_t diff = wakeup - start;
        struct timespec t;
        t.tv_sec = diff / 1000;
        t.tv_nsec = (diff - t.tv_sec*1000) * 1000000;
        while(nanosleep(&t, &t));
    }

    uint64_t stop =  lsm9ds1_get_timestamp();
    return stop-start;
}

static bool fire_callback(const TLSM9DS1Vector3D acceleration[], const TLSM9DS1Vector3D gyro_angular_rate[], const float temperature[], const uint64_t timestamp[], const uint16_t num_elements, bool average)
{
    pthread_mutex_lock(&_mutex_cb);
    if (_cb)
    {
        if (average)
        {
            TLSM9DS1Vector3D av_acceleration = acceleration[0];
            TLSM9DS1Vector3D av_gyro_angular_rate = gyro_angular_rate[0];
            float av_temperature = temperature[0];
            for (uint16_t i=1; i<num_elements; i++)
            {
                av_acceleration.x += acceleration[i].x;
                av_acceleration.y += acceleration[i].y;
                av_acceleration.z += acceleration[i].z;
                av_gyro_angular_rate.x += gyro_angular_rate[i].x;
                av_gyro_angular_rate.y += gyro_angular_rate[i].y;
                av_gyro_angular_rate.z += gyro_angular_rate[i].z;
                av_temperature += temperature[i];
            }
            av_acceleration.x /= num_elements;
            av_acceleration.y /= num_elements;
            av_acceleration.z /= num_elements;
            av_gyro_angular_rate.x /= num_elements;
            av_gyro_angular_rate.y /= num_elements;
            av_gyro_angular_rate.z /= num_elements;
            av_temperature /= num_elements;
            uint64_t last_timestamp = timestamp[num_elements-1];
            _cb(&av_acceleration, &av_gyro_angular_rate, &av_temperature, &last_timestamp, 1);
        }
        else
        {
            _cb(acceleration, gyro_angular_rate, temperature, timestamp, num_elements);
        }
    }
    pthread_mutex_unlock(&_mutex_cb);
}

/**
 * Worker thread to read LSM9DS1 data
 * @param param pointer to parameters (currently unused)
 */
static void* lsm9ds1_reader_thread(void* param)
{
    TLSM9DS1Vector3D acceleration[_num_samples];
    TLSM9DS1Vector3D gyro_angular_rate[_num_samples];
    float temperature[_num_samples];
    uint64_t timestamp[_num_samples];

    uint16_t sample_idx = 0;

    uint64_t next =  lsm9ds1_get_timestamp();
    uint64_t next_cb = next;

    while (_lsm9ds1_reader_loop)
    {
        if (lsm9ds1_read_accel_gyro(&acceleration[sample_idx], &gyro_angular_rate[sample_idx], &temperature[sample_idx], &timestamp[sample_idx]))
        {
            sample_idx++;
        }
        else
        {
            sample_idx = 0; // ???
            //TODO fire error callback!!!!! TODO
        }


        //fire callback when either the requested number of samples has been acquired or the corresponding time is over
        if ((sample_idx == _num_samples) || (lsm9ds1_get_timestamp() > next_cb))
        {
            fire_callback(acceleration, gyro_angular_rate, temperature, timestamp, sample_idx, _average);
            sample_idx = 0;
            next_cb += _sample_interval*_num_samples;
        }
        //wait until next sampling timeslot
        next = next + _sample_interval;
        sleep_until(next);
    }
}




/** ===================================================================
 * 4.) FUNCTIONS IMPLEMENTING THE PUBLIC INTERFACE OF lsm9ds1.h
 */

bool lsm9ds1_init(const char* i2c_device, uint8_t i2c_addr, ELSM9DS1OutputDataRate odr)
{
    bool result = false;
    result = _i2ccomm.init(i2c_device, i2c_addr);
    if (result)
    {
        result = lsm9ds1_config();
    }
    if (result)
    {
        result = lsm9ds1_setODR(odr); //also trigges wake up from power down
    }
    return result;
}

bool lsm9ds1_deinit()
{
    bool result = false;
    result = _i2ccomm.deinit();
    return result;
}


bool lsm9ds1_read_accel_gyro(TLSM9DS1Vector3D* acceleration, TLSM9DS1Vector3D* gyro_angular_rate, float* temperature, uint64_t* timestamp)
{
    bool result = true;
    int16_t value;
    struct timespec time_value;
    uint8_t block[14]; //6 bytes: gyro, 6 bytes: accel, 2 bytes: temperature

    //Although gyro and accelerometer registers are not consecutive,
    //they can apparently be read in a single block
    //See datasheet, section 3.3
    //"When both accelerometer and gyroscope sensors are activated at the same ODR, starting
    //from OUT_X_G (18h - 19h) multiple reads can be performed. Once OUT_Z_XL (2Ch - 2Dh)
    //is read, the system automatically restarts from OUT_X_G (18h - 19h) (see Figure 7)."

    uint8_t start_reg = 0;
    uint16_t num_bytes = 0;
    uint8_t start = 6;

    //read acceleration?
    if (acceleration)
    {
        start_reg = LSM9DS1_REG_OUT_X_XL;
        num_bytes +=6;
    }
    //read gyro_angular_rate?
    if (gyro_angular_rate)
    {
        start_reg = LSM9DS1_REG_OUT_X_G;
        num_bytes +=6;
        start = 0;
    }

    if (timestamp != NULL)
    {
        *timestamp = lsm9ds1_get_timestamp();
    }

    if (temperature)
    {
        if (_i2ccomm.read_block(LSM9DS1_REG_OUT_TEMP, block+12, 2))
        {
            value = (((int16_t)block[13]) << 8) | block[12];
            *temperature = conv_temp(value);
        }
        else
        {
            result = false;
        }
    }

    if (start_reg && _i2ccomm.read_block(start_reg, block+start, num_bytes))
    {
        /* GENIVI specifies, that x,y,z axes form a right-handed coordinate system.
         * The LSM9DS1 x,y,z axes form a left-handed coordinate system.
         * Therefore the y-axis values are inverted
         */
        if (acceleration)
        {
            value = (((int16_t)block[7]) << 8) | block[6];
            acceleration->x = conv_accel(value);
            value = (((int16_t)block[9]) << 8) | block[8];
            acceleration->y = -conv_accel(value);
            value = (((int16_t)block[11]) << 8) | block[10];
            acceleration->z = conv_accel(value);
        }
        if (gyro_angular_rate)
        {
            value = (((int16_t)block[1]) << 8) | block[0];
            gyro_angular_rate->x = conv_gyro(value);
            value = (((int16_t)block[3]) << 8) | block[2];
            gyro_angular_rate->y = -conv_gyro(value);
            value = (((int16_t)block[5]) << 8) | block[4];
            gyro_angular_rate->z = conv_gyro(value);
        }
    }
    else
    {
        result = false;
    }
    return result;
}

bool lsm9ds1_register_callback(LSM9DS1Callback callback)
{
    if(_cb != 0)
    {
        return false; //if already registered
    }

    pthread_mutex_lock(&_mutex_cb);
    _cb = callback;
    pthread_mutex_unlock(&_mutex_cb);

    return true;
}

bool lsm9ds1_deregister_callback(LSM9DS1Callback callback)
{
    if(_cb == callback && _cb != 0)
    {
        return false; //if already registered
    }

    pthread_mutex_lock(&_mutex_cb);
    _cb = 0;
    pthread_mutex_unlock(&_mutex_cb);

    return true;
}

bool lsm9ds1_start_reader_thread(uint64_t sample_interval, uint16_t num_samples, bool average)
{
    if (_lsm9ds1_reader_loop)
    {
        return false; //thread already running
    }
    if (sample_interval == 0)
    {
        return false;
    }
    if (num_samples == 0)
    {
        return false;
    }

    _sample_interval = sample_interval;
    _num_samples = num_samples;
    _average = average;

    _lsm9ds1_reader_loop = 1;

    int res = pthread_create (&_reader_thread, NULL, lsm9ds1_reader_thread, NULL);

    if (res != 0)
    {
        _lsm9ds1_reader_loop = 0;
        return false;
    }

    return true;
}

bool lsm9ds1_stop_reader_thread()
{
    _lsm9ds1_reader_loop = 0;
    pthread_join (_reader_thread, NULL);
    return true;
}

uint64_t lsm9ds1_get_timestamp()
{
  struct timespec time_value;
  if (clock_gettime(CLOCK_MONOTONIC, &time_value) != -1)
  {
    return (time_value.tv_sec*1000 + time_value.tv_nsec/1000000);
  }
  else
  {
    return 0xFFFFFFFFFFFFFFFF;
  }
}

