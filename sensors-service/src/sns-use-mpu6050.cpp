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

DLT_DECLARE_CONTEXT(gContext);

//configuration parameters - may be set from outside

//configure how to address the mpu6050
#ifndef MPU6050_I2C_DEV
#define MPU6050_I2C_DEV MPU6050_I2C_DEV_1
#endif

//sample interval (ms)
#ifndef MPU6050_SAMPLE_INTERVAL
#define MPU6050_SAMPLE_INTERVAL 10
#endif
//number of samples per callback
#ifndef MPU6050_NUM_SAMPLES
#define MPU6050_NUM_SAMPLES 10
#endif
//control whether samples are averaged for the callback
#ifndef MPU6050_AVG_SAMPLES
#define MPU6050_AVG_SAMPLES true
#endif

static volatile bool is_initialized = false;

static void mpu6050_cb(const TMPU6050Vector3D acceleration[], const TMPU6050Vector3D gyro_angular_rate[], const float temperature[], const uint64_t timestamp[], const uint16_t num_elements)
{
    TAccelerationData accel[MPU6050_NUM_SAMPLES] = {0};
    TGyroscopeData gyro[MPU6050_NUM_SAMPLES] = {0};
    
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
        if (is_ok)
        {
            //DLPF cut-off 42Hz fits best to 100Hz sample rate
            is_ok = mpu6050_init(MPU6050_I2C_DEV, MPU6050_ADDR_1, MPU6050_DLPF_42HZ);
        }
        if (is_ok)
        {    
            is_ok = mpu6050_register_callback(&mpu6050_cb);
        }        
        if (is_ok)
        {
            is_ok = mpu6050_start_reader_thread(MPU6050_SAMPLE_INTERVAL, MPU6050_NUM_SAMPLES, MPU6050_AVG_SAMPLES);
        }
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
        bool is_ok = mpu6050_stop_reader_thread();
        is_ok = is_ok && mpu6050_deregister_callback(&mpu6050_cb);
        is_ok = is_ok && mpu6050_deinit();
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

bool snsWheeltickInit()
{
    return iWheeltickInit();
}

bool snsWheeltickDestroy()
{
    return iWheeltickDestroy();
}
