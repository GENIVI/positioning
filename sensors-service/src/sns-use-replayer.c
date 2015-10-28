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

#include "sns-init.h"
#include "acceleration.h"
#include "gyroscope.h"

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
#include "log.h"

#define BUFLEN 256
#define MSGIDLEN 20
#define PORT 9931

#define MAX_BUF_MSG 16

DLT_DECLARE_CONTEXT(gContext);

pthread_t listenerThread;
bool isRunning = false;

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

bool snsAccelerationInit()
{
    return iAccelerationInit();
}

bool snsAccelerationDestroy()
{
    return iAccelerationDestroy();
}

bool snsGyroscopeInit()
{
    return iGyroscopeInit();
}

bool snsGyroscopeDestroy()
{
    return iGyroscopeDestroy();
}

bool snsVehicleSpeedInit()
{
    return iVehicleSpeedInit();
}

bool snsVehicleSpeedDestroy()
{
    return iVehicleSpeedDestroy();
}

bool snsWheeltickInit()
{
    return iWheeltickInit();
}

bool snsWheeltickDestroy()
{
    return iWheeltickDestroy();
}

bool processGVSNSWHTK(char* data)
{
    //parse data like: 061076000,0$GVSNSWHTK,061076000,7,266,8,185,0,0,0,0
    
    //storage for buffered data
    static TWheelticks buf_whtk[MAX_BUF_MSG];
    static uint16_t buf_size = 0;
    static uint16_t last_countdown = 0;    

    uint64_t timestamp;
    uint16_t countdown;
    TWheelticks whtk = { 0 };
    uint32_t n = 0;

    if(!data)
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%llu,%hu$GVSNSWHTK,%llu,%u,%u,%u,%u,%u,%u,%u,%u", &timestamp, &countdown, &whtk.timestamp
      ,&whtk.elements[0].wheeltickIdentifier, &whtk.elements[0].wheeltickCounter
      ,&whtk.elements[1].wheeltickIdentifier, &whtk.elements[1].wheeltickCounter
      ,&whtk.elements[2].wheeltickIdentifier, &whtk.elements[2].wheeltickCounter
      ,&whtk.elements[3].wheeltickIdentifier, &whtk.elements[3].wheeltickCounter
      );

    if (n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVSNSWHTK failed!");
        return false;
    }

    //buffered data handling
    if (countdown < MAX_BUF_MSG) //enough space in buffer?
    {
        if (buf_size == 0) //a new sequence starts
        {
            buf_size = countdown+1;
            last_countdown = countdown;
            buf_whtk[buf_size-countdown-1] = whtk;
        }
        else if ((last_countdown-countdown) == 1) //sequence continued
        {
            last_countdown = countdown;
            buf_whtk[buf_size-countdown-1] = whtk;
        }
        else //sequence interrupted: clear buffer
        {
            buf_size = 0;
            last_countdown = 0;
        }
    }
    else //clear buffer
    {
        buf_size = 0;
        last_countdown = 0;
    }

    if((countdown == 0) && (buf_size >0) )
    {
        updateWheelticks(buf_whtk,buf_size);
        buf_size = 0;
        last_countdown = 0;        
    }

    return true;
}

static bool processGVSNSGYRO(char* data)
{
    //parse data like: 061074000,0$GVSNSGYRO,061074000,-38.75,0,0,0,0X01
    
    //storage for buffered data
    static TGyroscopeData buf_gyro[MAX_BUF_MSG];
    static uint16_t buf_size = 0;
    static uint16_t last_countdown = 0;    

    uint64_t timestamp;
    uint16_t countdown;
    TGyroscopeData gyro = { 0 };
    uint32_t n = 0;

    if(!data )
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%llu,%hu$GVSNSGYRO,%llu,%f,%f,%f,%f,%x", &timestamp, &countdown, &gyro.timestamp, &gyro.yawRate, &gyro.pitchRate, &gyro.rollRate, &gyro.temperature, &gyro.validityBits);    

    if (n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVSNSGYRO failed!");
        return false;
    }

    //buffered data handling
    if (countdown < MAX_BUF_MSG) //enough space in buffer?
    {
        if (buf_size == 0) //a new sequence starts
        {
            buf_size = countdown+1;
            last_countdown = countdown;
            buf_gyro[buf_size-countdown-1] = gyro;
        }
        else if ((last_countdown-countdown) == 1) //sequence continued
        {
            last_countdown = countdown;
            buf_gyro[buf_size-countdown-1] = gyro;
        }
        else //sequence interrupted: clear buffer
        {
            buf_size = 0;
            last_countdown = 0;
        }
    }
    else //clear buffer
    {
        buf_size = 0;
        last_countdown = 0;
    }

    if((countdown == 0) && (buf_size >0) )
    {
        updateGyroscopeData(buf_gyro,buf_size);
        buf_size = 0;
        last_countdown = 0;        
    }

    return true;
}



