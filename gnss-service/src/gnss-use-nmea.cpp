/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \brief GNSS service implementation based on a simple NMEA parser
*        Direct NMEA parsing may allow finer control than using gpsd
*        The following #defines must be set during compilation
*        GNSS_DEVICE    device at which the GPS receiver is attached, 
*                       e.g. "/dev/ttyACM0"
*        GNSS_BAUDRATE  baudrate constant from <asm/termbits.h>
*                       ( included via <termios.h> )
*                       e.g. B38400
*        
*
* \author Helmut Schmidt <https://github.com/huirad>
*
* \copyright Copyright (C) 2009, Helmut Schmidt
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

//provided interface
#include "gnss-init.h"
//GNSS data types
#include "gnss.h"
//helper functions to dispatch the receeived data
#include "globals.h"
//log/trace macros
#include "log.h"
//standard c library functions
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//the NMEA parser
#include "hnmea.h"

DLT_DECLARE_CONTEXT(gContext);

/**
 * INTERNAL FUNCTIONS
 * 
 * 
 */


/** Flag to terminate NMEA reader thread */
volatile int g_GNSS_NMEA_loop=1; 


/**
 * Provide a system timestamp in milliseconds.
 * @return system timestamp in milliseconds
 */
static uint64_t gnss_get_timestamp()
{
  struct timespec time_value;
  if (clock_gettime(CLOCK_MONOTONIC, &time_value) != -1)
  {
    return (time_value.tv_sec*1000 + time_value.tv_nsec/1000000);
  }
  else
  {
    return 0xFFFFFFFFFFFFFFFF;
  }
}


/**
 * Convert GPS data from NMEA parser to TGNSSPosition struct
 * @param gps_data [IN] GPS data decoded from NMEA parser
 * @param timestamp [IN] timestamp in milliseconds
 * @param gnss_pos [OUT] TGNSSPosition struct which will be filled with gps_data and timestamp
 * @return conversion has been successful
 */
bool extractPosition(const GPS_DATA& gps_data, uint64_t timestamp, TGNSSPosition& gnss_pos)
{
    gnss_pos.validityBits = 0;    
    
    gnss_pos.timestamp = timestamp;
    if (gps_data.valid & GPS_DATA_LAT) 
    {
        gnss_pos.latitude = gps_data.lat;
        gnss_pos.validityBits |= GNSS_POSITION_LATITUDE_VALID; 
    }
    if (gps_data.valid & GPS_DATA_LON) 
    {
        gnss_pos.longitude = gps_data.lon;    
        gnss_pos.validityBits |= GNSS_POSITION_LONGITUDE_VALID; 
    }
    if (gps_data.valid & GPS_DATA_ALT) 
    {
        gnss_pos.altitudeMSL = gps_data.alt;
        gnss_pos.validityBits |= GNSS_POSITION_ALTITUDEMSL_VALID;
        if (gps_data.valid & GPS_DATA_GEOID)
        {   //http://earth-info.nga.mil/GandG/wgs84/gravitymod/wgs84_180/intptWhel.html
            gnss_pos.altitudeEll = gps_data.alt+gps_data.geoid;
            gnss_pos.validityBits |= GNSS_POSITION_ALTITUDEELL_VALID;
        }
    }
    if (gps_data.valid & GPS_DATA_SPEED) 
    {    
        gnss_pos.hSpeed = gps_data.speed;
        gnss_pos.validityBits |= GNSS_POSITION_HSPEED_VALID;
    }
    gnss_pos.vSpeed = 9999; //not available
    if (gps_data.valid & GPS_DATA_SPEED) 
    {  
        gnss_pos.heading = gps_data.course;
        gnss_pos.validityBits |= GPS_DATA_COURSE;
    }
    if (gps_data.valid & GPS_DATA_PDOP) 
    {          
        gnss_pos.pdop = gps_data.pdop;
        gnss_pos.validityBits |= GNSS_POSITION_PDOP_VALID;
    }
    if (gps_data.valid & GPS_DATA_HDOP) 
    {  
        gnss_pos.hdop = gps_data.hdop;
        gnss_pos.validityBits |= GNSS_POSITION_HDOP_VALID;
    }
    if (gps_data.valid & GPS_DATA_VDOP) 
    {      
        gnss_pos.vdop = gps_data.vdop;
        gnss_pos.validityBits |= GNSS_POSITION_VDOP_VALID;
    }
    if (gps_data.valid & GPS_DATA_USAT) 
    {  
        gnss_pos.usedSatellites = gps_data.usat;
        gnss_pos.validityBits |= GNSS_POSITION_USAT_VALID;
    }
    gnss_pos.trackedSatellites = 9999; //not available
    gnss_pos.visibleSatellites = 9999; //not available
    gnss_pos.sigmaHPosition = 9999; //not available
    gnss_pos.sigmaAltitude = 9999; //not available
    gnss_pos.sigmaHSpeed = 9999; //not available
    gnss_pos.sigmaVSpeed = 9999; //not available
    gnss_pos.sigmaHeading = 9999; //not available
    
    gnss_pos.fixStatus = GNSS_FIX_STATUS_NO;
    if (gps_data.valid & GPS_DATA_FIX2D)
    {
        gnss_pos.validityBits |= GNSS_POSITION_STAT_VALID;
        if (gps_data.fix2d)
        {
            gnss_pos.fixStatus = GNSS_FIX_STATUS_2D;
        }
    }
    if ( (gps_data.valid & GPS_DATA_FIX3D) && (gps_data.fix3d) )
    {
        gnss_pos.fixStatus = GNSS_FIX_STATUS_3D;
    }

    //hardcoded values for standard GPS receiver
    gnss_pos.fixTypeBits = GNSS_FIX_TYPE_SINGLE_FREQUENCY;
    gnss_pos.validityBits |= GNSS_POSITION_TYPE_VALID;
    gnss_pos.activated_systems = GNSS_SYSTEM_GPS;
    gnss_pos.validityBits |= GNSS_POSITION_ASYS_VALID;
    gnss_pos.used_systems = GNSS_SYSTEM_GPS;
    gnss_pos.validityBits |= GNSS_POSITION_USYS_VALID;

    return true;
}

