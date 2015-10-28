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
#include "gnss.h"

static pthread_mutex_t mutexCb  = PTHREAD_MUTEX_INITIALIZER;   //protects the callbacks
static pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;  //protects the data

static TGNSSSatelliteDetail gSatelliteDetail; //TODO: buffer full set of satellite details for one point in time
static GNSSSatelliteDetailCallback cbSatelliteDetail = 0;

static TGNSSPosition gPosition;
static volatile GNSSPositionCallback cbPosition = 0;

static TGNSSTime gTime;
static volatile GNSSTimeCallback cbTime = 0;




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


void updateGNSSTime(const TGNSSTime time[], uint16_t numElements)
//void updateGNSSTime(const TGNSSTime* gnss_time)
{
    if (time != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gTime = time[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbTime)
        {
            cbTime(time, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}


void updateGNSSPosition(const TGNSSPosition position[], uint16_t numElements)
//void updateGNSSPosition(const TGNSSPosition* gnss_pos)
{
    if (position != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gPosition = position[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbPosition)
        {
            cbPosition(position, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}

void updateGNSSSatelliteDetail(const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements)
{
    if (satelliteDetail != NULL && numElements > 0)
    {
        pthread_mutex_lock(&mutexData);
        gSatelliteDetail = satelliteDetail[numElements-1];
        pthread_mutex_unlock(&mutexData);
        pthread_mutex_lock(&mutexCb);
        if (cbSatelliteDetail)
        {
            cbSatelliteDetail(satelliteDetail, numElements);
        }
        pthread_mutex_unlock(&mutexCb);
    }
}
