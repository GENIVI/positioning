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

bool isRunning = false;
int s = 0;

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

//backward compatible processing of GVGNSAC to the new TGNSSPosition
bool processGVGNSAC(char* data)
{
    //parse data like: 061064000,0$GVGNSP,061064000,49.02657,12.06527,336.70000,0X07

    //storage for buffered data
    static TGNSSPosition buf_acc[MAX_BUF_MSG];
    static uint16_t buf_size = 0;
    static uint16_t last_countdown = 0;

    uint64_t timestamp;
    uint16_t countdown;
    TGNSSPosition pos = { 0 };
    uint16_t fixStatus;
    uint32_t GVGNSAC_validityBits;
    uint32_t n = 0;

    if(!data)
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%llu,%hu$GVGNSAC,%llu,%f,%f,%f,%hu,%hu,%hu,%f,%f,%f,%hu,%x,%x",
         &timestamp, &countdown, &pos.timestamp,
         &pos.pdop, &pos.hdop, &pos.vdop,
         &pos.usedSatellites, &pos.trackedSatellites, &pos.visibleSatellites,
         &pos.sigmaHPosition, &pos.sigmaHPosition, &pos.sigmaAltitude,
         &fixStatus, &pos.fixTypeBits, &GVGNSAC_validityBits);
    //fix status: order in enum has changed
    if (fixStatus == 0) { pos.fixStatus = GNSS_FIX_STATUS_NO; }
    if (fixStatus == 1) { pos.fixStatus = GNSS_FIX_STATUS_2D; }
    if (fixStatus == 2) { pos.fixStatus = GNSS_FIX_STATUS_3D; }
    if (fixStatus == 3) { pos.fixStatus = GNSS_FIX_STATUS_TIME; }
    //map the old validity bits to the new validity bits
    pos.validityBits = 0;
    if (GVGNSAC_validityBits&0x00000001) { pos.validityBits |= GNSS_POSITION_PDOP_VALID; }
    if (GVGNSAC_validityBits&0x00000002) { pos.validityBits |= GNSS_POSITION_HDOP_VALID; }
    if (GVGNSAC_validityBits&0x00000004) { pos.validityBits |= GNSS_POSITION_VDOP_VALID; }
    if (GVGNSAC_validityBits&0x00000008) { pos.validityBits |= GNSS_POSITION_USAT_VALID; }
    if (GVGNSAC_validityBits&0x00000010) { pos.validityBits |= GNSS_POSITION_TSAT_VALID; }
    if (GVGNSAC_validityBits&0x00000020) { pos.validityBits |= GNSS_POSITION_VSAT_VALID; }
    if (GVGNSAC_validityBits&0x00000040) { pos.validityBits |= GNSS_POSITION_SHPOS_VALID; }
    if (GVGNSAC_validityBits&0x00000100) { pos.validityBits |= GNSS_POSITION_SALT_VALID; }
    if (GVGNSAC_validityBits&0x00000200) { pos.validityBits |= GNSS_POSITION_STAT_VALID; }
    if (GVGNSAC_validityBits&0x00000400) { pos.validityBits |= GNSS_POSITION_TYPE_VALID; }

    if (n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVGNSAC failed!");
        return false;
    }

    //global Position: update the changed fields, retain the existing fields from other callbacks
    TGNSSPosition upd_pos;
    if (gnssGetPosition(&upd_pos))
    {
        upd_pos.timestamp = pos.timestamp;
        upd_pos.pdop = pos.pdop;
        upd_pos.hdop = pos.hdop;
        upd_pos.vdop = pos.vdop;
        upd_pos.usedSatellites = pos.usedSatellites;
        upd_pos.trackedSatellites = pos.trackedSatellites;
        upd_pos.visibleSatellites = pos.visibleSatellites;
        upd_pos.sigmaHPosition = pos.sigmaHPosition;
        upd_pos.sigmaAltitude = pos.sigmaAltitude;
        upd_pos.fixStatus = pos.fixStatus;
        upd_pos.fixTypeBits = pos.fixTypeBits;
        upd_pos.validityBits |= pos.validityBits;
        pos = upd_pos;
    }

    //buffered data handling
    if (countdown < MAX_BUF_MSG) //enough space in buffer?
    {
        if (buf_size == 0) //a new sequence starts
        {
            buf_size = countdown+1;
            last_countdown = countdown;
            buf_acc[buf_size-countdown-1] = pos;
        }
        else if ((last_countdown-countdown) == 1) //sequence continued
        {
            last_countdown = countdown;
            buf_acc[buf_size-countdown-1] = pos;
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
        updateGNSSPosition(buf_acc,buf_size);
        buf_size = 0;
        last_countdown = 0;
    }

    return true;
}