/**
 * Convert GPS data from NMEA parser to TGNSSTime struct
 * @param gps_data [IN] GPS data decoded from NMEA parser
 * @param timestamp [IN] timestamp in milliseconds
 * @param gnss_time [OUT] TGNSSTime struct which will be filled with gps_data and timestamp
 * @return conversion has been successful
 */
bool extractTime(const GPS_DATA& gps_data, uint64_t timestamp, TGNSSTime& gnss_time)
{
    gnss_time.validityBits = 0;    
    
    gnss_time.timestamp = timestamp;
    
    if (gps_data.valid & GPS_DATA_TIME) 
    {
        gnss_time.hour = gps_data.time_hh;
        gnss_time.minute = gps_data.time_mm;
        gnss_time.second = gps_data.time_ss;
        gnss_time.ms = 0;
        gnss_time.validityBits |= GNSS_TIME_TIME_VALID; 
    }
    if (gps_data.valid & GPS_DATA_DATE) 
    {
        gnss_time.year = gps_data.date_yyyy;
        gnss_time.month = gps_data.date_mm-1;
        gnss_time.day = gps_data.date_dd;
        gnss_time.validityBits |= GNSS_TIME_DATE_VALID; 
    }

    return true;
}

/**
 * Open GNSS NMEA device for canonical input processing with given baud rate
 * @ref http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html
 * @param gps_device [IN] device, e.g. "/dev/ttyACM0"
 * @param baudrate [IN] baud rate (see definitions in <asm/termbits.h>
 * @return file descriptor of GNSS device
 */
int open_GNSS_NMEA_device(const char* gps_device, unsigned int baudrate)
{
  int fd,c, res;
  struct termios oldtio,newtio;

/* 
  Open modem device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/
    fd = open(gps_device, O_RDWR | O_NOCTTY ); 
    if (fd <0) 
    {
        LOG_ERROR(gContext,"Cannot open: %s", gps_device);
        return fd; 
    }

    tcgetattr(fd,&oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

/* 
  BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
  CRTSCTS : output hardware flow control (only used if the cable has
            all necessary lines. See sect. 7 of Serial-HOWTO)
  CS8     : 8n1 (8bit,no parity,1 stopbit)
  CLOCAL  : local connection, no modem contol
  CREAD   : enable receiving characters
*/
    newtio.c_cflag = GNSS_BAUDRATE | CS8 | CLOCAL | CREAD;
 
/*
  IGNPAR  : ignore bytes with parity errors
  ICRNL   : map CR to NL (otherwise a CR input on the other computer
            will not terminate input)
  otherwise make device raw (no other input processing)
*/
    newtio.c_iflag = IGNPAR;
 
/*
 Raw output.
*/
    newtio.c_oflag = 0;
 
/*
  ICANON  : enable canonical input
  disable all echo functionality, and don't send signals to calling program
*/
    newtio.c_lflag = ICANON;
 
/* 
  initialize all control characters 
  default values can be found in /usr/include/termios.h, and are given
  in the comments, but we don't need them here
*/
    newtio.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
    newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
    newtio.c_cc[VERASE]   = 0;     /* del */
    newtio.c_cc[VKILL]    = 0;     /* @ */
    newtio.c_cc[VEOF]     = 4;     /* Ctrl-d */
    newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
    newtio.c_cc[VSWTC]    = 0;     /* '\0' */
    newtio.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
    newtio.c_cc[VSTOP]    = 0;     /* Ctrl-s */
    newtio.c_cc[VSUSP]    = 0;     /* Ctrl-z */
    newtio.c_cc[VEOL]     = 0;     /* '\0' */
    newtio.c_cc[VREPRINT] = 0;     /* Ctrl-r */
    newtio.c_cc[VDISCARD] = 0;     /* Ctrl-u */
    newtio.c_cc[VWERASE]  = 0;     /* Ctrl-w */
    newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
    newtio.c_cc[VEOL2]    = 0;     /* '\0' */

/* 
  now clean the modem line and activate the settings for the port
*/
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

/* 
  Done
*/
    LOG_DEBUG(gContext, "OPEN successful %d\n", fd);
    return fd;
}

