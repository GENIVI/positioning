/**************************************************************************
 * @brief Access library for MPU6050/MPU9150 inertial sensors
 *
 * @details Encapsulate I2C access to a MPU6050 sensor on a Linux machine
 * The MPU6050 from Invense is a 6DOF inertial sensor
 * @see http://www.invensense.com/mems/gyro/mpu6050.html
 * The functions work also with the MPU9150 which is a MPU6050 with
 * additional functionality (magnetometer)
 * @see http://www.invensense.com/mems/gyro/mpu9150.html
 *
 * @author Helmut Schmidt <https://github.com/huirad>
 * @copyright Copyright (C) 2015, Helmut Schmidt
 *
 * @license MPL-2.0 <http://spdx.org/licenses/MPL-2.0>
 *
 **************************************************************************/


/** ===================================================================
 * 1.) INCLUDES
 */

 //provided interface
#include "mpu6050.h"

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
 * 2.) MPU6050 magic numbers
 * Source: http://invensense.com/mems/gyro/documents/RM-MPU-6000A-00v4.2.pdf
 */

/** MPU6050 register addresses
 *  Accelerometer, temperature, and gyro readings are each 16bit signed integers
 *  stored in two consecutive registeres as 2's complement value
 *  The registers MPU6050_REG_ACCEL_XOUT ... MPU6050_REG_GYRO_ZOUT
 *  each contain the high byte of the 16 bit. The low by is in the next register
 *  Favourably, the accelerometer, temperature, and gyro registers
 *  are clustered in a fashion that is optimized for block reads
 */
#define MPU6050_REG_CONFIG     0x1A
#define MPU6050_REG_ACCEL_XOUT 0x3B
#define MPU6050_REG_ACCEL_YOUT 0x3D
#define MPU6050_REG_ACCEL_ZOUT 0x3F
#define MPU6050_REG_TEMP_OUT   0x41
#define MPU6050_REG_GYRO_XOUT  0x43
#define MPU6050_REG_GYRO_YOUT  0x45
#define MPU6050_REG_GYRO_ZOUT  0x47
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_WHO_AM_I   0x75

 /** MPU6050 register values
  */
#define MPU6050_PWR_MGMT_1__SLEEP  0x40
#define MPU6050_PWR_MGMT_1__WAKEUP 0x00
#define MPU6050_WHO_AM_I           0x68

 /** MPU6050 conversion factors
  * Accelerometer scale at default +-2g range: 16384 LSB/g
  * Temperature in degrees C = (TEMP_OUT Register Value as a signed quantity)/340 + 36.53
  * Gyroscope scale at default +-250 deg/s range: 131 LSB/(deg/s)
  */
#define MPU6050_ACCEL_SCALE  16384.0
#define MPU6050_TEMP_SCALE   340.0
#define MPU6050_TEMP_BIAS    36.53
#define MPU6050_GYRO_SCALE   131.0


/** ===================================================================
 * 3.) PRIVATE VARIABLES AND FUNCTIONS
 * Functions starting with conv_ convert the raw data to common measurement units
 */

/** MPU6050 reader thread control
 */
static volatile int _mpu6050_reader_loop = 0;
static pthread_t _reader_thread;
static uint64_t _sample_interval;
static uint16_t _num_samples;
static bool _average;

/** Callback function and associated mutex
 */
static pthread_mutex_t _mutex_cb  = PTHREAD_MUTEX_INITIALIZER;
static volatile MPU6050Callback _cb = 0;

static i2ccomm _i2ccomm;

static bool mpu6050_wakeup()
{
    uint8_t whoami;
    bool result = true;
    //Wake up the MPU6050 as it starts in sleep mode
    result = _i2ccomm.write_uint8(MPU6050_REG_PWR_MGMT_1, MPU6050_PWR_MGMT_1__WAKEUP);
    //Test the WHO_AM_I register
    if (result)
    {
        result = _i2ccomm.read_uint8(MPU6050_REG_WHO_AM_I, &whoami);
        result = result && (MPU6050_WHO_AM_I == whoami) ;
    }
    //wait 10ms to guarantee that sensor data is available at next read attempt
    usleep(10000);
    return result;
}

static bool mpu6050_setDLPF(EMPU6050LowPassFilterBandwidth bandwidth)
{
    bool result = true;
    result = _i2ccomm.write_uint8(MPU6050_REG_CONFIG, bandwidth);
    return result;
}


static float conv_accel(int16_t raw_accel)
{
    return raw_accel / MPU6050_ACCEL_SCALE;
}

static float conv_temp(int16_t raw_temp)
{
    return raw_temp / MPU6050_TEMP_SCALE + MPU6050_TEMP_BIAS;
}

static float conv_gyro(int16_t raw_gyro)
{
    return raw_gyro / MPU6050_GYRO_SCALE;
}

static uint64_t sleep_until(uint64_t wakeup)
{
    uint64_t start =  mpu6050_get_timestamp();

    if (wakeup > start)
    {
        uint64_t diff = wakeup - start;
        struct timespec t;
        t.tv_sec = diff / 1000;
        t.tv_nsec = (diff - t.tv_sec*1000) * 1000000;
        while(nanosleep(&t, &t));
    }

    uint64_t stop =  mpu6050_get_timestamp();
    return stop-start;
}

