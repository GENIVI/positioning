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


#define __STDC_FORMAT_MACROS
#include <inttypes.h>


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
#include <inttypes.h>

#include "gnss-init.h"
#include "gnss.h"
#include "gnss-status.h"
#include "sns-init.h"



/**
 * Double buffer for log strings
 * Purpose: avoid blocking of callback functions by I/O
 * Multiple writer threads are allowed but only on reader thread
 */
class DBuf {

#define DBUF_STRING_SIZE 256
#define DBUF_NUM_LINES 512

    struct SBuf {
    public:
        char strings [DBUF_STRING_SIZE] [DBUF_NUM_LINES];
        uint16_t rnext;
        uint16_t wnext;
        SBuf(): rnext(0), wnext(0) {};
    };

    SBuf* wbuf;
    SBuf* rbuf;
    pthread_mutex_t wmutex;
    pthread_mutex_t rmutex;
public:

    DBuf()
    {
        pthread_mutex_init(&wmutex, NULL);
        pthread_mutex_init(&rmutex, NULL);
        wbuf = new(SBuf);
        rbuf = new(SBuf);
    }

    /**
     * Add a string to the buffer.
     * Return true on success, false on failure (e.g. buffer full).
     */
    bool write(const char* s)
    {
        bool retval = false;
        pthread_mutex_lock(&wmutex);
        if (s && wbuf && (wbuf->wnext < DBUF_NUM_LINES))
        {
            strncpy(wbuf->strings[wbuf->wnext], s, DBUF_STRING_SIZE-1);
            wbuf->strings[wbuf->wnext][DBUF_STRING_SIZE-1] = 0;
            wbuf->wnext++;
            retval = true;
        }
        pthread_mutex_unlock(&wmutex);
        return retval;
    }

    /**
     * Read next string from the buffer
     * Return NULL, if no more string available
     */
    const char* read()
    {
        const char* ret = NULL;
        pthread_mutex_lock(&rmutex);
        if (rbuf && (rbuf->rnext < rbuf->wnext) && (rbuf->rnext < DBUF_NUM_LINES) )
        {
            ret = rbuf->strings[rbuf->rnext];
            rbuf->rnext++;
        }
        pthread_mutex_unlock(&rmutex);
        return ret;
    }

    /**
     * Swap read and write buffers.
     * Clears read buffer before to ensure that new write buffer is empty.
     * Shall only be called by reader thread when read buffer has been
     * completely processed.
     */
    void swap()
    {
        SBuf* tmp;
        pthread_mutex_lock(&rmutex);
        rbuf->rnext = 0;
        rbuf->wnext = 0;
        pthread_mutex_lock(&wmutex);
        tmp = wbuf;
        wbuf = rbuf;
        rbuf = tmp;
        pthread_mutex_unlock(&wmutex);
        pthread_mutex_unlock(&rmutex);
    }

};



#define GNSS_INIT_MAX_RETRIES 30

//global variable to control the main loop - will be set by signal handlers or status callback
enum EExitCondition {
    eExitNone = 0,
    eExitSigInt,
    eExitGnssFailure
};

static volatile bool g_sigterm = false;
static volatile EExitCondition g_exit = eExitNone;
static volatile bool g_gnss_failure = false;
//global pointer to the double buffer
DBuf* g_dbuf = 0;
pthread_t g_logthread;
uint32_t g_write_failures = 0;
FILE* g_logfile = 0;

/**
 * Logger callback to add string to the double buffer.
 */
void dbufCb(const char* string)
{
    if (g_dbuf)
    {
        bool write_success = g_dbuf->write(string);
        if (!write_success)
        {
            g_write_failures++;
        }
        //printf("WBUF %s\n", string);
    }
}

/**
 * Background thread to write double buffer to a file.
 *
 */
void* loop_log_writer(void*)
{
    bool stop = false;
    //printf("LOGWRITER\n");
    while (g_dbuf && !stop && !g_sigterm)
    {
        const char* s = 0;
        //process read buffer - should always be empty
        while (s = g_dbuf->read())
        {
            printf("BUF1 %s\n", s);
        }
        //swap and process previous write buffer
        g_dbuf->swap();
        while (s = g_dbuf->read())
        {
            fprintf(g_logfile, "%s\n", s);
        }
        fflush(g_logfile);
        //printf("LOGWRITER SLEEP\n");
        if (g_exit == eExitNone)
        {
            sleep(2);
            //TODO it seems that only the main thread will be interrupted by the signal???
            //TODO CHECK how to force faster reaction
        }
        else
        {
            stop = true;
        }
        //printf("LOGWRITER WAKEUP\n");
    }
    //printf("LOGWRITER END\n");
}

