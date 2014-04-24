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

TGNSSAccuracy gAccuracy;
GNSSAccuracyCallback cbAccuracy = 0;

TGNSSSatelliteDetail gSatelliteDetail; //TODO: buffer full set of satellite details for one point in time
GNSSSatelliteDetailCallback cbSatelliteDetail = 0;

bool gnssExtendedInit()
{
    return true;
}

bool gnssExtendedDestroy()
{
    return true;
}

bool gnssExtendedRegisterAccuracyCallback(GNSSAccuracyCallback callback)
{
    if(cbAccuracy != 0) 
    {
        return false; //if already registered
    }

    pthread_mutex_lock(&mutexCb);
    cbAccuracy = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool gnssExtendedDeregisterAccuracyCallback(GNSSAccuracyCallback callback)
{
    if(cbAccuracy == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbAccuracy = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }

    return false;
}

bool gnssExtendedGetAccuracy(TGNSSAccuracy* accuracy)
{
    if(!accuracy)
    {
        return false;
    }

    pthread_mutex_lock(&mutexData);
    *accuracy = gAccuracy;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool gnssExtendedRegisterSatelliteDetailCallback(GNSSSatelliteDetailCallback callback)
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

bool gnssExtendedDeregisterSatelliteDetailCallback(GNSSSatelliteDetailCallback callback)
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

bool gnssExtendedGetSatelliteDetails(TGNSSSatelliteDetail* satelliteDetails, uint16_t count, uint16_t* numSatelliteDetails)
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
