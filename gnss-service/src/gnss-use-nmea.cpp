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
//for integer format macros such as PRIu64
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

//provided interface
#include "gnss-init.h"
//GNSS data types
#include "gnss.h"
//GNSS status
#include "gnss-status.h"
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
#include <sys/timeb.h>
//the NMEA parser
#include "hnmea.h"


//activate this #define to print raw NMEA
//#define NMEA_PRINT_RAW

/**
 * CONFIGURATION PARAMETERS
 *
 * #required
 * GNSS_DEVICE: device name at which GNSS receiver is attached, e.g. "dev/ttyACM0"
 * GNSS_BAUDRATE: baud rate of GNSS receiver at device GNSS_DEVICE, e.g. B38400
 *
 * #optional
 * GNSS_CHIPSET_XXX: Identification of GNSS chipset, e.g. GNSS_CHIPSET_UBLOX
 * GNSS_DELAY: Delay in ms of terminating NMEA sentence with respect to time of fix
 *
 */
#ifndef GNSS_DELAY
#define GNSS_DELAY 0
#endif

DLT_DECLARE_CONTEXT(gContext);

/**
 * INTERNAL FUNCTIONS
 * 
 * 
 */

/** Flag to terminate NMEA reader thread */
volatile int g_GNSS_NMEA_loop=1; 

/** Maximum number of retries to re-open GNSS_DEVICE when select() returns an error */
#define OPEN_RETRY_MAX 15
/** Delay between retiers in seconds */
#define OPEN_RETRY_DELAY 2

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
 * Helper function to conventiently set GNSS status
  */
void setGNSSStatus(EGNSSStatus newStatus)
{
    static EGNSSStatus lastStatus = GNSS_STATUS_NOTAVAILABLE;
    if(newStatus != lastStatus)
    {
        lastStatus = newStatus;
        TGNSSStatus status = {0};
        status.timestamp = gnss_get_timestamp();
        status.status = newStatus;
        status.validityBits = GNSS_STATUS_STATUS_VALID;
        updateGNSSStatus(&status);
    }
}   

/**
 * Convert GNSS data from NMEA parser to TGNSSPosition struct
 * @param gns_data [IN] GNSS data decoded from NMEA parser
 * @param timestamp [IN] timestamp in milliseconds
 * @param gnss_pos [OUT] TGNSSPosition struct which will be filled with gns_data and timestamp
 * @return conversion has been successful
 */
