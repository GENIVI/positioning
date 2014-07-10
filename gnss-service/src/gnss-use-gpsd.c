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

#include "globals.h"
#include "gnss.h"
#include "gnss-simple.h"
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

bool extractAccuracy(struct gps_data_t* pGpsData, TGNSSAccuracy* pAccuracy)
{
    static EGNSSFixStatus oldFixStatus = GNSS_FIX_STATUS_NO;
    static uint16_t oldUsedSatellites = 0;
    static uint16_t oldVisibleSatellites = 0;
    bool fixStatusChanged = false;
    bool satellitesChanged = false;

    if(!pGpsData || !pAccuracy)
    {
        return false;
    }

    pAccuracy->validityBits = 0;

    pAccuracy->timestamp = pGpsData->fix.time;

    fixStatusChanged = (oldFixStatus != convertToFixStatus(pGpsData->fix.mode));
 
    satellitesChanged = ((oldUsedSatellites != (uint16_t)pGpsData->satellites_used) || 
                         (oldVisibleSatellites != (uint16_t)pGpsData->satellites_visible));
                        
    if(((pGpsData->set & MODE_SET) && fixStatusChanged) ||
       (pGpsData->set & DOP_SET) ||
       ((pGpsData->set & SATELLITE_SET) && satellitesChanged))
    {
        oldFixStatus = pAccuracy->fixStatus;
        pAccuracy->fixStatus = convertToFixStatus(pGpsData->fix.mode);
        pAccuracy->validityBits |= GNSS_ACCURACY_STAT_VALID;
        LOG_DEBUG(gContext,"FixStatus: %d", (int)pAccuracy->fixStatus);

        pAccuracy->pdop = (float)pGpsData->dop.pdop;
        pAccuracy->validityBits |= GNSS_ACCURACY_PDOP_VALID;
        LOG_DEBUG(gContext,"pdop: %lf", pAccuracy->pdop);

        pAccuracy->hdop = (float)pGpsData->dop.hdop;
        pAccuracy->validityBits |= GNSS_ACCURACY_HDOP_VALID;
        LOG_DEBUG(gContext,"hdop: %lf", pAccuracy->hdop);

        pAccuracy->vdop = (float)pGpsData->dop.vdop;
        pAccuracy->validityBits |= GNSS_ACCURACY_VDOP_VALID;
        LOG_DEBUG(gContext,"vdop: %lf", pAccuracy->vdop);

        if(pGpsData->satellites_used >= 0)
        {
            oldUsedSatellites = pAccuracy->usedSatellites;
            pAccuracy->usedSatellites = (uint16_t)pGpsData->satellites_used;      
            pAccuracy->validityBits |= GNSS_ACCURACY_USAT_VALID;
            LOG_DEBUG(gContext,"Used Satellites: %d", pAccuracy->usedSatellites);
        }
        
        if(pGpsData->satellites_visible >= 0)
        {
            oldVisibleSatellites = pAccuracy->visibleSatellites; 
            pAccuracy->visibleSatellites = (uint16_t)pGpsData->satellites_visible;
            pAccuracy->validityBits |= GNSS_ACCURACY_VSAT_VALID;
            LOG_DEBUG(gContext,"Visible Satellites: %d", pAccuracy->visibleSatellites);
        }

        if(cbAccuracy != 0)
        {
            cbAccuracy(pAccuracy,1);
        }
    }
    return true;
}

bool extractPosition(struct gps_data_t* pGpsData, TGNSSPosition* pPosition)
{
   if(!pGpsData || !pPosition)
   {
        return false;
   }

   if( ((pGpsData->set & LATLON_SET) || (pGpsData->set & ALTITUDE_SET)) &&
       (pGpsData->set & MODE_SET) && 
       ((pGpsData->fix.mode == MODE_2D) || (pGpsData->fix.mode == MODE_3D)) )
   {
       pPosition->validityBits = 0;
 
       pPosition->timestamp = pGpsData->fix.time;

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
           pPosition->validityBits |= GNSS_POSITION_ALTITUDE_VALID;
           pPosition->altitude = (float)pGpsData->fix.altitude;

           LOG_DEBUG(gContext,"Altitude: %lf", pPosition->altitude);
       }

       if (cbPosition != 0)
       {
           cbPosition(pPosition,1);
       }
    }
    return true;
}

