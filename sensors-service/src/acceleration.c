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
#include "acceleration.h"
#include "sns-meta-data.h"

static pthread_mutex_t mutexCb  = PTHREAD_MUTEX_INITIALIZER;   //protects the callbacks
static pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;  //protects the data

static volatile AccelerationCallback cbAcceleration = 0;
static TAccelerationData gAccelerationData = {0};
TAccelerationConfiguration gAccelerationConfiguration;

static TSensorStatus gStatus = {0};
static volatile SensorStatusCallback cbStatus = 0;

bool iAccelerationInit()
{
    pthread_mutex_lock(&mutexCb);
    cbAcceleration = 0;
    pthread_mutex_unlock(&mutexCb);
    
    pthread_mutex_lock(&mutexData);
    gAccelerationData.validityBits = 0;
    //example accelerometer configuration for a 3-axis accelerometer
    gAccelerationConfiguration.dist2RefPointX = 0;
    gAccelerationConfiguration.dist2RefPointY = 0;
    gAccelerationConfiguration.dist2RefPointZ = 0;
    gAccelerationConfiguration.angleYaw = 0;
    gAccelerationConfiguration.anglePitch = 0;
    gAccelerationConfiguration.angleRoll = 0;
    gAccelerationConfiguration.sigmaX = 0;
    gAccelerationConfiguration.sigmaY = 0;
    gAccelerationConfiguration.sigmaZ = 0;
    gAccelerationConfiguration.typeBits = 
        ACCELERATION_X_PROVIDED |
        ACCELERATION_Y_PROVIDED |
        ACCELERATION_Z_PROVIDED;
    gAccelerationConfiguration.validityBits = 
      ACCELERATION_CONFIG_ANGLEYAW_VALID | 
      ACCELERATION_CONFIG_ANGLEPITCH_VALID |
      ACCELERATION_CONFIG_ANGLEROLL_VALID |
      ACCELERATION_CONFIG_TYPE_VALID;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool iAccelerationDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbAcceleration = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsAccelerationGetAccelerationData(TAccelerationData * accelerationData)
{
    bool retval = false;
    if(accelerationData)
    {
        pthread_mutex_lock(&mutexData);
        *accelerationData = gAccelerationData;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}

bool snsAccelerationRegisterCallback(AccelerationCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbAcceleration)
    {
        cbAcceleration = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsAccelerationDeregisterCallback(AccelerationCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbAcceleration == callback) && callback)
    {
        cbAcceleration = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsAccelerationGetMetaData(TSensorMetaData *data)
{
    bool retval = false;    
    
    if(data) 
    {
        pthread_mutex_lock(&mutexData);
        *data = gSensorsMetaData[3];
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }

    return retval;
}

bool snsAccelerationGetAccelerationConfiguration(TAccelerationConfiguration* config)
{
    bool retval = false; 
    if(config) 
    {
        pthread_mutex_lock(&mutexData);
        *config = gAccelerationConfiguration;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }

    return retval;
}

void updateAccelerationData(const TAccelerationData accelerationData[], uint16_t numElements)
{
    if (accelerationData != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gAccelerationData = accelerationData[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbAcceleration)
        {
            cbAcceleration(accelerationData, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}

bool snsAccelerationGetStatus(TSensorStatus* status){
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

bool snsAccelerationRegisterStatusCallback(SensorStatusCallback callback)
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

bool snsAccelerationDeregisterStatusCallback(SensorStatusCallback callback)
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

void updateAccelerationStatus(const TSensorStatus* status)
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