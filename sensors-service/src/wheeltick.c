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

static volatile WheeltickCallback cbWheelticks = 0;
static TWheelticks gWheelticks = {0};

bool iWheeltickInit()
{
    int i;

    pthread_mutex_lock(&mutexCb);
    cbWheelticks = 0;
    pthread_mutex_unlock(&mutexCb);

    pthread_mutex_lock(&mutexData);
    for(i = 0; i < WHEEL_NUM_ELEMENTS; i++)
    {
        gWheelticks.elements[i].wheeltickCounter = 0;
        gWheelticks.elements[i].wheeltickIdentifier = WHEEL_INVALID;
    }
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool iWheeltickDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbWheelticks = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsWheeltickGetWheelticks(TWheelticks * ticks)
{
    bool retval = false;
    if(ticks)
    {
        pthread_mutex_lock(&mutexData);
        *ticks = gWheelticks;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}

bool snsWheeltickRegisterCallback(WheeltickCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbWheelticks)
    {
        cbWheelticks = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool snsWheeltickDeregisterCallback(WheeltickCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbWheelticks == callback) && callback)
    {
        cbWheelticks = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

void updateWheelticks(const TWheelticks ticks[], uint16_t numElements)
{
    if (ticks != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gWheelticks = ticks[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbWheelticks)
        {
            cbWheelticks(ticks, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}