/**
 * Worker thread to read NMEA data from GNSS device and provide to GNSS API
 * @param dev pointer to file descriptor of GNSS device
 */
void* loop_GNSS_NMEA_device(void* dev)
{
    int* p_fd = (int*)dev;
    int fd = *p_fd;
    fd_set readfs;    /* file descriptor set */
    int    maxfd;     /* maximum file desciptor used */
    int linecount=0;
    char buf[255];

    //gps data as returned by NMEA parser
    GPS_DATA gps_data;
    HNMEA_Init_GPS_DATA(&gps_data);

    /* loop until we have a terminating condition */
    LOG_DEBUG(gContext, "entering NMEA reading loop %d\n", fd);
    while (g_GNSS_NMEA_loop) 
    {     
        int res;
        struct timeval Timeout;
        /* set timeout value within input loop */
        Timeout.tv_usec = 0;  /* milliseconds */
        Timeout.tv_sec  = 2;  /* seconds */
        FD_SET(fd, &readfs);
        maxfd = fd+1;

        /* block until input becomes available */
        res = select(maxfd, &readfs, NULL, NULL, &Timeout);
        if (res==-1)
        {
            LOG_DEBUG_MSG(gContext, "select()\n");
        }
        else if (res==0)
        {
            LOG_DEBUG_MSG(gContext, "TIMEOUT\n");
        } 
        else if (FD_ISSET(fd, &readfs))
        {
            res = read(fd,buf,255); 
            buf[res]=0;             /* set end of string, so we can printf */
            linecount++;
            //LOG_DEBUG(gContext, "%d:%s", linecount, buf);
            NMEA_RESULT nmea_res = HNMEA_Parse(buf, &gps_data);
            if (nmea_res == NMEA_GPRMC)
            {
                uint64_t timestamp = gnss_get_timestamp();
                TGNSSTime gnss_time = { 0 };
                TGNSSPosition gnss_pos = { 0 };
                if (extractTime(gps_data, timestamp, gnss_time))
                {
                    updateGNSSTime(&gnss_time, 1);
                }
                if (extractPosition(gps_data, timestamp, gnss_pos))
                {
                    updateGNSSPosition(&gnss_pos,1 );
                }
            }
        }
    }
    LOG_DEBUG_MSG(gContext, "END NMEA reading loop\n");
    return NULL;
}


/**
 * EXTERNAL FUNCTIONS provided by this module
 * @ref gnss-init.h
 * 
 * 
 */


pthread_t g_thread;
int g_fd = -1;

extern bool gnssInit()
{
    g_fd = open_GNSS_NMEA_device(GNSS_DEVICE, GNSS_BAUDRATE);
    if (g_fd >=0) 
    {
        pthread_create (&g_thread, NULL, loop_GNSS_NMEA_device, &g_fd);
        return true;
    }
    else
    {
        perror(GNSS_DEVICE); 
        return false;     
    }
}

extern bool gnssDestroy()
{
    g_GNSS_NMEA_loop = 0;
    pthread_join (g_thread, NULL);
    LOG_DEBUG_MSG(gContext, "gnssDestroy: NMEA reader thread terminated\n");
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

bool gnssConfigGNSSSystems(uint32_t activate_systems)
{
    return false; //satellite system configuration request not supported by NMEA protocol
}