bool extractCourse(struct gps_data_t* pGpsData, TGNSSCourse* pCourse)
{
   static float oldSpeed = 0;
   static float oldClimb = 0;
   static float oldHeading = 0;

   if(!pGpsData || !pCourse)
   {
        return false;
   }
   
   pCourse->validityBits = 0;

   if( ((pGpsData->set & SPEED_SET) && (oldSpeed != (float)pGpsData->fix.speed)) || 
       ((pGpsData->set & CLIMB_SET) && (oldClimb != (float)pGpsData->fix.climb)) || 
       ((pGpsData->set & TRACK_SET) && (oldHeading != (float)pGpsData->fix.track)) )
   {
       if(pGpsData->set & SPEED_SET)
       {
           oldSpeed = pCourse->speed;
           pCourse->speed = (float)pGpsData->fix.speed; 
           pCourse->validityBits |= GNSS_COURSE_SPEED_VALID;
           LOG_DEBUG(gContext,"Speed: %lf", pCourse->speed);
       }

       if(pGpsData->set & CLIMB_SET)
       {
           oldClimb = pCourse->climb;
           pCourse->climb = (float)pGpsData->fix.climb;
           pCourse->validityBits |= GNSS_COURSE_CLIMB_VALID;
           LOG_DEBUG(gContext,"Climb: %lf", pCourse->climb);
       }
            		
       if(pGpsData->set & TRACK_SET)
       {
           oldHeading = pCourse->heading;
           pCourse->heading = (float)pGpsData->fix.track;
           pCourse->validityBits |= GNSS_COURSE_HEADING_VALID;
           LOG_DEBUG(gContext,"Heading: %lf", pCourse->heading);
       }
            	
       if(cbCourse != 0)
       {
           cbCourse(pCourse,1);
       }
    }
    return true;
}