//backward compatible processing of GVGNSP to the new TGNSSPosition
static bool processGVGNSP(char* data)
{
    //parse data like: 061064000,0$GVGNSP,061064000,49.02657,12.06527,336.70000,0X07

    //storage for buffered data
    static TGNSSPosition buf_pos[MAX_BUF_MSG];
    static uint16_t buf_size = 0;
    static uint16_t last_countdown = 0;

    uint64_t timestamp;
    uint16_t countdown;
    TGNSSPosition pos = { 0 };
    uint32_t GVGNSP_validityBits;
    uint32_t n = 0;

    if(!data)
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%llu,%hu$GVGNSP,%llu,%lf,%lf,%f,%x", &timestamp, &countdown, &pos.timestamp, &pos.latitude, &pos.longitude, &pos.altitudeMSL, &GVGNSP_validityBits);

    //map the old validity bits to the new validity bits
    pos.validityBits = 0;
    if (GVGNSP_validityBits&0x00000001) { pos.validityBits |= GNSS_POSITION_LATITUDE_VALID; }
    if (GVGNSP_validityBits&0x00000002) { pos.validityBits |= GNSS_POSITION_LONGITUDE_VALID; }
    if (GVGNSP_validityBits&0x00000004) { pos.validityBits |= GNSS_POSITION_ALTITUDEMSL_VALID; }

    if (n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVGNSP failed!");
        return false;
    }

    //buffered data handling
    if (countdown < MAX_BUF_MSG) //enough space in buffer?
    {
        if (buf_size == 0) //a new sequence starts
        {
            buf_size = countdown+1;
            last_countdown = countdown;
            buf_pos[buf_size-countdown-1] = pos;
        }
        else if ((last_countdown-countdown) == 1) //sequence continued
        {
            last_countdown = countdown;
            buf_pos[buf_size-countdown-1] = pos;
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
        updateGNSSPosition(buf_pos,buf_size);
        buf_size = 0;
        last_countdown = 0;
    }

    return true;
}

//backward compatible processing of GVGNSC to the new TGNSSPosition
static bool processGVGNSC(char* data)
{
    //parse data like: 061064000,0$GVGNSC,061064000,0.00,0,131.90000,0X05

    //storage for buffered data
    static TGNSSPosition buf_course[MAX_BUF_MSG];
    static uint16_t buf_size = 0;
    static uint16_t last_countdown = 0;

    uint64_t timestamp;
    uint16_t countdown;
    TGNSSPosition pos = { 0 };
    uint32_t GVGNSC_validityBits;
    uint32_t n = 0;

    if(!data)
    {
        LOG_ERROR_MSG(gContext,"wrong parameter!");
        return false;
    }

    n = sscanf(data, "%llu,%hu$GVGNSC,%llu,%f,%f,%f,%x", &timestamp, &countdown, &pos.timestamp, &pos.hSpeed, &pos.vSpeed, &pos.heading, &GVGNSC_validityBits);

    //map the old validity bits to the new validity bits
    pos.validityBits = 0;
    if (GVGNSC_validityBits&0x00000001) { pos.validityBits |= GNSS_POSITION_HSPEED_VALID; }
    if (GVGNSC_validityBits&0x00000002) { pos.validityBits |= GNSS_POSITION_VSPEED_VALID; }
    if (GVGNSC_validityBits&0x00000004) { pos.validityBits |= GNSS_POSITION_HEADING_VALID; }

    if (n <= 0)
    {
        LOG_ERROR_MSG(gContext,"replayer: processGVGNSC failed!");
        return false;
    }

    //global Position: update the changed fields, retain the existing fields from other callbacks
    TGNSSPosition upd_pos;
    if (gnssGetPosition(&upd_pos))
    {    
        upd_pos.timestamp = pos.timestamp;
        upd_pos.hSpeed = pos.hSpeed;
        upd_pos.vSpeed = pos.vSpeed;
        upd_pos.heading = pos.heading;
        upd_pos.validityBits |= pos.validityBits;
        pos = upd_pos;        
    }

    //buffered data handling
    if (countdown < MAX_BUF_MSG) //enough space in buffer?
    {
        if (buf_size == 0) //a new sequence starts
        {
            buf_size = countdown+1;
            last_countdown = countdown;
            buf_course[buf_size-countdown-1] = pos;
        }
        else if ((last_countdown-countdown) == 1) //sequence continued
        {
            last_countdown = countdown;
            buf_course[buf_size-countdown-1] = pos;
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
        updateGNSSPosition(buf_course,buf_size);
        buf_size = 0;
        last_countdown = 0;
    }

    return true;
}

static bool processGVGNSSAT(char* data)
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

    if(!data)
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

    if((countdown == 0) && (buf_size >0) )
    {
        updateGNSSSatelliteDetail(buf_sat,buf_size);
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

        if(strcmp("GVGNSP", msgId) == 0)
        {
             processGVGNSP(buf);
        }
        else if(strcmp("GVGNSC", msgId) == 0)
        {
             processGVGNSC(buf);
        }
        else if(strcmp("GVGNSAC", msgId) == 0)
        {
             processGVGNSAC(buf);
        }
        else if(strcmp("GVGNSSAT", msgId) == 0)
        {
             processGVGNSSAT(buf);
        }

    }

    close(s);

    return EXIT_SUCCESS;
}




