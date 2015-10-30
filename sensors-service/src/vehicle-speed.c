/**************************************************************************
* Component Name: SensorsService
* Author: Marco Residori <marco.residori@xse.de>
*
* Copyright (C) 2013, XS Embedded GmbH
* 
* License:
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
**************************************************************************/

#include "globals.h"
#include "vehicle-speed.h"
#include "sns-meta-data.h"

static pthread_mutex_t mutexCb  = PTHREAD_MUTEX_INITIALIZER;   //protects the callbacks
static pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;  //protects the data

static volatile VehicleSpeedCallback cbVehicleSpeed = 0;
static TVehicleSpeedData gVehicleSpeedData = {0};

bool iVehicleSpeedInit()
{
    pthread_mutex_lock(&mutexCb);
    cbVehicleSpeed = 0;
    pthread_mutex_unlock(&mutexCb);

    pthread_mutex_lock(&mutexData);
    gVehicleSpeedData.validityBits = 0;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool iVehicleSpeedDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbVehicleSpeed = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsVehicleSpeedGetVehicleSpeedData(TVehicleSpeedData* vehicleSpeed)
{
    bool retval = false;
    if(vehicleSpeed)
    {
        pthread_mutex_lock(&mutexData);
        *vehicleSpeed = gVehicleSpeedData;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}

bool snsVehicleSpeedRegisterCallback(VehicleSpeedCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbVehicleSpeed)
    {
        cbVehicleSpeed = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsVehicleSpeedDeregisterCallback(VehicleSpeedCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbVehicleSpeed == callback) && callback)
    {
        cbVehicleSpeed = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsVehicleSpeedGetMetaData(TSensorMetaData *data)
{
    bool retval = false;    
    
    if(data) 
    {
        pthread_mutex_lock(&mutexData);
        *data = gSensorsMetaData[2];
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }

    return retval;
}

void updateVehicleSpeedData(const TVehicleSpeedData vehicleSpeedData[], uint16_t numElements)
{
    if (vehicleSpeedData != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gVehicleSpeedData = vehicleSpeedData[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbVehicleSpeed)
        {
            cbVehicleSpeed(vehicleSpeedData, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}