bool extractLocation(struct gps_data_t* pGpsData, TGNSSLocation* pLocation)
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

    if(!pGpsData || !pLocation)
    {
        return false;
    }

    pLocation->validityBits = 0;

    pLocation->timestamp = pGpsData->fix.time;

    if( ((pGpsData->set & LATLON_SET) || (pGpsData->set & ALTITUDE_SET)) &&
       (pGpsData->set & MODE_SET) && 
       ((pGpsData->fix.mode == MODE_2D) || (pGpsData->fix.mode == MODE_3D)) )
    {
       positionAvailable = true;
 
       if(pGpsData->set & LATLON_SET)
       {
           pLocation->validityBits |= GNSS_LOCATION_LATITUDE_VALID;
           pLocation->latitude = pGpsData->fix.latitude;

           pLocation->validityBits |= GNSS_LOCATION_LONGITUDE_VALID;
           pLocation->longitude = pGpsData->fix.longitude;

           LOG_DEBUG(gContext,"Latitude: %lf", pLocation->latitude);
           LOG_DEBUG(gContext,"Longitude: %lf", pLocation->longitude);
       }

       if((pGpsData->set & ALTITUDE_SET) && (pGpsData->fix.mode == MODE_3D))
       {
           pLocation->validityBits |= GNSS_LOCATION_ALTITUDEMSL_VALID;
           pLocation->altitudeMSL = (float)pGpsData->fix.altitude;

           LOG_DEBUG(gContext,"Altitude: %lf", pLocation->altitudeMSL);
       }
    }

    if( ((pGpsData->set & SPEED_SET) && (oldHSpeed != (float)pGpsData->fix.speed)) || 
        ((pGpsData->set & CLIMB_SET) && (oldVSpeed != (float)pGpsData->fix.climb)) || 
        ((pGpsData->set & TRACK_SET) && (oldHeading != (float)pGpsData->fix.track)) )
    {
        
        velocityAvailable = true;
        
        if(pGpsData->set & SPEED_SET)
        {
            oldHSpeed = pLocation->hSpeed;
            pLocation->hSpeed = (float)pGpsData->fix.speed; 
            pLocation->validityBits |= GNSS_LOCATION_HSPEED_VALID;
            LOG_DEBUG(gContext,"Speed: %lf", pLocation->hSpeed);
        }

        if(pGpsData->set & CLIMB_SET)
        {
            oldVSpeed = pLocation->vSpeed;
            pLocation->vSpeed = (float)pGpsData->fix.climb;
            pLocation->validityBits |= GNSS_LOCATION_VSPEED_VALID;
            LOG_DEBUG(gContext,"Climb: %lf", pLocation->vSpeed);
        }
            		
        if(pGpsData->set & TRACK_SET)
        {
            oldHeading = pLocation->heading;
            pLocation->heading = (float)pGpsData->fix.track;
            pLocation->validityBits |= GNSS_LOCATION_HEADING_VALID;
            LOG_DEBUG(gContext,"Heading: %lf", pLocation->heading);
        }
    }

    fixStatusChanged = (oldFixStatus != convertToFixStatus(pGpsData->fix.mode));
 
    satellitesChanged = ((oldUsedSatellites != (uint16_t)pGpsData->satellites_used) || 
                         (oldVisibleSatellites != (uint16_t)pGpsData->satellites_visible));
                        
    if(((pGpsData->set & MODE_SET) && fixStatusChanged) ||
       (pGpsData->set & DOP_SET) ||
       ((pGpsData->set & SATELLITE_SET) && satellitesChanged))
    {
        oldFixStatus = pLocation->fixStatus;
        pLocation->fixStatus = convertToFixStatus(pGpsData->fix.mode);
        pLocation->validityBits |= GNSS_LOCATION_STAT_VALID;
        LOG_DEBUG(gContext,"FixStatus: %d", (int)pLocation->fixStatus);
        
        //fixTypeBits: hardcoded
        pLocation->fixTypeBits |= GNSS_FIX_TYPE_SINGLE_FREQUENCY;
        pLocation->validityBits |= GNSS_LOCATION_TYPE_VALID;
        
        pLocation->pdop = (float)pGpsData->dop.pdop;
        pLocation->validityBits |= GNSS_LOCATION_PDOP_VALID;
        LOG_DEBUG(gContext,"pdop: %lf", pLocation->pdop);

        pLocation->hdop = (float)pGpsData->dop.hdop;
        pLocation->validityBits |= GNSS_LOCATION_HDOP_VALID;
        LOG_DEBUG(gContext,"hdop: %lf", pLocation->hdop);

        pLocation->vdop = (float)pGpsData->dop.vdop;
        pLocation->validityBits |= GNSS_LOCATION_VDOP_VALID;
        LOG_DEBUG(gContext,"vdop: %lf", pLocation->vdop);

        pLocation->sigmaHPosition = (float)pGpsData->fix.epx;
        pLocation->validityBits |= GNSS_LOCATION_SHPOS_VALID;
        LOG_DEBUG(gContext,"sigmaHorPosition: %lf", pLocation->sigmaHPosition);

        pLocation->sigmaHSpeed = (float)pGpsData->fix.eps;
        pLocation->validityBits |= GNSS_LOCATION_SHSPEED_VALID;
        LOG_DEBUG(gContext,"sigmaHorSpeed: %lf", pLocation->sigmaHSpeed);

        pLocation->sigmaHeading = (float)pGpsData->fix.epd;
        pLocation->validityBits |= GNSS_LOCATION_SHEADING_VALID;
        LOG_DEBUG(gContext,"sigmaHeading: %lf", pLocation->sigmaHeading);

        if(pGpsData->satellites_used >= 0)
        {
            oldUsedSatellites = pLocation->usedSatellites;
            pLocation->usedSatellites = (uint16_t)pGpsData->satellites_used;      
            pLocation->validityBits |= GNSS_LOCATION_USAT_VALID;
            LOG_DEBUG(gContext,"Used Satellites: %d", pLocation->usedSatellites);
        }
        
        if(pGpsData->satellites_visible >= 0)
        {
            oldVisibleSatellites = pLocation->visibleSatellites; 
            pLocation->visibleSatellites = (uint16_t)pGpsData->satellites_visible;
            pLocation->validityBits |= GNSS_LOCATION_VSAT_VALID;
            LOG_DEBUG(gContext,"Visible Satellites: %d", pLocation->visibleSatellites);
        }
    }
   
    if (positionAvailable || velocityAvailable || fixStatusChanged || satellitesChanged)
    {
        if(cbLocation != 0)
        {
            cbLocation(pLocation,1);
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

                if(!extractPosition(&gpsdata,&gPosition))
                {
                    LOG_ERROR_MSG(gContext,"error extracting position");
                }

                if(!extractCourse(&gpsdata,&gCourse))
                {
                    LOG_ERROR_MSG(gContext,"error extracting course");
                }
      	
                if(!extractAccuracy(&gpsdata,&gAccuracy))
                {
                    LOG_ERROR_MSG(gContext,"error extracting accuracy");
                }

                if(!extractLocation(&gpsdata,&gLocation))
                {
                    LOG_ERROR_MSG(gContext,"error extracting positionVelocityAccuracy");
                }

                pthread_mutex_unlock(&mutexData);
            }
        }
    }

    (void)gps_close(&gpsdata);

    return EXIT_SUCCESS;
}




