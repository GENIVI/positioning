/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup SensorsService
* \author Marco Residori <marco.residori@xse.de>
*
* \copyright Copyright (C) 2013, XS Embedded GmbH
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#include "globals.h"
#include "gyroscope.h"
#include "sns-meta-data.h"

static pthread_mutex_t mutexCb  = PTHREAD_MUTEX_INITIALIZER;   //protects the callbacks
static pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;  //protects the data

static volatile GyroscopeCallback cbGyroscope = 0;
static TGyroscopeData gGyroscopeData = {0};
TGyroscopeConfiguration gGyroscopeConfiguration;

static TSensorStatus gStatus = {0};
static volatile SensorStatusCallback cbStatus = 0;


bool iGyroscopeInit()
{
    pthread_mutex_lock(&mutexCb);
    cbGyroscope = 0;
    pthread_mutex_unlock(&mutexCb);

    pthread_mutex_lock(&mutexData);
    gGyroscopeData.validityBits = 0;
    //example gyroscope configuration for a 3-axis gyro
    gGyroscopeConfiguration.angleYaw = 0;
    gGyroscopeConfiguration.anglePitch = 0;
    gGyroscopeConfiguration.angleRoll = 0;
    gGyroscopeConfiguration.momentOfYawInertia = 0;
    gGyroscopeConfiguration.sigmaGyroscope = 0;
    gGyroscopeConfiguration.typeBits = 
        GYROSCOPE_YAWRATE_PROVIDED |
        GYROSCOPE_PITCHRATE_PROVIDED |
        GYROSCOPE_ROLLRATE_PROVIDED;
    gGyroscopeConfiguration.validityBits = 
      GYROSCOPE_CONFIG_ANGLEYAW_VALID | 
      GYROSCOPE_CONFIG_ANGLEPITCH_VALID |
      GYROSCOPE_CONFIG_ANGLEROLL_VALID |
      GYROSCOPE_CONFIG_TYPE_VALID;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool iGyroscopeDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbGyroscope = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsGyroscopeGetGyroscopeData(TGyroscopeData * gyroData)
{
    bool retval = false;
    if(gyroData)
    {
        pthread_mutex_lock(&mutexData);
        *gyroData = gGyroscopeData;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}

bool snsGyroscopeRegisterCallback(GyroscopeCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbGyroscope)
    {
        cbGyroscope = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsGyroscopeDeregisterCallback(GyroscopeCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbGyroscope == callback) && callback)
    {
        cbGyroscope = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsGyroscopeGetMetaData(TSensorMetaData *data)
{
    bool retval = false;    
    
    if(data) 
    {
        pthread_mutex_lock(&mutexData);
        *data = gSensorsMetaData[1];
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }

    return retval;
}

bool snsGyroscopeGetConfiguration(TGyroscopeConfiguration* gyroConfig)
{
    bool retval = false; 
    if(gyroConfig) 
    {
        pthread_mutex_lock(&mutexData);
        *gyroConfig = gGyroscopeConfiguration;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }

    return retval;
}

void updateGyroscopeData(const TGyroscopeData gyroData[], uint16_t numElements)
{
    if (gyroData != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gGyroscopeData = gyroData[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbGyroscope)
        {
            cbGyroscope(gyroData, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}

bool snsGyroscopeGetStatus(TSensorStatus* status){
    bool retval = false;
    if(status)
    {
        pthread_mutex_lock(&mutexData);
        *status = gStatus;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}

bool snsGyroscopeRegisterStatusCallback(SensorStatusCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbStatus)
    {
        cbStatus = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsGyroscopeDeregisterStatusCallback(SensorStatusCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbStatus == callback) && callback)
    {
        cbStatus = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

void updateGyroscopeStatus(const TSensorStatus* status)
{
    if (status)
    {
        pthread_mutex_lock(&mutexData);
        gStatus = *status;
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbStatus)
        {
            cbStatus(status);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}