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

#include "gnss-init.h"
#include "gnss.h"
#include "log.h"

DLT_DECLARE_CONTEXT(gCtx);

static void cbTime(const TGNSSTime time[], uint16_t numElements)
{
    int i;    
    if(time == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbTime failed!");
        return;
    }

    for (i = 0; i<numElements; i++)
    {
        if (time[i].validityBits & GNSS_TIME_DATE_VALID)
        {
            char month [12] [4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};         
            LOG_INFO(gCtx,"Time Update[%d/%d]: timestamp=%llu UTC: %04d-%s-%02d %02d:%02d:%02d",
                 i+1,
                 numElements,
                 time[i].timestamp, 
                 time[i].year,
                 month[time[i].month%12],
                 time[i].day,
                 time[i].hour,
                 time[i].minute,
                 time[i].second);
        }
        else
        {
            LOG_INFO(gCtx,"Time Update[%d/%d]: Invalid Date/Time",
                 i+1,
                 numElements);
            
        }
    }
}

static void cbPosition(const TGNSSPosition position[], uint16_t numElements)
{
    int i;    
    if(position == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbPosition failed!");
        return;
    }

    for (i = 0; i<numElements; i++)
    {
        LOG_INFO(gCtx,"Position Update[%d/%d]: timestamp=%llu latitude=%.5f longitude=%.5f altitudeMSL=%.1f hSpeed=%.1f heading=%.1f\n hdop=%.1f usedSatellites=%d sigmaHPosition=%.1f sigmaHSpeed=%.1f sigmaHeading=%.1f\n fixStatus=%d fixTypeBits=0x%08X activatedSystems=0x%08X usedSystems=0x%08X",
                 i+1,
                 numElements,
                 position[i].timestamp, 
                 position[i].latitude,
                 position[i].longitude,
                 position[i].altitudeMSL,
                 position[i].hSpeed,
                 position[i].heading,
                 position[i].hdop,
                 position[i].usedSatellites,
                 position[i].sigmaHPosition,
                 position[i].sigmaHSpeed,
                 position[i].sigmaHeading,
                 position[i].fixStatus,
                 position[i].fixTypeBits,
                 position[i].activatedSystems,
                 position[i].usedSystems);
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
    if(!checkMajorVersion(GENIVI_GNSS_API_MAJOR))
    {
        exit(EXIT_FAILURE);
    }

    if(!gnssInit())
    {
        exit(EXIT_FAILURE);
    }
}

int main()
{
    init();

    DLT_REGISTER_APP("GNSS", "GNSS-SERVICE-CLIENT");
    DLT_REGISTER_CONTEXT(gCtx,"GCLT", "Global Context");

    if (!gnssSetGNSSSystems(GNSS_SYSTEM_GPS|GNSS_SYSTEM_GLONASS))
    {
        LOG_INFO_MSG(gCtx,"gnssConfigGNSSSystems not supported!");
    }

    LOG_INFO_MSG(gCtx,"Starting gnss-service-client...");

    // register for GNSS
    gnssRegisterTimeCallback(&cbTime);    
    gnssRegisterSatelliteDetailCallback(&cbSatelliteDetail);
    gnssRegisterPositionCallback(&cbPosition);

    // enter endless loop
    while(1)
    {
        sleep(1);
    }

    // deregister
    gnssDeregisterTimeCallback(&cbTime);    
    gnssDeregisterSatelliteDetailCallback(&cbSatelliteDetail);
    gnssDeregisterPositionCallback(&cbPosition);

    gnssDestroy();

    return EXIT_SUCCESS;
}
