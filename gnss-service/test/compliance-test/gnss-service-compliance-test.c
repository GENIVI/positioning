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

#define TEST_FAILED EXIT_FAILURE
#define TEST_PASSED EXIT_SUCCESS

static int testResult = TEST_PASSED;


static void cbPosition(const TGNSSPosition pos[], uint16_t numElements)
{
    if(pos == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbPosition failed!");
        testResult = TEST_FAILED;
        return;
    }
}

static void cbCourse(const TGNSSCourse course[], uint16_t numElements)
{
    if(course == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbCourse failed!");
        testResult = TEST_FAILED;
        return;
    }
}

static void cbAccuracy(const TGNSSAccuracy accuracy[], uint16_t numElements)
{
    if(accuracy == NULL || numElements < 1)
    {
        LOG_ERROR_MSG(gCtx,"cbAccuracy failed!");
        testResult = TEST_FAILED;
        return;
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

        testResult = TEST_FAILED;
        return false;
    }

    return true;
}

bool init()
{
    if(!checkMajorVersion(2))
    {
        return false;
    }

    if(!gnssInit())
    {
        return false;
    }

    if(!gnssSimpleInit())
    {
        return false;
    }

    return true;
}

int main()
{
    int i = 0;

    DLT_REGISTER_APP("GNSS", "GNSS-SERVICE-COMPLIANCE-TEST");
    DLT_REGISTER_CONTEXT(gCtx,"GCLT", "Global Context");
    
    LOG_INFO_MSG(gCtx,"Starting gnss-service-compliance-test...");

    if(init())
    {
        //register for GNSS
        gnssSimpleRegisterPositionCallback(&cbPosition);
        gnssSimpleRegisterCourseCallback(&cbCourse);
        gnssExtendedRegisterAccuracyCallback(&cbAccuracy);

        //listen for events for about 10 seconds
        for(i = 0; i < 10; i++)
        {
            sleep(1);
        }

        //deregister
        gnssSimpleDeregisterPositionCallback(&cbPosition);
        gnssSimpleDeregisterCourseCallback(&cbCourse);
        gnssExtendedDeregisterAccuracyCallback(&cbAccuracy);

        gnssSimpleDestroy();
        gnssDestroy();
    }
    
    //return if the test succeded or failed
    if(testResult == TEST_FAILED)
    { 
         LOG_INFO_MSG(gCtx,"TEST_FAILED");
         return EXIT_FAILURE;
    }

    LOG_INFO_MSG(gCtx,"TEST_PASSED");

    return EXIT_SUCCESS;
}
