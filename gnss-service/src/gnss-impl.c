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
#include "gnss-status.h"


static pthread_mutex_t mutexCb  = PTHREAD_MUTEX_INITIALIZER;   //protects the callbacks
static pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;  //protects the data

static TGNSSSatelliteDetail gSatelliteDetail = {0}; //TODO: buffer full set of satellite details for one point in time
static GNSSSatelliteDetailCallback cbSatelliteDetail = 0;

static TGNSSPosition gPosition = {0};
static volatile GNSSPositionCallback cbPosition = 0;

static TGNSSTime gTime = {0};
static volatile GNSSTimeCallback cbTime = 0;

static TGNSSStatus gStatus = {0};
static volatile GNSSStatusCallback cbStatus = 0;


bool gnssRegisterSatelliteDetailCallback(GNSSSatelliteDetailCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbSatelliteDetail)
    {
        cbSatelliteDetail = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool gnssDeregisterSatelliteDetailCallback(GNSSSatelliteDetailCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbSatelliteDetail == callback) && callback)
    {
        cbSatelliteDetail = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool gnssGetSatelliteDetails(TGNSSSatelliteDetail* satelliteDetails, uint16_t count, uint16_t* numSatelliteDetails)
{
    bool retval = false;

    if(satelliteDetails && count)
    {
//TODO: return full set of satellite details for one point in time
        pthread_mutex_lock(&mutexData);
        *satelliteDetails = gSatelliteDetail;
        *numSatelliteDetails = 1;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }

    return retval;
}

bool gnssRegisterPositionCallback(GNSSPositionCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbPosition)
    {
        cbPosition = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool gnssDeregisterPositionCallback(GNSSPositionCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbPosition == callback) && callback)
    {
        cbPosition = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool gnssGetPosition(TGNSSPosition* position)
{
    bool retval = false;
    if(position)
    {
        pthread_mutex_lock(&mutexData);
        *position = gPosition;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}


bool gnssRegisterTimeCallback(GNSSTimeCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    //only if valid callback and not already registered
    if(callback && !cbTime)
    {
        cbTime = callback;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}


bool gnssDeregisterTimeCallback(GNSSTimeCallback callback)
{
    bool retval = false;

    pthread_mutex_lock(&mutexCb);
    if((cbTime == callback) && callback)
    {
        cbTime = 0;
        retval = true;
    }
    pthread_mutex_unlock(&mutexCb);

    return retval;
}

bool gnssGetTime(TGNSSTime* time)
{
    bool retval = false;
    if(time)
    {
        pthread_mutex_lock(&mutexData);
        *time = gTime;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }
    return retval;
}

bool gnssRegisterStatusCallback(GNSSStatusCallback callback)
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


bool gnssDeregisterStatusCallback(GNSSStatusCallback callback)
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

bool gnssGetStatus(TGNSSStatus* status)
{
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


void updateGNSSTime(const TGNSSTime time[], uint16_t numElements)
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


void updateGNSSStatus(const TGNSSStatus* status)
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