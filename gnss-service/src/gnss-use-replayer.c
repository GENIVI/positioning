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
#include "gnss-init.h"
#include "log.h"

#define BUFLEN 256
#define MSGIDLEN 20
#define PORT 9930

#define MAX_BUF_MSG 16

pthread_t listenerThread;
pthread_mutex_t mutexCb;
pthread_mutex_t mutexData;

bool isRunning = false;
int s = NULL;

void *listenForMessages( void *ptr );

DLT_DECLARE_CONTEXT(gContext);

bool gnssInit()
{
    isRunning = true;
    
    if(pthread_create(&listenerThread, NULL, listenForMessages, NULL) != 0)
    {
        isRunning = false;
    	return false;
    }

    return true;
}

bool gnssDestroy()
{
    isRunning = false;
    
    //shut down the socket
    shutdown(s,2);

    if(listenerThread)
    {
        pthread_join(listenerThread, NULL);
    }

    return true;
}

void gnssGetVersion(int *major, int *minor, int *micro)
{
    if(major)
    {
        *major = GENIVI_GNSS_API_MAJOR;
    }

    if (minor)
    {
        *minor = GENIVI_GNSS_API_MINOR;
    }

    if (micro)
    {
        *micro = GENIVI_GNSS_API_MICRO;
    }
}

//TODO: add support reading of TGNSSSpatial 
//      ?? retain backwards compatibility for old logs?

static bool processGVGNSSAT(char* data, TGNSSSatelliteDetail* pSatelliteDetail)
{
    //parse data like: 061064000,05$GVGNSSAT,061064000,1,18,314.0,22.0,39,0X00,0X1F

    //storage for buffered data
    static TGNSSSatelliteDetail buf_sat[MAX_BUF_MSG];
    static uint16_t buf_size = 0;
    static uint16_t last_countdown = 0;    

    uint64_t timestamp;
    uint16_t countdown;
    TGNSSSatelliteDetail sat = { 0 };
    uint16_t system = 0;
    uint32_t n = 0;

    if(!data || !pSatelliteDetail)
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%llu,%hu$GVGNSSAT,%llu,%hu,%hu,%hu,%hu,%hu,%x,%x", &timestamp, &countdown, &sat.timestamp,&system,&sat.satelliteId,&sat.azimuth,&sat.elevation,&sat.SNR,&sat.statusBits,&sat.validityBits);
    sat.system = system;
    //LOG_DEBUG(gContext,"Decoded: %llu,%hu$GVGNSSAT,%llu,%d,%hu,%hu,%hu,%hu,0X%X,0X%X ", timestamp, countdown, sat.timestamp, sat.system, sat.satelliteId, sat.azimuth, sat.elevation, sat.SNR, sat.statusBits, sat.validityBits);

    if (n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVGNSSAT failed!");
        return false;
    }

    *pSatelliteDetail = sat;

    //buffered data handling
    if (countdown < MAX_BUF_MSG) //enough space in buffer?
    {
        if (buf_size == 0) //a new sequence starts
        {
            buf_size = countdown+1;
            last_countdown = countdown;
            buf_sat[buf_size-countdown-1] = sat;         
        }
        else if ((last_countdown-countdown) == 1) //sequence continued
        {
            last_countdown = countdown;
            buf_sat[buf_size-countdown-1] = sat;                     
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

    if((cbSatelliteDetail != 0) && (countdown == 0) && (buf_size >0) )
    {
        cbSatelliteDetail(buf_sat,buf_size);
        buf_size = 0;
        last_countdown = 0;        
    }

    return true;
}

void *listenForMessages( void *ptr )
{  
    struct sockaddr_in si_me;
    struct sockaddr_in si_other;
    //int s;
    socklen_t slen = sizeof(si_other);
    char buf[BUFLEN];
    char msgId[MSGIDLEN];
    int port = PORT;

    DLT_REGISTER_APP("GNSS", "GNSS-SERVICE");
    DLT_REGISTER_CONTEXT(gContext,"GSRV", "Global Context");

    LOG_DEBUG(gContext,"GNSSService listening on port %d...",port);

    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
        LOG_ERROR_MSG(gContext,"socket() failed!");
        exit(EXIT_FAILURE);
    }

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;

    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(s, (struct sockaddr *)&si_me, (socklen_t)sizeof(si_me)) == -1)
    {
         LOG_ERROR_MSG(gContext,"bind() failed!");
         exit(EXIT_FAILURE);
    }

    while(isRunning == true)
    {
        if(recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *)&si_other, (socklen_t *)&slen) == -1)
        {
            LOG_ERROR_MSG(gContext,"recvfrom() failed!");
            exit(EXIT_FAILURE);
        }

        LOG_DEBUG_MSG(gContext,"------------------------------------------------");

        LOG_DEBUG(gContext,"Received Packet from %s:%d", 
                  inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));

        sscanf(buf, "%*[^'$']$%[^',']", msgId);

        LOG_DEBUG(gContext,"MsgID:%s",msgId);
        LOG_DEBUG(gContext,"Len:%d",strlen(buf));
        LOG_DEBUG(gContext,"Data:%s",buf);

        LOG_DEBUG_MSG(gContext,"------------------------------------------------");

        pthread_mutex_lock(&mutexData);

        if(strcmp("GVGNSSAT", msgId) == 0)
        {
             processGVGNSSAT(buf, &gSatelliteDetail);
        }

        pthread_mutex_unlock(&mutexData);
    }

    close(s);

    return EXIT_SUCCESS;
}




