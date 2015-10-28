/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \brief Test program for the positioning logger
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

void testCb(const char* string)
{
    printf("CB %s\n", string);
}

int main()
{
    char log1[] = "LOG 1";
    char log2[] = "LOG 2";
    const char* logstrings[] = {log1, log2};
    char logger_version_string[64];
    int major;
    int minor;
    int micro;
    EPoslogReleaseLevel level;
  
    //prepare the sinks: DLT, syslog, file descriptor, callback
#if (DLT_ENABLED)  
    DLT_REGISTER_APP("PLT","Test Application for Positioning Logging");
#endif  
    openlog("POSLOGTEST", LOG_PID, LOG_USER);//syslog
    poslogSetFD(STDOUT_FILENO);
    poslogSetCB(testCb);

    //Init
    poslogInit();
    
    poslogGetVersion(&major, &minor, &micro, &level);
    sprintf(logger_version_string, "0,0$GVLOGVER,%d,%d,%d,%X", major, minor, micro, level);
    //activate all possible sinks
    poslogSetActiveSinks(POSLOG_SINK_DLT|POSLOG_SINK_SYSLOG|POSLOG_SINK_FD|POSLOG_SINK_CB);
    
    poslogAddString(logger_version_string);
    poslogAddString(logstrings[0],POSLOG_SEQ_START);
    poslogAddString(logstrings[1],POSLOG_SEQ_STOP);
    
    //cleanup
    poslogSetActiveSinks(0);
    poslogAddString("This log should not appear");
    closelog();//syslog
    poslogDestroy();
#if (DLT_ENABLED)    
    DLT_UNREGISTER_APP();
#endif
}