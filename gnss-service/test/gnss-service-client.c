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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

#include "gnss.h"
#include "gnss-ext.h"
#include "log.h"

DLT_DECLARE_CONTEXT(gCtx);


static void cbSpatial(const TGNSSSpatial spatial[], uint16_t numElements)
{
    int i;    
    if(spatial == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbSpatial failed!");
        return;
    }

    for (i = 0; i<numElements; i++)
    {
        LOG_INFO(gCtx,"Location Update[%d/%d]: timestamp=%llu latitude=%.5f longitude=%.5f altitudeMSL=%.1f hSpeed=%.1f heading=%.1f\n hdop=%.1f usedSatellites=%d sigmaHPosition=%.1f sigmaHSpeed=%.1f sigmaHeading=%.1f fixStatus=%d fixTypeBits=0x%08X",
                 i+1,
                 numElements,
                 spatial[i].timestamp, 
                 spatial[i].latitude,
                 spatial[i].longitude,
                 spatial[i].altitudeMSL,
                 spatial[i].hSpeed,
                 spatial[i].heading,
                 spatial[i].hdop,
                 spatial[i].usedSatellites,
                 spatial[i].sigmaHPosition,
                 spatial[i].sigmaHSpeed,
                 spatial[i].sigmaHeading,
                 spatial[i].fixStatus,
                 spatial[i].fixTypeBits);
    }
}

static void cbSatelliteDetail(const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements)
{
    int i;    
    if(satelliteDetail == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbSatelliteDetail failed!");
        return;
    }

    for (i = 0; i<numElements; i++)
    {
        LOG_INFO(gCtx,"SatelliteDetail Update[%d/%d]: satelliteId=%d azimuth=%d elevation=%d SNR=%d",
                 i+1,
                 numElements,
                 satelliteDetail[i].satelliteId, 
                 satelliteDetail[i].azimuth,
                 satelliteDetail[i].elevation,                                   
                 satelliteDetail[i].SNR);
    }    
}

bool checkMajorVersion(int expectedMajor)
{
    int major = -1;

    gnssGetVersion(&major, 0, 0);

    if(major != expectedMajor)
    {
        LOG_ERROR(gCtx,"Wrong API version: gnssGetVersion returned unexpected value %d != %d",
                  major, 
                  expectedMajor);

        return false;
    }

    return true;
}

void init()
{
    if(!checkMajorVersion(2))
    {
        exit(EXIT_FAILURE);
    }

    if(!gnssInit())
    {
        exit(EXIT_FAILURE);
    }

    if(!gnssExtendedInit())
    {
        exit(EXIT_FAILURE);
    }
}

int main()
{
    init();

    DLT_REGISTER_APP("GNSS", "GNSS-SERVICE-CLIENT");
    DLT_REGISTER_CONTEXT(gCtx,"GCLT", "Global Context");

    LOG_INFO_MSG(gCtx,"Starting gnss-service-client...");

    // register for GNSS
    gnssExtendedRegisterSatelliteDetailCallback(&cbSatelliteDetail);
    gnssExtendedRegisterSpatialCallback(&cbSpatial);

    // enter endless loop
    while(1)
    {
        sleep(1);
    }

    // deregister
    gnssExtendedDeregisterSatelliteDetailCallback(&cbSatelliteDetail);
    gnssExtendedDeregisterSpatialCallback(&cbSpatial);

    gnssExtendedDestroy();
    gnssDestroy();

    return EXIT_SUCCESS;
}
