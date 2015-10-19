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

AccelerationCallback cbAcceleration = 0;
TAccelerationData gAccelerationData;
TAccelerationConfiguration gAccelerationConfiguration;

bool snsAccelerationInit()
{
    pthread_mutex_lock(&mutexCb);
    cbAcceleration = 0;
    pthread_mutex_unlock(&mutexCb);
    
    pthread_mutex_lock(&mutexData);
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

bool snsAccelerationDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbAcceleration = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsAccelerationGetAccelerationData(TAccelerationData * accelerationData)
{
    if(!accelerationData)
    {
    	return false;
    }

    pthread_mutex_lock(&mutexData);
    *accelerationData = gAccelerationData;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool snsAccelerationRegisterCallback(AccelerationCallback callback)
{
    if(!callback)
    {
        return false;
    }

    //printf("snsAccelerationRegisterCallback\n");
    pthread_mutex_lock(&mutexCb);
    if(cbAcceleration != 0) 
    {   
        //already registered
        pthread_mutex_unlock(&mutexCb);
        return false;
    }
    cbAcceleration = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsAccelerationDeregisterCallback(AccelerationCallback callback)
{
    if(!callback)
    {
        return false;
    }

    //printf("snsAccelerationDeregisterCallback\n");
    pthread_mutex_lock(&mutexCb);
    if(cbAcceleration == callback)
    {
        cbAcceleration = 0;
    }
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsAccelerationGetMetaData(TSensorMetaData *data)
{
    if(!data) 
    {
        return false;
    }
    
    pthread_mutex_lock(&mutexData);
    *data = gSensorsMetaData[3];
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool snsAccelerationGetAccelerationConfiguration(TAccelerationConfiguration* config)
{
    if(!config) 
    {
        return false;
    }

    pthread_mutex_lock(&mutexData);
    *config = gAccelerationConfiguration;
    pthread_mutex_unlock(&mutexData);

    return true;
}




