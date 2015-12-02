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
#include "wheel.h"

static pthread_mutex_t mutexCb  = PTHREAD_MUTEX_INITIALIZER;   //protects the callbacks
static pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;  //protects the data

static volatile WheelCallback cbWheel = 0;
static TWheelData gWheelData = {0};

static TSensorStatus gStatus = {0};
static volatile SensorStatusCallback cbStatus = 0;

bool iWheelInit()
{
    int i;

    pthread_mutex_lock(&mutexCb);
    cbWheel = 0;
    pthread_mutex_unlock(&mutexCb);

    pthread_mutex_lock(&mutexData);
    gWheelData.validityBits = 0;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool iWheelDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbWheel = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsWheelGetWheelData(TWheelData *wheelData)
{
    bool retval = false;
    if(wheelData)
    {
        pthread_mutex_lock(&mutexData);
        *wheelData = gWheelData;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}

bool snsWheelRegisterCallback(WheelCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbWheel)
    {
        cbWheel = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsWheelDeregisterCallback(WheelCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbWheel == callback) && callback)
    {
        cbWheel = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

void updateWheelData(const TWheelData wheelData[], uint16_t numElements)
{
    if (wheelData != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gWheelData = wheelData[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbWheel)
        {
            cbWheel(wheelData, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}

bool snsWheelGetStatus(TSensorStatus* status){
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

bool snsWheelRegisterStatusCallback(SensorStatusCallback callback)
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

bool snsWheelDeregisterStatusCallback(SensorStatusCallback callback)
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

void updateWheelStatus(const TSensorStatus* status)
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