bool extractPosition(const GNS_DATA& gns_data, uint64_t timestamp, TGNSSPosition& gnss_pos)
{
    gnss_pos.validityBits = 0;    
    
    gnss_pos.timestamp = timestamp;
    if (gns_data.valid & GNS_DATA_LAT)
    {
        gnss_pos.latitude = gns_data.lat;
        gnss_pos.validityBits |= GNSS_POSITION_LATITUDE_VALID; 
    }
    if (gns_data.valid & GNS_DATA_LON)
    {
        gnss_pos.longitude = gns_data.lon;
        gnss_pos.validityBits |= GNSS_POSITION_LONGITUDE_VALID; 
    }
    if (gns_data.valid & GNS_DATA_ALT)
    {
        gnss_pos.altitudeMSL = gns_data.alt;
        gnss_pos.validityBits |= GNSS_POSITION_ALTITUDEMSL_VALID;
        if (gns_data.valid & GNS_DATA_GEOID)
        {
            //Geoid separation terminology might be difficult to understand
            //You can cross check your NMEA output and calculation results
            //with an online geoid calculator such as:
            //  http://www.unavco.org/software/geodetic-utilities/geoid-height-calculator/geoid-height-calculator.html
            //  http://earth-info.nga.mil/GandG/wgs84/gravitymod/wgs84_180/intptWhel.html
            //  http://geographiclib.sourceforge.net/cgi-bin/GeoidEval
            gnss_pos.altitudeEll = gns_data.alt+gns_data.geoid;
            gnss_pos.validityBits |= GNSS_POSITION_ALTITUDEELL_VALID;
        }
    }
    if (gns_data.valid & GNS_DATA_SPEED)
    {
        gnss_pos.hSpeed = gns_data.speed;
        gnss_pos.validityBits |= GNSS_POSITION_HSPEED_VALID;
    }
    gnss_pos.vSpeed = 9999; //not available
    if (gns_data.valid & GNS_DATA_COURSE)
    {
        gnss_pos.heading = gns_data.course;
        gnss_pos.validityBits |= GNSS_POSITION_HEADING_VALID;
    }
    if (gns_data.valid & GNS_DATA_PDOP)
    {
        gnss_pos.pdop = gns_data.pdop;
        gnss_pos.validityBits |= GNSS_POSITION_PDOP_VALID;
    }
    if (gns_data.valid & GNS_DATA_HDOP)
    {
        gnss_pos.hdop = gns_data.hdop;
        gnss_pos.validityBits |= GNSS_POSITION_HDOP_VALID;
    }
    if (gns_data.valid & GNS_DATA_VDOP)
    {
        gnss_pos.vdop = gns_data.vdop;
        gnss_pos.validityBits |= GNSS_POSITION_VDOP_VALID;
    }
    if (gns_data.valid & GNS_DATA_USAT)
    {
        gnss_pos.usedSatellites = gns_data.usat;
        gnss_pos.validityBits |= GNSS_POSITION_USAT_VALID;
    }
    gnss_pos.trackedSatellites = 9999; //not available
    gnss_pos.visibleSatellites = 9999; //not available
    if (gns_data.valid & GNS_DATA_HACC)
    {
        gnss_pos.sigmaHPosition = gns_data.hacc;
        gnss_pos.validityBits |= GNSS_POSITION_SHPOS_VALID;
    }
    if (gns_data.valid & GNS_DATA_VACC)
    {
        gnss_pos.sigmaAltitude = gns_data.vacc;
        gnss_pos.validityBits |= GNSS_POSITION_SALT_VALID;
    }

    gnss_pos.sigmaHSpeed = 9999; //not available
    gnss_pos.sigmaVSpeed = 9999; //not available
    gnss_pos.sigmaHeading = 9999; //not available

    gnss_pos.validityBits |= GNSS_POSITION_STAT_VALID; //always valid
    gnss_pos.fixStatus = GNSS_FIX_STATUS_NO;
    if (gns_data.valid & GNS_DATA_FIX2D)
    {
        if (gns_data.fix2d)
        {
            gnss_pos.fixStatus = GNSS_FIX_STATUS_2D;
        }
    }
    if ( (gns_data.valid & GNS_DATA_FIX3D) && (gns_data.fix3d) )
    {
        gnss_pos.fixStatus = GNSS_FIX_STATUS_3D;
    }

    //hardcoded values for standard GPS receiver
    gnss_pos.fixTypeBits = GNSS_FIX_TYPE_SINGLE_FREQUENCY;
    gnss_pos.validityBits |= GNSS_POSITION_TYPE_VALID;
    gnss_pos.activatedSystems = GNSS_SYSTEM_GPS;
    gnss_pos.validityBits |= GNSS_POSITION_ASYS_VALID;
    gnss_pos.usedSystems = GNSS_SYSTEM_GPS;
    gnss_pos.validityBits |= GNSS_POSITION_USYS_VALID;
    //check whether GLONASS is active in addition to GPS
    //TODO check explicitly for GPS to cover GLONASS only
    if ( (gns_data.valid_ext & GNS_DATA_USAT_GLO) && (gns_data.usat_glo > 0) )
    {
        gnss_pos.activatedSystems |= GNSS_SYSTEM_GLONASS;
        gnss_pos.usedSystems |= GNSS_SYSTEM_GLONASS;
        gnss_pos.fixTypeBits |= GNSS_FIX_TYPE_MULTI_CONSTELLATION;
    }
    
    return true;
}

