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
#include "gnss.h"
#include "log.h"
#include "gps.h"

#define PORT "2947"

static struct gps_data_t gpsdata;

pthread_t listenerThread;
pthread_mutex_t mutexCb;
pthread_mutex_t mutexData;
bool isRunning = false;

DLT_DECLARE_CONTEXT(gContext);

void *listen( void *ptr );

bool gnssInit()
{
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

bool extractSpatial(struct gps_data_t* pGpsData, TGNSSSpatial* pSpatial)
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

    if(!pGpsData || !pSpatial)
    {
        return false;
    }

    pSpatial->validityBits = 0;

    pSpatial->timestamp = pGpsData->fix.time;

    if( ((pGpsData->set & LATLON_SET) || (pGpsData->set & ALTITUDE_SET)) &&
       (pGpsData->set & MODE_SET) && 
       ((pGpsData->fix.mode == MODE_2D) || (pGpsData->fix.mode == MODE_3D)) )
    {
       positionAvailable = true;
 
       if(pGpsData->set & LATLON_SET)
       {
           pSpatial->validityBits |= GNSS_SPATIAL_LATITUDE_VALID;
           pSpatial->latitude = pGpsData->fix.latitude;

           pSpatial->validityBits |= GNSS_SPATIAL_LONGITUDE_VALID;
           pSpatial->longitude = pGpsData->fix.longitude;

           LOG_DEBUG(gContext,"Latitude: %lf", pSpatial->latitude);
           LOG_DEBUG(gContext,"Longitude: %lf", pSpatial->longitude);
       }

       if((pGpsData->set & ALTITUDE_SET) && (pGpsData->fix.mode == MODE_3D))
       {
           pSpatial->validityBits |= GNSS_SPATIAL_ALTITUDEMSL_VALID;
           pSpatial->altitudeMSL = (float)pGpsData->fix.altitude;

           LOG_DEBUG(gContext,"Altitude: %lf", pSpatial->altitudeMSL);
       }
    }

    if( ((pGpsData->set & SPEED_SET) && (oldHSpeed != (float)pGpsData->fix.speed)) || 
        ((pGpsData->set & CLIMB_SET) && (oldVSpeed != (float)pGpsData->fix.climb)) || 
        ((pGpsData->set & TRACK_SET) && (oldHeading != (float)pGpsData->fix.track)) )
    {
        
        velocityAvailable = true;
        
        if(pGpsData->set & SPEED_SET)
        {
            oldHSpeed = pSpatial->hSpeed;
            pSpatial->hSpeed = (float)pGpsData->fix.speed; 
            pSpatial->validityBits |= GNSS_SPATIAL_HSPEED_VALID;
            LOG_DEBUG(gContext,"Speed: %lf", pSpatial->hSpeed);
        }

        if(pGpsData->set & CLIMB_SET)
        {
            oldVSpeed = pSpatial->vSpeed;
            pSpatial->vSpeed = (float)pGpsData->fix.climb;
            pSpatial->validityBits |= GNSS_SPATIAL_VSPEED_VALID;
            LOG_DEBUG(gContext,"Climb: %lf", pSpatial->vSpeed);
        }
            		
        if(pGpsData->set & TRACK_SET)
        {
            oldHeading = pSpatial->heading;
            pSpatial->heading = (float)pGpsData->fix.track;
            pSpatial->validityBits |= GNSS_SPATIAL_HEADING_VALID;
            LOG_DEBUG(gContext,"Heading: %lf", pSpatial->heading);
        }
    }

    fixStatusChanged = (oldFixStatus != convertToFixStatus(pGpsData->fix.mode));
 
    satellitesChanged = ((oldUsedSatellites != (uint16_t)pGpsData->satellites_used) || 
                         (oldVisibleSatellites != (uint16_t)pGpsData->satellites_visible));
                        
    if(((pGpsData->set & MODE_SET) && fixStatusChanged) ||
       (pGpsData->set & DOP_SET) ||
       ((pGpsData->set & SATELLITE_SET) && satellitesChanged))
    {
        oldFixStatus = pSpatial->fixStatus;
        pSpatial->fixStatus = convertToFixStatus(pGpsData->fix.mode);
        pSpatial->validityBits |= GNSS_SPATIAL_STAT_VALID;
        LOG_DEBUG(gContext,"FixStatus: %d", (int)pSpatial->fixStatus);
        
        //fixTypeBits: hardcoded
        pSpatial->fixTypeBits |= GNSS_FIX_TYPE_SINGLE_FREQUENCY;
        pSpatial->validityBits |= GNSS_SPATIAL_TYPE_VALID;
        
        pSpatial->pdop = (float)pGpsData->dop.pdop;
        pSpatial->validityBits |= GNSS_SPATIAL_PDOP_VALID;
        LOG_DEBUG(gContext,"pdop: %lf", pSpatial->pdop);

        pSpatial->hdop = (float)pGpsData->dop.hdop;
        pSpatial->validityBits |= GNSS_SPATIAL_HDOP_VALID;
        LOG_DEBUG(gContext,"hdop: %lf", pSpatial->hdop);

        pSpatial->vdop = (float)pGpsData->dop.vdop;
        pSpatial->validityBits |= GNSS_SPATIAL_VDOP_VALID;
        LOG_DEBUG(gContext,"vdop: %lf", pSpatial->vdop);

        pSpatial->sigmaHPosition = (float)pGpsData->fix.epx;
        pSpatial->validityBits |= GNSS_SPATIAL_SHPOS_VALID;
        LOG_DEBUG(gContext,"sigmaHorPosition: %lf", pSpatial->sigmaHPosition);

        pSpatial->sigmaHSpeed = (float)pGpsData->fix.eps;
        pSpatial->validityBits |= GNSS_SPATIAL_SHSPEED_VALID;
        LOG_DEBUG(gContext,"sigmaHorSpeed: %lf", pSpatial->sigmaHSpeed);

        pSpatial->sigmaHeading = (float)pGpsData->fix.epd;
        pSpatial->validityBits |= GNSS_SPATIAL_SHEADING_VALID;
        LOG_DEBUG(gContext,"sigmaHeading: %lf", pSpatial->sigmaHeading);

        if(pGpsData->satellites_used >= 0)
        {
            oldUsedSatellites = pSpatial->usedSatellites;
            pSpatial->usedSatellites = (uint16_t)pGpsData->satellites_used;      
            pSpatial->validityBits |= GNSS_SPATIAL_USAT_VALID;
            LOG_DEBUG(gContext,"Used Satellites: %d", pSpatial->usedSatellites);
        }
        
        if(pGpsData->satellites_visible >= 0)
        {
            oldVisibleSatellites = pSpatial->visibleSatellites; 
            pSpatial->visibleSatellites = (uint16_t)pGpsData->satellites_visible;
            pSpatial->validityBits |= GNSS_SPATIAL_VSAT_VALID;
            LOG_DEBUG(gContext,"Visible Satellites: %d", pSpatial->visibleSatellites);
        }
    }
   
    if (positionAvailable || velocityAvailable || fixStatusChanged || satellitesChanged)
    {
        if(cbSpatial != 0)
        {
            cbSpatial(pSpatial,1);
        }
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

        if(cbTime != 0)
        {
            cbTime(pTime,1);
        }
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
                pthread_mutex_lock(&mutexData);


                if(!extractSpatial(&gpsdata,&gSpatial))
                {
                    LOG_ERROR_MSG(gContext,"error extracting spatial data");
                }

                if(!extractTime(&gpsdata,&gTime))
                {
                    LOG_ERROR_MSG(gContext,"error extracting Time");
                }


                pthread_mutex_unlock(&mutexData);
            }
        }
    }

    (void)gps_close(&gpsdata);

    return EXIT_SUCCESS;
}




