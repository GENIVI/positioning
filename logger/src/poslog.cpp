/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \brief Logging service for data from positioning and similar sources
*        Log data can be forwarded to different sinks in parallel,
*        e.g. to DLT and to a file or a socket
*        Additionally a custom callback can be registered as log sink
*        Log data can be provided from different threads running parallel
*        Logging is done synchronously, so it may block temporarily 
*        depending on the type of sinks which are active
*        However using the callback sink, an asynchronous logging
*        can be implemented without impact to the threads providing log data
*        Log data must be provided as ASCII strings 
*        The clients are responsible for string formatting 
*        according the positioning log format specification
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

#include "poslog.h"  
#if (DLT_ENABLED)
#include "dlt.h"
#endif
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>

static pthread_mutex_t mutexLog = PTHREAD_MUTEX_INITIALIZER;  //protects everything
static TPoslogSinks g_active_sinks = 0;
static int g_fd = -1;
static PoslogCallback g_callback = NULL;
#if (DLT_ENABLED)
DLT_DECLARE_CONTEXT(poslogContext);
#endif
  
bool poslogInit()
{
    pthread_mutex_lock(&mutexLog);
    g_active_sinks = 0;
    int g_fd = -1;
    g_callback= NULL;
#if (DLT_ENABLED)
     DLT_REGISTER_CONTEXT(poslogContext,"POSL","Positioning Logging");
#endif
    pthread_mutex_unlock(&mutexLog);
    return true;
}

bool poslogDestroy()
{
    pthread_mutex_lock(&mutexLog);
    g_active_sinks = 0;
    int g_fd = -1;
    g_callback= NULL;
#if (DLT_ENABLED)    
    DLT_UNREGISTER_CONTEXT(poslogContext);
#endif
    pthread_mutex_unlock(&mutexLog);
    return true;
}

void poslogGetVersion(int *major, int *minor, int *micro, EPoslogReleaseLevel *level)
{
    *major = GENIVI_POSLOG_MAJOR;
    *minor = GENIVI_POSLOG_MINOR;
    *micro = GENIVI_POSLOG_MICRO;
    *level = GENIVI_POSLOG_LEVEL;
}

void poslogSetActiveSinks(TPoslogSinks sinks)
{
    pthread_mutex_lock(&mutexLog);
    g_active_sinks = sinks;
    pthread_mutex_unlock(&mutexLog);      
}

TPoslogSinks poslogGetActiveSinks()
{
    pthread_mutex_lock(&mutexLog);
    TPoslogSinks sinks = g_active_sinks;
    pthread_mutex_unlock(&mutexLog);
    return sinks;
}

int poslogSetFD(int fd)
{
    pthread_mutex_lock(&mutexLog);
    int old_fd = g_fd;
    g_fd = fd;
    pthread_mutex_unlock(&mutexLog);
    return old_fd;
}

PoslogCallback poslogSetCB(PoslogCallback cb)
{
    pthread_mutex_lock(&mutexLog);
    PoslogCallback old_callback = g_callback;
    g_callback = cb;
    pthread_mutex_unlock(&mutexLog);      
    return old_callback;
}

static void poslogAddString_nolock(const char* logstring)
{
#if (DLT_ENABLED) 
    if (g_active_sinks & POSLOG_SINK_DLT)
    {
        DLT_LOG(poslogContext, DLT_LOG_INFO, DLT_STRING(logstring));
    }
#endif
    if (g_active_sinks & POSLOG_SINK_SYSLOG)
    {
        //syslog(LOG_INFO, logstring);
        syslog(LOG_EMERG, logstring);
    }
    if (g_active_sinks & POSLOG_SINK_FD)
    {
        write(g_fd, logstring, strlen(logstring));
        write(g_fd, "\n", 1);
    }
    if (g_active_sinks & POSLOG_SINK_CB)
    {
        if (g_callback) g_callback(logstring);
    }          
}

void poslogAddString(const char* logstring, TPoslogSeq seq)
{
    if (seq & POSLOG_SEQ_START)
    {
        pthread_mutex_lock(&mutexLog);
    }
    poslogAddString_nolock(logstring);
    if (seq & POSLOG_SEQ_STOP)
    {
        pthread_mutex_unlock(&mutexLog);
    }
}