static bool processGVSNSVEHSP(char* data)
{
    //parse data like: 061074000,0$GVSNSVEHSP,061074000,0.51,0X01
    
    //storage for buffered data
    static TVehicleSpeedData buf_vehsp[MAX_BUF_MSG];
    static uint16_t buf_size = 0;
    static uint16_t last_countdown = 0;    

    uint64_t timestamp;
    uint16_t countdown;
    TVehicleSpeedData vehsp = { 0 };
    uint32_t n = 0;

    if(!data)
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%llu,%hu$GVSNSVEHSP,%llu,%f,%x", &timestamp, &countdown, &vehsp.timestamp, &vehsp.vehicleSpeed, &vehsp.validityBits);    

    if (n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVSNSVEHSP failed!");
        return false;
    }

    //buffered data handling
    if (countdown < MAX_BUF_MSG) //enough space in buffer?
    {
        if (buf_size == 0) //a new sequence starts
        {
            buf_size = countdown+1;
            last_countdown = countdown;
            buf_vehsp[buf_size-countdown-1] = vehsp;
        }
        else if ((last_countdown-countdown) == 1) //sequence continued
        {
            last_countdown = countdown;
            buf_vehsp[buf_size-countdown-1] = vehsp;
        }
        else //sequence interrupted: clear buffer
        {
            buf_size = 0;
            last_countdown = 0;
        }
    }
    else //clear buffer
    {
        buf_size = 0;
        last_countdown = 0;
    }

    if((countdown == 0) && (buf_size >0) )
    {
        updateVehicleSpeedData(buf_vehsp,buf_size);
        buf_size = 0;
        last_countdown = 0;        
    }

    return true;
}

void *listenForMessages( void *ptr )
{  
    struct sockaddr_in si_me;
    struct sockaddr_in si_other;
    int s;
    socklen_t slen = sizeof(si_other);
    ssize_t readBytes = 0;
    char buf[BUFLEN];
    char msgId[MSGIDLEN];
    int port = PORT;

    DLT_REGISTER_APP("SNSS", "SENSOSRS-SERVICE");
    DLT_REGISTER_CONTEXT(gContext,"SSRV", "Global Context");

    LOG_INFO(gContext,"SensorsService listening on port %d...",port);

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
        //use select to introduce a timeout - alloy shutdown even when no data are received
        fd_set readfs;    /* file descriptor set */
        int    maxfd;     /* maximum file desciptor used */        
        int res;
        struct timeval Timeout;
        /* set timeout value within input loop */
        Timeout.tv_usec = 0;  /* milliseconds */
        Timeout.tv_sec  = 1;  /* seconds */
        FD_SET(s, &readfs);
        maxfd = s+1;
        /* block until input becomes available */
        res = select(maxfd, &readfs, NULL, NULL, &Timeout);

        if (res > 0)
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

            sscanf(buf, "%*[^'$']$%[^',']", msgId);
    
            LOG_DEBUG(gContext,"MsgID:%s",msgId);
            LOG_DEBUG(gContext,"Len:%d",(int)strlen(buf));
            LOG_INFO(gContext,"Data:%s",buf);

            LOG_DEBUG_MSG(gContext,"------------------------------------------------");

            if(strcmp("GVSNSGYRO", msgId) == 0)
            {
                processGVSNSGYRO(buf);
            }
            else if(strcmp("GVSNSWHTK", msgId) == 0)
            {
                processGVSNSWHTK(buf);
            }
            else if(strcmp("GVSNSVEHSP", msgId) == 0)
            {
                processGVSNSVEHSP(buf);
            }
        }
    }

    close(s);

    return EXIT_SUCCESS;
}




