/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup SensorsService
* \author Helmut Schmidt <https://github.com/huirad>
*
* \copyright Copyright (C) 2015, Helmut Schmidt
*
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

//APIs provided
#include "sns-init.h"
#include "acceleration.h"
#include "gyroscope.h"

//standard headers
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>

//sns internals
#include "globals.h"
#include "log.h"
#include "mpu6050.h"
#include "lsm9ds1.h"

DLT_DECLARE_CONTEXT(gContext);

//configuration parameters - may be set from outside

//configure how to address the IMU
#ifndef IMU_I2C_DEV
#define IMU_I2C_DEV MPU6050_I2C_DEV_1
#endif


//sample interval (ms)
#ifndef IMU_SAMPLE_INTERVAL
#define IMU_SAMPLE_INTERVAL 10
#endif
//number of samples per callback
#ifndef IMU_NUM_SAMPLES
#define IMU_NUM_SAMPLES 10
#endif
//control whether samples are averaged for the callback
#ifndef IMU_AVG_SAMPLES
#define IMU_AVG_SAMPLES true
#endif

static volatile bool is_initialized = false;

static void mpu6050_cb(const TMPU6050Vector3D acceleration[], const TMPU6050Vector3D gyro_angular_rate[], const float temperature[], const uint64_t timestamp[], const uint16_t num_elements)
{
    TAccelerationData accel[IMU_NUM_SAMPLES] = {0};
    TGyroscopeData gyro[IMU_NUM_SAMPLES] = {0};

    for (uint16_t i=0; i<num_elements; i++)
    {
        accel[i].timestamp = timestamp[i];
        accel[i].x = acceleration[i].x*MPU6050_UNIT_1_G;
        accel[i].y = acceleration[i].y*MPU6050_UNIT_1_G;
        accel[i].z = acceleration[i].z*MPU6050_UNIT_1_G;
        accel[i].temperature = temperature[i];
        accel[i].validityBits = ACCELERATION_X_VALID | ACCELERATION_Y_VALID |
                                ACCELERATION_Z_VALID | ACCELERATION_TEMPERATURE_VALID;

        gyro[i].timestamp = timestamp[i];
        gyro[i].yawRate = gyro_angular_rate[i].z;
        gyro[i].pitchRate = gyro_angular_rate[i].y;
        gyro[i].rollRate = gyro_angular_rate[i].x;
        gyro[i].temperature = temperature[i];
        gyro[i].validityBits = GYROSCOPE_YAWRATE_VALID | GYROSCOPE_PITCHRATE_VALID |
                               GYROSCOPE_ROLLRATE_VALID | GYROSCOPE_TEMPERATURE_VALID;

    }
    updateAccelerationData(accel, num_elements);
    updateGyroscopeData(gyro, num_elements);

}

static void lsm9ds1_cb(const TLSM9DS1Vector3D acceleration[], const TLSM9DS1Vector3D gyro_angular_rate[], const float temperature[], const uint64_t timestamp[], const uint16_t num_elements)
{
    TAccelerationData accel[IMU_NUM_SAMPLES] = {0};
    TGyroscopeData gyro[IMU_NUM_SAMPLES] = {0};

    for (uint16_t i=0; i<num_elements; i++)
    {
        accel[i].timestamp = timestamp[i];
        accel[i].x = acceleration[i].x*LSM9DS1_UNIT_1_G;
        accel[i].y = acceleration[i].y*LSM9DS1_UNIT_1_G;
        accel[i].z = acceleration[i].z*LSM9DS1_UNIT_1_G;
        accel[i].temperature = temperature[i];
        accel[i].validityBits = ACCELERATION_X_VALID | ACCELERATION_Y_VALID |
                                ACCELERATION_Z_VALID | ACCELERATION_TEMPERATURE_VALID;

        gyro[i].timestamp = timestamp[i];
        gyro[i].yawRate = gyro_angular_rate[i].z;
        gyro[i].pitchRate = gyro_angular_rate[i].y;
        gyro[i].rollRate = gyro_angular_rate[i].x;
        gyro[i].temperature = temperature[i];
        gyro[i].validityBits = GYROSCOPE_YAWRATE_VALID | GYROSCOPE_PITCHRATE_VALID |
                               GYROSCOPE_ROLLRATE_VALID | GYROSCOPE_TEMPERATURE_VALID;

    }
    updateAccelerationData(accel, num_elements);
    updateGyroscopeData(gyro, num_elements);

}

