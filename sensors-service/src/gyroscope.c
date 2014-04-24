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
    cbGyroscope = 0;

    return true;
}

bool snsGyroscopeDestroy()
{
    cbGyroscope = 0;

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
    //printf("snsGyroscopeRegisterCallback\n");
    if(cbGyroscope != 0) 
    {
        return false;
    }

    pthread_mutex_lock(&mutexCb);
    cbGyroscope = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool snsGyroscopeDeregisterCallback(GyroscopeCallback callback)
{
    //printf("snsGyroscopeDeregisterCallback\n");
    if(cbGyroscope == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbGyroscope = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }

    return false;
}

bool snsGyroscopeGetMetaData(TSensorMetaData *data)
{
    if(data != 0) 
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
    return true;
}




