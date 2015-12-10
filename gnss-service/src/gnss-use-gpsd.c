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
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <assert.h>
#include <math.h>

#include "globals.h"
#include "gnss-init.h"
#include "log.h"
#include "gps.h"

#define PORT "2947"

static struct gps_data_t gpsdata;

pthread_t listenerThread;
bool isRunning = false;

DLT_DECLARE_CONTEXT(gContext);

void *listen( void *ptr );

bool gnssInit()
{
    iGnssInit();
    
    isRunning = true;

    if(pthread_create( &listenerThread, NULL, listen, NULL) != 0)
    {
        isRunning = false;
    	return false;
    }

    return true;
}

bool gnssDestroy()
{
    isRunning = false;

    if(listenerThread)
    {
        pthread_join( listenerThread, NULL);
    }

    iGnssDestroy();

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

bool gnssSetGNSSSystems(uint32_t activate_systems)
{
    return false; //satellite system configuration request not supported by gpsd
}

static EGNSSFixStatus convertToFixStatus(int fixMode)
{
    EGNSSFixStatus status = GNSS_FIX_STATUS_NO;

    switch (fixMode) 
    {
       case MODE_NOT_SEEN:
           status = GNSS_FIX_STATUS_NO; 
           break;
       case MODE_NO_FIX:
           status = GNSS_FIX_STATUS_TIME; 
           break; // check this
       case MODE_2D:
           status = GNSS_FIX_STATUS_2D; 
           break;
       case MODE_3D:
           status = GNSS_FIX_STATUS_3D; 
           break;
       default:
           status = GNSS_FIX_STATUS_NO;    
           LOG_ERROR_MSG(gContext,"should not reach default case");
           break;
    }

    return status;
}

bool extractPosition(struct gps_data_t* pGpsData, TGNSSPosition* pPosition)
{
    static float oldHSpeed = 0;
    static float oldVSpeed = 0;
    static float oldHeading = 0;
    static EGNSSFixStatus oldFixStatus = GNSS_FIX_STATUS_NO;
    static uint16_t oldUsedSatellites = 0;
    static uint16_t oldVisibleSatellites = 0;
    
    bool positionAvailable = false;
    bool velocityAvailable = false;
    bool fixStatusChanged = false;
    bool satellitesChanged = false;

    if(!pGpsData || !pPosition)
    {
        return false;
    }

    pPosition->validityBits = 0;

    pPosition->timestamp = pGpsData->fix.time;

    if( ((pGpsData->set & LATLON_SET) || (pGpsData->set & ALTITUDE_SET)) &&
       (pGpsData->set & MODE_SET) && 
       ((pGpsData->fix.mode == MODE_2D) || (pGpsData->fix.mode == MODE_3D)) )
    {
       positionAvailable = true;
 
       if(pGpsData->set & LATLON_SET)
       {
           pPosition->validityBits |= GNSS_POSITION_LATITUDE_VALID;
           pPosition->latitude = pGpsData->fix.latitude;

           pPosition->validityBits |= GNSS_POSITION_LONGITUDE_VALID;
           pPosition->longitude = pGpsData->fix.longitude;

           LOG_DEBUG(gContext,"Latitude: %lf", pPosition->latitude);
           LOG_DEBUG(gContext,"Longitude: %lf", pPosition->longitude);
       }

       if((pGpsData->set & ALTITUDE_SET) && (pGpsData->fix.mode == MODE_3D))
       {
           pPosition->validityBits |= GNSS_POSITION_ALTITUDEMSL_VALID;
           pPosition->altitudeMSL = (float)pGpsData->fix.altitude;

           LOG_DEBUG(gContext,"Altitude: %lf", pPosition->altitudeMSL);
       }
    }

    if( ((pGpsData->set & SPEED_SET) && (oldHSpeed != (float)pGpsData->fix.speed)) || 
        ((pGpsData->set & CLIMB_SET) && (oldVSpeed != (float)pGpsData->fix.climb)) || 
        ((pGpsData->set & TRACK_SET) && (oldHeading != (float)pGpsData->fix.track)) )
    {
        
        velocityAvailable = true;
        
        if(pGpsData->set & SPEED_SET)
        {
            oldHSpeed = pPosition->hSpeed;
            pPosition->hSpeed = (float)pGpsData->fix.speed; 
            pPosition->validityBits |= GNSS_POSITION_HSPEED_VALID;
            LOG_DEBUG(gContext,"Speed: %lf", pPosition->hSpeed);
        }

        if(pGpsData->set & CLIMB_SET)
        {
            oldVSpeed = pPosition->vSpeed;
            pPosition->vSpeed = (float)pGpsData->fix.climb;
            pPosition->validityBits |= GNSS_POSITION_VSPEED_VALID;
            LOG_DEBUG(gContext,"Climb: %lf", pPosition->vSpeed);
        }
            		
        if(pGpsData->set & TRACK_SET)
        {
            oldHeading = pPosition->heading;
            pPosition->heading = (float)pGpsData->fix.track;
            pPosition->validityBits |= GNSS_POSITION_HEADING_VALID;
            LOG_DEBUG(gContext,"Heading: %lf", pPosition->heading);
        }
    }

    fixStatusChanged = (oldFixStatus != convertToFixStatus(pGpsData->fix.mode));
 
    satellitesChanged = ((oldUsedSatellites != (uint16_t)pGpsData->satellites_used) || 
                         (oldVisibleSatellites != (uint16_t)pGpsData->satellites_visible));
                        
    if(((pGpsData->set & MODE_SET) && fixStatusChanged) ||
       (pGpsData->set & DOP_SET) ||
       ((pGpsData->set & SATELLITE_SET) && satellitesChanged))
    {
        oldFixStatus = pPosition->fixStatus;
        pPosition->fixStatus = convertToFixStatus(pGpsData->fix.mode);
        pPosition->validityBits |= GNSS_POSITION_STAT_VALID;
        LOG_DEBUG(gContext,"FixStatus: %d", (int)pPosition->fixStatus);
        
        //fixTypeBits: hardcoded
        pPosition->fixTypeBits |= GNSS_FIX_TYPE_SINGLE_FREQUENCY;
        pPosition->validityBits |= GNSS_POSITION_TYPE_VALID;
        
        pPosition->pdop = (float)pGpsData->dop.pdop;
        pPosition->validityBits |= GNSS_POSITION_PDOP_VALID;
        LOG_DEBUG(gContext,"pdop: %lf", pPosition->pdop);

        pPosition->hdop = (float)pGpsData->dop.hdop;
        pPosition->validityBits |= GNSS_POSITION_HDOP_VALID;
        LOG_DEBUG(gContext,"hdop: %lf", pPosition->hdop);

        pPosition->vdop = (float)pGpsData->dop.vdop;
        pPosition->validityBits |= GNSS_POSITION_VDOP_VALID;
        LOG_DEBUG(gContext,"vdop: %lf", pPosition->vdop);

        pPosition->sigmaHPosition = (float)pGpsData->fix.epx;
        pPosition->validityBits |= GNSS_POSITION_SHPOS_VALID;
        LOG_DEBUG(gContext,"sigmaHorPosition: %lf", pPosition->sigmaHPosition);

        pPosition->sigmaHSpeed = (float)pGpsData->fix.eps;
        pPosition->validityBits |= GNSS_POSITION_SHSPEED_VALID;
        LOG_DEBUG(gContext,"sigmaHorSpeed: %lf", pPosition->sigmaHSpeed);

        pPosition->sigmaHeading = (float)pGpsData->fix.epd;
        pPosition->validityBits |= GNSS_POSITION_SHEADING_VALID;
        LOG_DEBUG(gContext,"sigmaHeading: %lf", pPosition->sigmaHeading);

        if(pGpsData->satellites_used >= 0)
        {
            oldUsedSatellites = pPosition->usedSatellites;
            pPosition->usedSatellites = (uint16_t)pGpsData->satellites_used;      
            pPosition->validityBits |= GNSS_POSITION_USAT_VALID;
            LOG_DEBUG(gContext,"Used Satellites: %d", pPosition->usedSatellites);
        }
        
        if(pGpsData->satellites_visible >= 0)
        {
            oldVisibleSatellites = pPosition->visibleSatellites; 
            pPosition->visibleSatellites = (uint16_t)pGpsData->satellites_visible;
            pPosition->validityBits |= GNSS_POSITION_VSAT_VALID;
            LOG_DEBUG(gContext,"Visible Satellites: %d", pPosition->visibleSatellites);
        }
    }

    //hardcoded values for standard GPS receiver
    pPosition->fixTypeBits = GNSS_FIX_TYPE_SINGLE_FREQUENCY;
    pPosition->validityBits |= GNSS_POSITION_TYPE_VALID;
    pPosition->activated_systems = GNSS_SYSTEM_GPS;
    pPosition->validityBits |= GNSS_POSITION_ASYS_VALID;
    pPosition->used_systems = GNSS_SYSTEM_GPS;
    pPosition->validityBits |= GNSS_POSITION_USYS_VALID;
   
    if (positionAvailable || velocityAvailable || fixStatusChanged || satellitesChanged)
    {
        updateGNSSPosition(pPosition,1);
    }
    
    return true;
}

bool extractTime(struct gps_data_t* pGpsData, TGNSSTime* pTime)
{
    static timestamp_t oldTime = 0;

    if(!pGpsData || !pTime)
    {
        return false;
    }
   
    if ((pGpsData->set & TIME_SET) && (oldTime != pGpsData->fix.time))
   {
        char month [12] [4] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        
        pTime->validityBits = 0;
        pTime->timestamp = pGpsData->fix.time;

        if(pGpsData->set & TIME_SET)
        {
            oldTime = pGpsData->fix.time;
            time_t unix_sec = pGpsData->fix.time;
            struct tm * ptm = gmtime (&unix_sec);
            pTime->year = ptm->tm_year+1900;
            pTime->month = ptm->tm_mon;
            pTime->day = ptm->tm_mday;
            pTime->hour = ptm->tm_hour;
            pTime->minute = ptm->tm_min;
            pTime->second = ptm->tm_sec;
            pTime->ms = 1000*fmod(pGpsData->fix.time, 1.0);
            pTime->validityBits |= GNSS_TIME_TIME_VALID | GNSS_TIME_DATE_VALID;
            LOG_DEBUG(gContext,"UTC: %04d-%s-%02d %02d:%02d:%02d", pTime->year, month[pTime->month%12], pTime->day, pTime->hour, pTime->minute, pTime->second);
        }

        updateGNSSTime(pTime,1);
    }
    return true;
}


void *listen( void *ptr )
{  
    char* server = "localhost";
    char* device = 0;

    unsigned int flags = WATCH_ENABLE;

    DLT_REGISTER_APP("GNSS","GNSS-SERVICE");
    DLT_REGISTER_CONTEXT(gContext,"GSRV","Global Context");

    LOG_INFO(gContext,"GNSSService listening port %s...",PORT);
  
    device = getenv("GNSS_DEVICE");
   
    LOG_DEBUG(gContext,"server: %s, port: %s",server,PORT);

    if(gps_open("localhost", PORT, &gpsdata) != 0)
    {
        LOG_ERROR(gContext,"no gpsd running or network error: %d, %s",errno, gps_errstr(errno));
        exit(EXIT_FAILURE);
    }

    flags |= WATCH_JSON; 

    if(device != 0)
    {
        flags |= WATCH_DEVICE;
        LOG_DEBUG(gContext,"device: %s",device);
    }

    (void)gps_stream(&gpsdata, flags, device);

    while(isRunning == true) 
    {
        if(!gps_waiting(&gpsdata, 15000000)) 
        {
            LOG_ERROR_MSG(gContext,"error while waiting");
            break;
        } 
        else 
        {
            if(gps_read(&gpsdata))
            {
                TGNSSPosition position = = { 0 };
                if(!extractPosition(&gpsdata,&position))
                {
                    LOG_ERROR_MSG(gContext,"error extracting position data");
                }
                TGNSSTime time = { 0 };
                if(!extractTime(&gpsdata,&time))
                {
                    LOG_ERROR_MSG(gContext,"error extracting Time");
                }
            }
        }
    }

    (void)gps_close(&gpsdata);

    return EXIT_SUCCESS;
}