static bool snsGyroscopeInit_MPU6050()
{
    //DLPF cut-off 42Hz fits best to 100Hz sample rate
    bool is_ok = mpu6050_init(IMU_I2C_DEV, MPU6050_ADDR_1, MPU6050_DLPF_42HZ);
    is_ok = is_ok && mpu6050_register_callback(&mpu6050_cb);
    is_ok = is_ok && mpu6050_start_reader_thread(IMU_SAMPLE_INTERVAL, IMU_NUM_SAMPLES, IMU_AVG_SAMPLES);
    return is_ok;
}

static bool snsGyroscopeDestroy_MPU6050()
{
    bool is_ok = mpu6050_stop_reader_thread();
    is_ok = is_ok && mpu6050_deregister_callback(&mpu6050_cb);
    is_ok = is_ok && mpu6050_deinit();
    return is_ok;
}


static bool snsGyroscopeInit_LSM9DS1()
{
    //ODR 119Hz with LPF1 cut-off 38Hz fits best to 100Hz sample rate
    bool is_ok = lsm9ds1_init(IMU_I2C_DEV, LSM9DS1_ADDR_1, LSM9DS1_ODR_119HZ);
    is_ok = is_ok && lsm9ds1_register_callback(&lsm9ds1_cb);
    is_ok = is_ok && lsm9ds1_start_reader_thread(IMU_SAMPLE_INTERVAL, IMU_NUM_SAMPLES, IMU_AVG_SAMPLES);
    return is_ok;
}

static bool snsGyroscopeDestroy_LSM9DS1()
{
    bool is_ok = lsm9ds1_stop_reader_thread();
    is_ok = is_ok && lsm9ds1_deregister_callback(&lsm9ds1_cb);
    is_ok = is_ok && lsm9ds1_deinit();
    return is_ok;
}

bool snsInit()
{
    //nothing special to do
    return true;
}

bool snsDestroy()
{
    return true;
}

void snsGetVersion(int *major, int *minor, int *micro)
{
    if(major)
    {
        *major = GENIVI_SNS_API_MAJOR;
    }

    if(minor)
    {
        *minor = GENIVI_SNS_API_MINOR;
    }

    if(micro)
    {
        *micro = GENIVI_SNS_API_MICRO;
    }
}

bool snsGyroscopeInit()
{
    bool is_ok = false;
    if (is_initialized)
    {
        is_ok = true;
    }
    else
    {
        is_ok = iGyroscopeInit();
#if defined(IMU_TYPE_MPU6050)
        is_ok = is_ok && snsGyroscopeInit_MPU6050();
#elif defined(IMU_TYPE_LSM9DS1)
        is_ok = is_ok && snsGyroscopeInit_LSM9DS1();
#else
        is_ok = false;
#endif
        is_initialized = is_ok;
    }
    return is_ok;
}

bool snsGyroscopeDestroy()
{
    bool is_ok = false;
    if (!is_initialized)
    {
        is_ok = true;
    }
    else
    {
        is_initialized = false;
#if defined(IMU_TYPE_MPU6050)
        is_ok = is_ok && snsGyroscopeDestroy_MPU6050();
#elif defined(IMU_TYPE_LSM9DS1)
        is_ok = is_ok && snsGyroscopeDestroy_LSM9DS1();
#endif
        is_ok = is_ok && iGyroscopeDestroy();
    }
    return is_ok;
}

bool snsAccelerationInit()
{
    //gyro is the master for mpu6050 - nothing further to initialize
    return snsGyroscopeInit();
}

bool snsAccelerationDestroy()
{
    //gyro is the master for mpu6050 - nothing further to deeinit
    return snsGyroscopeDestroy();
}

//+ some dummy implementations
bool snsVehicleSpeedInit()
{
    return iVehicleSpeedInit();
}

bool snsVehicleSpeedDestroy()
{
    return iVehicleSpeedDestroy();
}

bool snsWheelInit()
{
    return iWheelInit();
}

bool snsWheelDestroy()
{
    return iWheelDestroy();
}
