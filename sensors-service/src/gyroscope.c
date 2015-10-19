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

GyroscopeCallback cbGyroscope = 0;
TGyroscopeData gGyroscopeData;
TGyroscopeConfiguration gGyroscopeConfiguration;

bool snsGyroscopeInit()
{
    pthread_mutex_lock(&mutexCb);
    cbGyroscope = 0;
    pthread_mutex_unlock(&mutexCb);

    pthread_mutex_lock(&mutexData);
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

bool snsGyroscopeDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbGyroscope = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsGyroscopeGetGyroscopeData(TGyroscopeData * gyroData)
{
    if(!gyroData)
    {
    	return false;
    }

    pthread_mutex_lock(&mutexData);
    *gyroData = gGyroscopeData;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool snsGyroscopeRegisterCallback(GyroscopeCallback callback)
{
    if(!callback)
    {
        return false;
    }

    //printf("snsGyroscopeRegisterCallback\n");
    pthread_mutex_lock(&mutexCb);
    if(cbGyroscope != 0) 
    {   
        //already registered
        pthread_mutex_unlock(&mutexCb);
        return false;
    }
    cbGyroscope = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsGyroscopeDeregisterCallback(GyroscopeCallback callback)
{
    if(!callback)
    {
        return false;
    }

    //printf("snsGyroscopeDeregisterCallback\n");
    pthread_mutex_lock(&mutexCb);
    if(cbGyroscope == callback)
    { 
        cbGyroscope = 0;
    }
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsGyroscopeGetMetaData(TSensorMetaData *data)
{
    if(!data) 
    {
        return false;
    }
    
    pthread_mutex_lock(&mutexData);
    *data = gSensorsMetaData[1];
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool snsGyroscopeGetConfiguration(TGyroscopeConfiguration* gyroConfig)
{
    if(!gyroConfig) 
    {
        return false;
    }

    pthread_mutex_lock(&mutexData);
    *gyroConfig = gGyroscopeConfiguration;
    pthread_mutex_unlock(&mutexData);

    return true;
}




