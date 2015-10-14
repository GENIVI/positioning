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

    for(i = 0; i < WHEEL_NUM_ELEMENTS; i++)
    {
        gWheelticks.elements[i].wheeltickCounter = 0;
        gWheelticks.elements[i].wheeltickIdentifier = WHEEL_INVALID;
    }

    cbWheelticks = 0;

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
    //printf("snsWheeltickRegisterCallback\n");
    if(cbWheelticks != 0) 
    {
        return false;
    }

    pthread_mutex_lock(&mutexCb);
    cbWheelticks = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsWheeltickDeregisterCallback(WheeltickCallback callback)
{
    //printf("snsWheeltickDeregisterCallback\n");
    if(cbWheelticks == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbWheelticks = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }

    return false;
}
