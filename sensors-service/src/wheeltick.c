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

WheeltickCallback cbWheelticks = 0;
TWheelticks gWheelticks;

bool snsWheeltickInit()
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

bool snsWheeltickDestroy()
{
    pthread_mutex_lock(&mutexCb);
    cbWheelticks = 0;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsWheeltickGetWheelticks(TWheelticks * ticks)
{
    if(!ticks)
    {
        return false;
    }

    pthread_mutex_lock(&mutexData);
    *ticks = gWheelticks;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool snsWheeltickRegisterCallback(WheeltickCallback callback)
{
    if(!callback)
    {
        return false;
    }

    //printf("snsWheeltickRegisterCallback\n");
    pthread_mutex_lock(&mutexCb);
    if(cbWheelticks != 0) 
    {
        //already registered
        pthread_mutex_unlock(&mutexCb);
        return false; 
    }
    cbWheelticks = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsWheeltickDeregisterCallback(WheeltickCallback callback)
{
    if(!callback)
    {
        return false;
    }

    //printf("snsWheeltickDeregisterCallback\n");
    pthread_mutex_lock(&mutexCb);
    if(cbWheelticks == callback)
    { 
        cbWheelticks = 0;      
    }
    pthread_mutex_unlock(&mutexCb);  

    return true;
}