/**
 * Convert GNSS data from NMEA parser to TGNSSTime struct
 * @param gns_data [IN] GNSS data decoded from NMEA parser
 * @param timestamp [IN] timestamp in milliseconds
 * @param gnss_time [OUT] TGNSSTime struct which will be filled with gns_data and timestamp
 * @return conversion has been successful
 */
bool extractTime(const GNS_DATA& gns_data, uint64_t timestamp, TGNSSTime& gnss_time)
{
    gnss_time.validityBits = 0;    
    
    gnss_time.timestamp = timestamp;
    
    if (gns_data.valid & GNS_DATA_TIME)
    {
        gnss_time.hour = gns_data.time_hh;
        gnss_time.minute = gns_data.time_mm;
        gnss_time.second = gns_data.time_ss;
        gnss_time.ms = gns_data.time_ms;
        gnss_time.validityBits |= GNSS_TIME_TIME_VALID; 
    }
    if (gns_data.valid & GNS_DATA_DATE)
    {
        gnss_time.year = gns_data.date_yyyy;
        gnss_time.month = gns_data.date_mm-1;
        gnss_time.day = gns_data.date_dd;
        gnss_time.validityBits |= GNSS_TIME_DATE_VALID; 
    }

    return true;
}

/**
 * Open GNSS NMEA device for canonical input processing with given baud rate
 * @ref http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html
 * @param gps_device [IN] device, e.g. "/dev/ttyACM0"
 * @param baudrate [IN] baud rate (see definitions in <asm/termbits.h>
 * @return file descriptor of GNSS device, negative values indicate an error
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
        //LOG_ERROR(gContext,"Cannot open: %s", gps_device);
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
#ifdef NMEA_PRINT_RAW
    //http://www.chemie.fu-berlin.de/chemnet/use/info/libc/libc_12.html#SEC237
    //http://www.chemie.fu-berlin.de/chemnet/use/info/libc/libc_27.html#SEC465
    //http://www.chemie.fu-berlin.de/chemnet/use/info/libc/libc_27.html#SEC468
    //printf("MAX_CANON: %d\n", fpathconf(fd, _PC_MAX_CANON));
    //printf("MAX_INPUT: %d\n", fpathconf(fd, _PC_MAX_INPUT));
#endif
    //LOG_DEBUG(gContext, "OPEN successful %d\n", fd);
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
    //read failure - used to trigger restart
    bool read_failure = false;
    //trigger message
    NMEA_RESULT trigger = NMEA_RMC;
    //gnss data as returned by NMEA parser
    GNS_DATA gns_data;
    HNMEA_Init_GNS_DATA(&gns_data);

    /* loop until we have a terminating condition */
    //LOG_DEBUG(gContext, "entering NMEA reading loop %d\n", fd);
    while (g_GNSS_NMEA_loop) 
    {     
        int res;
        struct timeval timeout;
        /* set timeout value within input loop */
        timeout.tv_usec = 0;  /* milliseconds */
        timeout.tv_sec  = 2;  /* seconds */
        FD_SET(fd, &readfs);
        maxfd = fd+1;

        /* block until input becomes available */
        res = select(maxfd, &readfs, NULL, NULL, &timeout);
        if (res==-1)
        {
            read_failure = true;
        }
        else if (res==0)
        {
            //LOG_DEBUG_MSG(gContext, "TIMEOUT\n");
        } 
        else if (FD_ISSET(fd, &readfs))
        {
            res = read(fd,buf,255); 
            if (res == 0)
            {
                read_failure = true;
            }
            buf[res]=0;             /* set end of string, so we can printf */
            linecount++;
            //LOG_DEBUG(gContext, "%d:%s", linecount, buf);
            #ifdef NMEA_PRINT_RAW
            printf("%"PRIu64",0,%s",gnss_get_timestamp(), buf);
            fflush(stdout);
            #endif
            NMEA_RESULT nmea_res = HNMEA_Parse(buf, &gns_data);

            //most receivers sent GPRMC as last, but u-blox send as first: use other trigger
            //determine most suitable trigger on actually received messages
            #ifdef GNSS_CHIPSET_UBLOX
            if (nmea_res == NMEA_GST)  //highest precedence
            {
                trigger = NMEA_GST;
            }
            if ((nmea_res == NMEA_GSA) && (trigger == NMEA_RMC)) //GSA better than RMC
            {
                trigger = NMEA_GSA;
            }
            #endif
            if (nmea_res == trigger)
            {
                setGNSSStatus(GNSS_STATUS_AVAILABLE);
                uint64_t timestamp = gnss_get_timestamp() - GNSS_DELAY;
                TGNSSTime gnss_time = { 0 };
                TGNSSPosition gnss_pos = { 0 };
                if (extractTime(gns_data, timestamp, gnss_time))
                {
                    updateGNSSTime(&gnss_time, 1);

                    #ifdef NMEA_PRINT_RAW
                    /* try to determine GNSS_DELAY assumming a well NTP-synched clock */
                    /* http://www.ntp.org/ntpfaq/NTP-s-sw-clocks-quality.htm */
                    struct timeb curtime;
                    struct tm *gmttime;
                    /* Get the current time. */
                    ftime (&curtime);
                    /* Convert it to local time representation. */
                    gmttime = gmtime (&(curtime.time));
                    printf("%"PRIu64",0,$HOSTTIME,%04d,%02d,%02d,%02d,%02d,%02d,%03d\n",
                           gnss_get_timestamp(),
                           gmttime->tm_year+1900, gmttime->tm_mon, gmttime->tm_mday,
                           gmttime->tm_hour, gmttime->tm_min, gmttime->tm_sec, curtime.millitm);
                    fflush(stdout);
                    #endif
                }
                if (extractPosition(gns_data, timestamp, gnss_pos))
                {
                    updateGNSSPosition(&gnss_pos,1 );
                }
            }
        }
        if(read_failure)
        {
            //Error - try to restart device connection
            setGNSSStatus(GNSS_STATUS_RESTARTING);
            close(fd);
            fd = -1;
            int device_open_retries = 0;
            while ((device_open_retries < OPEN_RETRY_MAX) && (fd < 0))
            {
                device_open_retries++;
                sleep(OPEN_RETRY_DELAY);
                fd = open_GNSS_NMEA_device(GNSS_DEVICE, GNSS_BAUDRATE);
            }
            if (fd >=0)
            {
                read_failure = false;
            }
            else
            {
                //reopen failed: terminate thread
                setGNSSStatus(GNSS_STATUS_FAILURE);
                g_GNSS_NMEA_loop = 0;
            }
        }
    }
    close(fd);
    //LOG_DEBUG_MSG(gContext, "END NMEA reading loop\n");
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
    iGnssInit();
    
    setGNSSStatus(GNSS_STATUS_INITIALIZING);
    g_fd = open_GNSS_NMEA_device(GNSS_DEVICE, GNSS_BAUDRATE);
    if (g_fd >=0)
    {
        //U-blox receivers: try to activate GPGST
#ifdef GNSS_CHIPSET_UBLOX
    char act_gst[] = "$PUBX,40,GST,0,0,0,1,0,0*5A\r\n";
    char act_grs[] = "$PUBX,40,GRS,0,0,0,1,0,0*5C\r\n";
    //printf("GNSS_CHIPSET == UBLOX\n");
    write(g_fd, act_gst, strlen(act_gst));
    write(g_fd, act_grs, strlen(act_grs));
#endif
        pthread_create(&g_thread, NULL, loop_GNSS_NMEA_device, &g_fd);
        return true;
    }
    else
    {
        setGNSSStatus(GNSS_STATUS_FAILURE);
        return false;     
    }
}

extern bool gnssDestroy()
{
    g_GNSS_NMEA_loop = 0;
    pthread_join(g_thread, NULL);
    //LOG_DEBUG_MSG(gContext, "gnssDestroy: NMEA reader thread terminated\n");
    
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
    return false; //satellite system configuration request not supported by NMEA protocol
}