static bool fire_callback(const TMPU6050Vector3D acceleration[], const TMPU6050Vector3D gyro_angular_rate[], const float temperature[], const uint64_t timestamp[], const uint16_t num_elements, bool average)
{
    pthread_mutex_lock(&_mutex_cb);
    if (_cb)
    {
        if (average)
        {
            TMPU6050Vector3D av_acceleration = acceleration[0];
            TMPU6050Vector3D av_gyro_angular_rate = gyro_angular_rate[0];
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
 * Worker thread to read MPU6050 data
 * @param param pointer to parameters (currently unused)
 */
static void* mpu6050_reader_thread(void* param)
{
    TMPU6050Vector3D acceleration[_num_samples];
    TMPU6050Vector3D gyro_angular_rate[_num_samples];
    float temperature[_num_samples];
    uint64_t timestamp[_num_samples];

    uint16_t sample_idx = 0;

    uint64_t next =  mpu6050_get_timestamp();
    uint64_t next_cb = next;

    while (_mpu6050_reader_loop)
    {
        if (mpu6050_read_accel_gyro(&acceleration[sample_idx], &gyro_angular_rate[sample_idx], &temperature[sample_idx], &timestamp[sample_idx]))
        {
            sample_idx++;
        }
        else
        {
            sample_idx = 0; // ???
            //TODO fire error callback!!!!! TODO
        }


        //fire callback when either the requested number of samples has been acquired or the corresponding time is over
        if ((sample_idx == _num_samples) || (mpu6050_get_timestamp() > next_cb))
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
 * 4.) FUNCTIONS IMPLEMENTING THE PUBLIC INTERFACE OF mpu6050.h
 */

bool mpu6050_init(const char* i2c_device, uint8_t i2c_addr, EMPU6050LowPassFilterBandwidth bandwidth)
{
    bool result = false;
    result = _i2ccomm.init(i2c_device, i2c_addr);
    if (result)
    {
        result = mpu6050_setDLPF(bandwidth);
    }
    if (result)
    {
        result = mpu6050_wakeup();
    }
    return result;
}

bool mpu6050_deinit()
{
    bool result = false;
    result = _i2ccomm.deinit();
    return result;
}


bool mpu6050_read_accel_gyro(TMPU6050Vector3D* acceleration, TMPU6050Vector3D* gyro_angular_rate, float* temperature, uint64_t* timestamp)
{
    bool result = true;
    int16_t value;
    struct timespec time_value;
    uint8_t block[14];

    //always read temperature
    uint8_t start_reg = MPU6050_REG_TEMP_OUT;
    uint16_t num_bytes = 2;
    uint8_t start = 6;

    //read acceleration?
    if (acceleration)
    {
        start_reg = MPU6050_REG_ACCEL_XOUT;
        num_bytes +=6;
        start = 0;
    }
    //read gyro_angular_rate?
    if (gyro_angular_rate)
    {
        num_bytes +=6;
    }

    if (timestamp != NULL)
    {
        *timestamp = mpu6050_get_timestamp();
    }

    if (_i2ccomm.read_block(start_reg, block+start, num_bytes))
    {
        if (acceleration != NULL)
        {
            value = (((int16_t)block[0]) << 8) | block[1];
            acceleration->x = conv_accel(value);
            value = (((int16_t)block[2]) << 8) | block[3];
            acceleration->y = conv_accel(value);
            value = (((int16_t)block[4]) << 8) | block[5];
            acceleration->z = conv_accel(value);
        }
        if (temperature != NULL)
        {
            value = (((int16_t)block[6]) << 8) | block[7];
            *temperature = conv_temp(value);
        }
        if (gyro_angular_rate != NULL)
        {
            value = (((int16_t)block[8]) << 8) | block[9];
            gyro_angular_rate->x = conv_gyro(value);
            value = (((int16_t)block[10]) << 8) | block[11];
            gyro_angular_rate->y = conv_gyro(value);
            value = (((int16_t)block[12]) << 8) | block[13];
            gyro_angular_rate->z = conv_gyro(value);
        }
    }
    else
    {
        result = false;
    }
    return result;
}

bool mpu6050_register_callback(MPU6050Callback callback)
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

bool mpu6050_deregister_callback(MPU6050Callback callback)
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

bool mpu6050_start_reader_thread(uint64_t sample_interval, uint16_t num_samples, bool average)
{
    if (_mpu6050_reader_loop)
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

    _mpu6050_reader_loop = 1;

    int res = pthread_create (&_reader_thread, NULL, mpu6050_reader_thread, NULL);

    if (res != 0)
    {
        _mpu6050_reader_loop = 0;
        return false;
    }

    return true;
}

bool mpu6050_stop_reader_thread()
{
    _mpu6050_reader_loop = 0;
    pthread_join (_reader_thread, NULL);
    return true;
}

uint64_t mpu6050_get_timestamp()
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

