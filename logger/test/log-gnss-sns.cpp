/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \brief Test program for GNSS+SNS logging
*
*
* \author Helmut Schmidt <https://github.com/huirad>
*
* \copyright Copyright (C) 2015, Helmut Schmidt
*
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#include "poslog.h"
#include "gnsslog.h"
#include "snslog.h"
#if (DLT_ENABLED)
#include "dlt.h"
#endif

#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>

#include "gnss-init.h"
#include "gnss.h"
#include "sns-init.h"

#define GNSS_INIT_MAX_RETRIES 30

//global variable to control the main loop - will be set by signal handlers
static volatile bool sigint = false;
static volatile bool sigterm = false;

static void sigHandler (int sig, siginfo_t *siginfo, void *context)
{
    if (sig == SIGINT) 
    {
        sigint = true;
    }
    else 
    if (sig == SIGTERM) 
    {
        sigterm = true;
    }
}

static bool registerSigHandlers()
{
    bool is_success = true;
    
	struct sigaction action;
 	memset (&action, '\0', sizeof(action));
	action.sa_sigaction = &sigHandler;
	action.sa_flags = SA_SIGINFO;

  	if (sigaction(SIGINT, &action, NULL) < 0) 
    {
		is_success = false;
	}
	if (sigaction(SIGTERM, &action, NULL) < 0) 
    {
		is_success = false;
	}
    return is_success;
}



static void cbTime(const TGNSSTime time[], uint16_t numElements)
{
    gnssTimeLog(gnsslogGetTimestamp(), time, numElements);
}

static void cbPosition(const TGNSSPosition position[], uint16_t numElements)
{
    gnssPositionLog(gnsslogGetTimestamp(), position, numElements);
}

static void cbAccel(const TAccelerationData accelerationData[], uint16_t numElements)
{
    accelerationDataLog(snslogGetTimestamp(), accelerationData, numElements);
}

static void cbGyro(const TGyroscopeData gyroData[], uint16_t numElements)
{
    gyroscopeDataLog(snslogGetTimestamp(), gyroData, numElements);
}


int main()
{
    int major;
    int minor;
    int micro;
    char version_string[64];
    
    bool is_poslog_init_ok = false;
    bool is_sns_init_ok = false;    
    bool is_sns_gyro_init_ok = false;
    bool is_sns_accel_init_ok = false;
    bool is_gnss_init_ok = false;
    int gnss_init_tries = 0;
    
    registerSigHandlers();
    
#if (DLT_ENABLED)
    DLT_REGISTER_APP("GLT","GNSS/SNS Logger");
#endif
    poslogSetFD(STDOUT_FILENO);
    is_poslog_init_ok = poslogInit();
    if (is_poslog_init_ok)
    {
        poslogSetActiveSinks(POSLOG_SINK_DLT|POSLOG_SINK_FD|POSLOG_SINK_CB);

        gnssGetVersion(&major, &minor, &micro);
        sprintf(version_string, "0,0$GVGNSVER,%d,%d,%d", major, minor, micro);
        poslogAddString(version_string);
        snsGetVersion(&major, &minor, &micro);
        sprintf(version_string, "0,0$GVSNSVER,%d,%d,%d", major, minor, micro);
        poslogAddString(version_string);

        is_sns_init_ok = snsInit();
        if (is_sns_init_ok)
        {
            is_sns_gyro_init_ok = snsGyroscopeInit();
            if(is_sns_gyro_init_ok)
            {
                poslogAddString("#INF snsGyroscopeInit() success");
                snsGyroscopeRegisterCallback(&cbGyro);
            }
            is_sns_accel_init_ok = snsAccelerationInit();
            if (is_sns_accel_init_ok)
            {
                poslogAddString("#INF snsAccelerationInit() success");
                snsAccelerationRegisterCallback(&cbAccel);
            }
            if (!is_sns_gyro_init_ok && !is_sns_accel_init_ok)
            {
                is_sns_init_ok = false;
                snsDestroy();
            }
            else
            {
                poslogAddString("#INF snsInit() success");
            }
        }

        //GNSS device may be available a bit late after startup
        is_gnss_init_ok = gnssInit();
        while (!is_gnss_init_ok && (gnss_init_tries < GNSS_INIT_MAX_RETRIES) && !sigint && !sigterm)
        {
            sleep(1);
            is_gnss_init_ok = gnssInit();
            gnss_init_tries += 1;
        }
        if (is_gnss_init_ok)
        {
            poslogAddString("#INF gnssInit() success");
            gnssRegisterTimeCallback(&cbTime);
            gnssRegisterPositionCallback(&cbPosition);
        }

        if (is_sns_init_ok || is_gnss_init_ok)
        {
            while(!sigint && !sigterm)
            {
                sleep(1);
            }
        }
        else
        {
            poslogAddString("#ERR snsInit() or gnssInit() failure - terminating");
        }
        
        //if not interrupted by SIGTERM then we have time to cleanup
        if (!sigterm) 
        {
            if (sigint)
            {
                poslogAddString("#SIGINT");
            }
            if (is_sns_init_ok)
            {
                if (is_sns_accel_init_ok)
                {
                    snsAccelerationDeregisterCallback(&cbAccel);
                    snsAccelerationDestroy();
                }
                if (is_sns_gyro_init_ok)
                {
                    snsGyroscopeRegisterCallback(&cbGyro);
                    snsGyroscopeDestroy();
                }
                snsDestroy();
            }
            if (is_gnss_init_ok)
            {
                gnssDeregisterPositionCallback(&cbPosition);
                gnssDeregisterTimeCallback(&cbTime);
                gnssDestroy();
            }
        }
        poslogDestroy();
    }
#if (DLT_ENABLED)
    DLT_UNREGISTER_APP();
#endif
}