static void sigHandler (int sig, siginfo_t *siginfo, void *context)
{
    if (sig == SIGINT) 
    {
        g_exit = eExitSigInt;
        if (g_logfile)
        {
            //ensure that also the logger thread is interrupted
            //pthread_kill(g_logthread, sig);  //seems not to work somehow
        }
    }
    else 
    if (sig == SIGTERM) 
    {
        g_sigterm = true;
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

static void cbGNSSStatus(const TGNSSStatus *status)
{
    if (status && (status->validityBits & GNSS_STATUS_STATUS_VALID))
    {
        char status_string[64];
        sprintf(status_string, "#GNSS Status: %d", status->status);
        poslogAddString(status_string);
        if (status->status == GNSS_STATUS_FAILURE)
        {
            g_exit = eExitGnssFailure;
        }
    }
}

static void cbAccel(const TAccelerationData accelerationData[], uint16_t numElements)
{
    accelerationDataLog(snslogGetTimestamp(), accelerationData, numElements);
}

static void cbGyro(const TGyroscopeData gyroData[], uint16_t numElements)
{
    gyroscopeDataLog(snslogGetTimestamp(), gyroData, numElements);
}

#define UDP_LOG
#ifdef UDP_LOG
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#define UDP_BUFLEN  128 //Max length of buffer
#define UDP_PORT 5701   //The port on which to listen for incoming data
#define UDP_LOGLEN  256

pthread_t g_udpthread;

void* loop_udp_log(void*)
{
    struct sockaddr_in si_me, si_other;
    int s;
    socklen_t slen = sizeof(si_other);
    ssize_t recv_len;
    char buf[UDP_BUFLEN];
    char log_string[UDP_LOGLEN];
    bool ok = true;

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        ok = false;
    }

    if (ok)
    {
        // zero out the structure
        memset((char *) &si_me, 0, sizeof(si_me));
        si_me.sin_family = AF_INET;
        si_me.sin_port = htons(UDP_PORT);
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);

        //bind socket to port
        if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
        {
            ok = false;
        }
    }

    //keep listening for data
    while(ok)
    {
        //try to receive some data, this is a blocking call
        if ((recv_len = recvfrom(s, buf, UDP_BUFLEN-1, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            ok = false;
        }
        else
        {
            buf[recv_len]=0;
            snprintf(log_string, UDP_LOGLEN-1, "%"PRIu64",0,$UDP,%s,%d,%s",
                snslogGetTimestamp(),
                inet_ntoa(si_other.sin_addr),
                ntohs(si_other.sin_port),
                buf
            );
            log_string[UDP_LOGLEN-1] = 0; //ensure that string is null-terminated
            poslogAddString(log_string);
        }
    }
    close(s);
}
#endif




int main (int argc, char *argv[])
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

        if(argv[1] && (g_logfile = fopen(argv[1], "a")))
        {
            g_dbuf = new(DBuf);
            poslogSetCB(dbufCb);
            pthread_create(&g_logthread, NULL, loop_log_writer, NULL);
            poslogSetActiveSinks(POSLOG_SINK_DLT|POSLOG_SINK_CB);
        }
        else
        {
            poslogSetActiveSinks(POSLOG_SINK_DLT|POSLOG_SINK_FD|POSLOG_SINK_CB);
        }

        gnssGetVersion(&major, &minor, &micro);
        sprintf(version_string, "0,0$GVGNSVER,%d,%d,%d", major, minor, micro);
        poslogAddString(version_string);
        snsGetVersion(&major, &minor, &micro);
        sprintf(version_string, "0,0$GVSNSVER,%d,%d,%d", major, minor, micro);
        poslogAddString(version_string);

#ifdef UDP_LOG
        pthread_create(&g_udpthread, NULL, loop_udp_log, NULL);
#endif
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
        while (!is_gnss_init_ok && (gnss_init_tries < GNSS_INIT_MAX_RETRIES) && (g_exit == eExitNone) && !g_sigterm)
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
            gnssRegisterStatusCallback(&cbGNSSStatus);
        }

        if (is_sns_init_ok || is_gnss_init_ok)
        {
            while(!g_sigterm && (g_exit == eExitNone))
            {
                sleep(1);
            }
        }
        else
        {
            poslogAddString("#ERR snsInit() or gnssInit() failure - terminating");
        }
        
        //if not interrupted by SIGTERM then we have time to cleanup
        if (!g_sigterm)
        {
            if (g_exit == eExitSigInt)
            {
                poslogAddString("#SIGINT");
            }
            if (g_exit == eExitGnssFailure)
            {
                poslogAddString("#GNSS_STATUS_FAILURE");
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
                gnssDeregisterStatusCallback(&cbGNSSStatus);
                gnssDeregisterPositionCallback(&cbPosition);
                gnssDeregisterTimeCallback(&cbTime);
                gnssDestroy();
            }
        }
        if (g_logfile)
        {
            pthread_join(g_logthread, NULL);
            fclose(g_logfile);
            printf("#Write Failures: %"PRIu32"\n", g_write_failures);
        }
        poslogDestroy();
    }

#if (DLT_ENABLED)
    DLT_UNREGISTER_APP();
#endif
}
