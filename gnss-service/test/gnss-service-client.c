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
#include "gnss-simple.h"
#include "gnss-ext.h"
#include "log.h"

DLT_DECLARE_CONTEXT(gCtx);

static void cbPosition(const TGNSSPosition pos[], uint16_t numElements)
{
    int i;
    if(pos == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbPosition failed!");
        return;
    }

    for (i = 0; i<numElements; i++)
    {
        LOG_INFO(gCtx,"Position Update[%d/%d]: lat=%f lon=%f",
                 i+1,
                 numElements,
                 pos[i].latitude, 
                 pos[i].longitude);
    }
}

static void cbCourse(const TGNSSCourse course[], uint16_t numElements)
{
    int i;
    if(course == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbCourse failed!");
        return;
    }

    for (i = 0; i<numElements; i++)
    {
        LOG_INFO(gCtx,"Course Updatee[%d/%d]: speed=%f heading=%f climb=%f",
                 i+1,
                 numElements,
                 course[i].speed, 
                 course[i].heading, 
                 course[i].climb);        
    }

}

static void cbAccuracy(const TGNSSAccuracy accuracy[], uint16_t numElements)
{
    int i;    
    if(accuracy == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbAccuracy failed!");
        return;
    }

    for (i = 0; i<numElements; i++)
    {
        LOG_INFO(gCtx,"Accuracy Update[%d/%d]: usedSatellites=%d visibleSatellites=%d fixStatus=%d fixTypeBits=0x%08X",
                 i+1,
                 numElements,
                 accuracy[i].usedSatellites, 
                 accuracy[i].visibleSatellites,
                 accuracy[i].fixStatus,
                 accuracy[i].fixTypeBits);
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

    if(!gnssSimpleInit())
    {
        exit(EXIT_FAILURE);
    }
}

int main()
{
    init();

    DLT_REGISTER_APP("GNSS", "GNSS-SERVICE-CLIENT");
    DLT_REGISTER_CONTEXT(gCtx,"GCLT", "Global Context");

    LOG_INFO_MSG(gCtx,"starting gnss-service-client...");

    // register for GNSS
    gnssSimpleRegisterPositionCallback(&cbPosition);
    gnssSimpleRegisterCourseCallback(&cbCourse);
    gnssExtendedRegisterAccuracyCallback(&cbAccuracy);
    gnssExtendedRegisterSatelliteDetailCallback(&cbSatelliteDetail);

    // enter endless loop
    while(1)
    {
        sleep(1);
    }

    // deregister
    gnssSimpleDeregisterPositionCallback(&cbPosition);
    gnssSimpleDeregisterCourseCallback(&cbCourse);
    gnssExtendedDeregisterAccuracyCallback(&cbAccuracy);
    gnssExtendedDeregisterSatelliteDetailCallback(&cbSatelliteDetail);

    gnssSimpleDestroy();
    gnssDestroy();

    return EXIT_SUCCESS;
}
