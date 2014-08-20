/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup GNSSService
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
#include "gnss-ext.h"

TGNSSSatelliteDetail gSatelliteDetail; //TODO: buffer full set of satellite details for one point in time
GNSSSatelliteDetailCallback cbSatelliteDetail = 0;

TGNSSPosition gPosition;
GNSSPositionCallback cbPosition = 0;

TGNSSTime gTime;
GNSSTimeCallback cbTime = 0;

bool gnssExtendedInit()
{
    return true;
}

bool gnssExtendedDestroy()
{
    return true;
}


bool gnssRegisterSatelliteDetailCallback(GNSSSatelliteDetailCallback callback)
{
    if(cbSatelliteDetail != 0) 
    {
        return false; //if already registered
    }

    pthread_mutex_lock(&mutexCb);
    cbSatelliteDetail = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool gnssDeregisterSatelliteDetailCallback(GNSSSatelliteDetailCallback callback)
{
    if(cbSatelliteDetail == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbSatelliteDetail = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }

    return false;
}

bool gnssGetSatelliteDetails(TGNSSSatelliteDetail* satelliteDetails, uint16_t count, uint16_t* numSatelliteDetails)
{
    if(!satelliteDetails || !count)
    {
        return false;
    }

//TODO: return full set of satellite details for one point in time
    pthread_mutex_lock(&mutexData);
    *satelliteDetails = gSatelliteDetail;
    *numSatelliteDetails = 1; 
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool gnssRegisterPositionCallback(GNSSPositionCallback callback)
{
    if(cbPosition != 0) 
    {
        return false; //if already registered
    }

    pthread_mutex_lock(&mutexCb);
    cbPosition = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool gnssDeregisterPositionCallback(GNSSPositionCallback callback)
{
    if(cbPosition == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbPosition = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }

    return false;
}

bool gnssGetPosition(TGNSSPosition* position)
{
    if(!position)
    {
        return false;
    }

    pthread_mutex_lock(&mutexData);
    *position = gPosition;
    pthread_mutex_unlock(&mutexData);

    return true;
}


bool gnssRegisterTimeCallback(GNSSTimeCallback callback)
{
    if(cbTime != 0) 
    {
        return false; //if already registered
    }

    pthread_mutex_lock(&mutexCb);
    cbTime = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool gnssDeregisterTimeCallback(GNSSTimeCallback callback)
{
    if(cbTime == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbTime = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }

    return false;
}

bool gnssGetTime(TGNSSTime* time)
{
    if(!time)
    {
        return false;
    }

    pthread_mutex_lock(&mutexData);
    *time = gTime;
    pthread_mutex_unlock(&mutexData);

    return true;
}