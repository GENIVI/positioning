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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <memory.h>

#include "globals.h"
#include "sns-init.h"
#include "log.h"

#define BUFLEN 256
#define MSGIDLEN 20
#define PORT 5555

//Sensors IDs used by the SensorLogger App
#define GPS_SENSOR 1
#define ACC_SENSOR 2
#define COMPASS    3
#define GYRO       4

DLT_DECLARE_CONTEXT(gContext);

pthread_t listenerThread;
bool isRunning = true;

void *listenForMessages( void *ptr );

bool snsInit()
{
    isRunning = true;

    if(pthread_create( &listenerThread, NULL, listenForMessages, NULL) != 0)
    {
        isRunning = false;

    	return false;
    }

    return true;
}

bool snsDestroy()
{
    isRunning = false;

    if(listenerThread)
    {
        pthread_join( listenerThread, NULL);
    }

    return true;
}

void snsGetVersion(int *major, int *minor, int *micro)
{
    if(major)
    {
        *major = GENIVI_SNS_API_MAJOR;
    }

    if(minor)
    {
        *minor = GENIVI_SNS_API_MINOR;
    }

    if(micro)
    {
        *micro = GENIVI_SNS_API_MICRO;
    }
}

bool snsGyroscopeInit()
{
    return iGyroscopeInit();
}

bool snsGyroscopeDestroy()
{
    return iGyroscopeDestroy();
}


static bool processGVGYRO(char* data)
{
    long unsigned int timestamp = 0;
    float yawRate;
    float pitchRate;
    float rollRate;
    int32_t sensorId;
    uint32_t n = 0;

    if(!data || !pGyroscopeData)
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%lu,%d,%f,%f,%f", &timestamp, &sensorId, &yawRate, &pitchRate, &rollRate);

    if(n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVGYRO failed!");
        return false;
    }

    TGyroscopeData gyroscopeData    
    
    gyroscopeData.timestamp = timestamp;

    //LOG_INFO(gContext,"timestamp:%lu",timestamp);

    //angleYaw
    gyroscopeData.yawRate = yawRate;
    gyroscopeData.validityBits |= GYROSCOPE_CONFIG_ANGLEYAW_VALID;
    LOG_INFO(gContext,"yawRate: %lf", gyroscopeData.yawRate);
     
    //anglePitch
    gyroscopeData.pitchRate = pitchRate;
    gyroscopeData.validityBits |= GYROSCOPE_CONFIG_ANGLEPITCH_VALID;
    LOG_INFO(gContext,"pitchRate: %lf", gyroscopeData.pitchRate);

    //angleRoll
    gyroscopeData.rollRate = rollRate;
    gyroscopeData.validityBits |= GYROSCOPE_CONFIG_ANGLEROLL_VALID;
    LOG_INFO(gContext,"rollRate: %f", gyroscopeData.rollRate);

    if(cbGyroscope != 0)
    {
        cbGyroscope(&gyroscopeData,1);
    }

    return true;
}

void *listenForMessages( void *ptr )
{  
    struct sockaddr_in si_me;
    struct sockaddr_in si_other;
    long unsigned int timestamp = 0;
    int s;
    socklen_t slen = sizeof(si_other);
    ssize_t readBytes = 0;
    char buf[BUFLEN];
    int msgId;
    int port = PORT;

    DLT_REGISTER_APP("SNSS", "SENSORS-SERVICE");
    DLT_REGISTER_CONTEXT(gContext,"SSRV", "Global Context");

    LOG_INFO(gContext,"Listening on port %d...",port);

    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
        LOG_ERROR_MSG(gContext,"socket() failed!");
        exit(EXIT_FAILURE);
    }

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;

    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(s, (struct sockaddr *)&si_me, sizeof(si_me)) == -1)
    {
        LOG_ERROR_MSG(gContext,"socket() failed!");
        exit(EXIT_FAILURE);
    }

    while(isRunning == true)
    {
        readBytes = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, &slen);

        if(readBytes < 0)
        {
            LOG_ERROR_MSG(gContext,"recvfrom() failed!");
            exit(EXIT_FAILURE);
        }

        buf[readBytes] = '\0';

        LOG_DEBUG_MSG(gContext,"------------------------------------------------");

        LOG_DEBUG(gContext,"Received Packet from %s:%d", 
                  inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

        sscanf(buf,"%lu,%d,",&timestamp, &msgId);

        LOG_DEBUG_MSG(gContext,"------------------------------------------------");
        LOG_DEBUG(gContext,"msgID:%d",msgId);
        
        pthread_mutex_lock(&mutexData);

        if(msgId == GYRO) 
        {
            processGVGYRO(buf, &gGyroscopeData);
        }

        pthread_mutex_unlock(&mutexData);
    }

    close(s);

    return EXIT_SUCCESS;
}
