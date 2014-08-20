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

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <unistd.h>

//include all sns header files here even if they are not yet used.
//this helps us to find syntax error during compilation
#include "sns-init.h"
#include "acceleration.h"
#include "gyroscope.h"
#include "inclination.h"
#include "odometer.h"
#include "reverse-gear.h"
#include "slip-angle.h"
#include "steering-angle.h"
#include "vehicle-data.h"
#include "vehicle-speed.h"
#include "vehicle-state.h"
#include "wheel.h"
#include "log.h"

DLT_DECLARE_CONTEXT(gCtx);

static void cbGyroscope(const TGyroscopeData gyroData[], uint16_t numElements)
{
    if(gyroData == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"error: cbGyroscope failed!");
        return;
    }

    LOG_INFO_MSG(gCtx,"Gyroscope Update");

    LOG_INFO(gCtx,"yawRate=%f", gyroData[0].yawRate);
    LOG_INFO(gCtx,"pitchRate=%f", gyroData[0].pitchRate);
    LOG_INFO(gCtx,"rollRate=%f", gyroData[0].rollRate);
}

static void cbWheelticks(const TWheelticks ticks[], uint16_t numElements)
{
    char* wheelIDs[] = 
    {"WHEEL_INVALID"
    ,"WHEEL_UNKNOWN"
    ,"WHEEL_AXLE_NONDRIVEN"
    ,"WHEEL_AXLE_FRONT"
    ,"WHEEL_AXLE_REAR"
    ,"WHEEL_LEFT_FRONT"
    ,"WHEEL_RIGHT_FRONT"
    ,"WHEEL_LEFT_REAR"
    ,"WHEEL_RIGHT_REAR"};
    int i=0;
    
    if(ticks == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"error: cbWheelticks failed!");
        return;
    }

    LOG_INFO_MSG(gCtx,"Wheelticks Update");

    for (i=0; i<WHEEL_NUM_ELEMENTS; i++)
    {
        LOG_INFO(gCtx,"ticks[0].elements[%d]: %s = %d", i,
                    wheelIDs[ticks[0].elements[i].wheeltickIdentifier],
                    ticks[0].elements[i].wheeltickCounter);
    }

}

static void cbVehicleSpeed(const TVehicleSpeedData vehSpeed[], uint16_t numElements)
{
    if(vehSpeed == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"error: cbVehicleSpeed failed!");
        return;
    }

    LOG_INFO_MSG(gCtx,"VehicleSpeed Update");

    LOG_INFO(gCtx,"vehicleSpeed=%f", vehSpeed[0].vehicleSpeed);
}

bool checkMajorVersion(int expectedMajor)
{
    int major = -1;

    snsGetVersion(&major, 0, 0);

    if(major != expectedMajor) 
    {
        LOG_ERROR(gCtx,"wrong API version: snsGetVersion returned unexpected value %d != %d", 
                  major, expectedMajor);
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

    assert( snsInit() );
    assert( snsGyroscopeInit() );
    assert( snsWheeltickInit() );
    assert( snsVehicleSpeedInit() );
}

int main()
{
    init();

    DLT_REGISTER_APP("SNSS", "SENSORS-SERVICE");
    DLT_REGISTER_CONTEXT(gCtx,"SCLT", "Global Context");

    LOG_INFO_MSG(gCtx,"Starting sensors-service-client...");

    //register
    snsWheeltickRegisterCallback(&cbWheelticks);
    snsGyroscopeRegisterCallback(&cbGyroscope);
    snsVehicleSpeedRegisterCallback(&cbVehicleSpeed);

    //enter endless loop
    while(1)
    {
    	sleep(1);
    }

    //deregister
    snsWheeltickDeregisterCallback(&cbWheelticks);
    snsGyroscopeDeregisterCallback(&cbGyroscope);
    snsVehicleSpeedDeregisterCallback(&cbVehicleSpeed);

    snsGyroscopeDestroy();
    snsWheeltickDestroy();
    snsVehicleSpeedDestroy();
    snsDestroy();

    return EXIT_SUCCESS;
}
