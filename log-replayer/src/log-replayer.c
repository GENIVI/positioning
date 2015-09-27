/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup LogReplayer
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

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <memory.h>

#include "log.h"

#define MSGIDLEN 20
#define BUFLEN 256
#define PORT1 9930   //port used for GNSS data
#define PORT2 9931   //port used for sensor data
#define PORT3 9932   //port used for vehicle data
#define IPADDR_DEFAULT "127.0.0.1"
#define MAXDELTA 1000  //max value to avoid overflow

DLT_DECLARE_CONTEXT(gContext);

bool running = true;

void sighandler(int sig)
{
  LOG_INFO_MSG(gContext,"Signal received");
  running = false;
}

bool getStrToSend(FILE* file, char* line, int dim)
{
    static long unsigned int lastTimestamp = 0;
    long unsigned int timestamp = 0;
    long signed int delta = 0;

    if(dim <= 0)
    {
       LOG_ERROR_MSG(gContext,"dim <= 0");
       return false;
    }

    char* ptrStr = fgets(line, dim, file);

    line[dim -1] = '\0';

    //LOG_DEBUG(gContext, "read line %s", line);

    //TODO handle comments in log file

    if(ptrStr == NULL || feof(file))
    {
        //error or end of file
        return false;
    }

    if(strchr(line, '#') != 0)
    {
        line[0] = '\0';
        return true; //skip comment line
    }

    if (!sscanf(line, "%lu", &timestamp))
    {
        line[0] = '\0';
        return true; //skip lines without timestamp
    }

    if(!lastTimestamp)
    {
        delta = 0;
    }
    else
    {
        delta = timestamp - lastTimestamp;
    }

    LOG_DEBUG_MSG(gContext,"------------------------------------------------");
    LOG_DEBUG(gContext,"Waiting %lu ms", delta);
    LOG_DEBUG_MSG(gContext,"------------------------------------------------");

    lastTimestamp = timestamp;

    if(delta < 0)
    {
        LOG_WARNING(gContext,"timestamp steps backward at %ld. ignoring line", timestamp);
        return true;
    }

    if(delta > MAXDELTA)
    {
        LOG_WARNING(gContext,"delta time too big at %ld. delta time set to %d to avoid overflow", timestamp, MAXDELTA);
        delta = MAXDELTA;
    }

    if(usleep(delta*1000) != 0) // TODO time drift issues
    {
        LOG_WARNING(gContext,"uslee failed");
        return true;
    }

    return true;
}

int main(int argc, char* argv[])
{
    struct sockaddr_in si_other;
    socklen_t slen = sizeof(si_other);
    int s;
    FILE * logfile = 0;
    char * filename = 0;
    char buf[BUFLEN];
    char msgId[MSGIDLEN];
    char * ipaddr = 0;

    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    if(argc < 2)
    {
       LOG_ERROR_MSG(gContext,"missing input parameter: logfile");
       return EXIT_FAILURE;
    }
    else
    {
        filename = argv[1];
        if(argc < 3)
            ipaddr = IPADDR_DEFAULT;
        else
            ipaddr = argv[2];
    }

    DLT_REGISTER_APP("RPLY", "LOG-REPLAYER");
    DLT_REGISTER_CONTEXT(gContext,"RSRV", "Global Context");

    LOG_INFO_MSG(gContext,"------------------------------------------------");
    LOG_INFO_MSG(gContext,"LOG REPLAYER STARTED");
    LOG_INFO_MSG(gContext,"------------------------------------------------");

    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        LOG_ERROR_MSG(gContext,"socket() failed!");
        return EXIT_FAILURE;
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    //si_other.sin_port = htons(<port number>);
    if(inet_aton(ipaddr, &si_other.sin_addr) == 0)
    {
        LOG_ERROR_MSG(gContext,"inet_aton() failded!");
        return EXIT_FAILURE;
    }

    logfile = fopen(filename, "r");

    if(logfile == NULL)
    {
    	LOG_ERROR(gContext,"error trying to open file %s",filename);
    	return EXIT_FAILURE;
    }

    LOG_INFO(gContext,"Started reading log file %s",filename);

    while(running)
    {
        if(!getStrToSend(logfile,buf,BUFLEN))
        {
            //error or end of file
            return EXIT_FAILURE;
        }

        if (strlen(buf) < 3)
        {
            //skip empty lines (includes comments)
            continue;
        }

        sscanf(buf, "%*[^'$']$%[^',']", msgId);

        //GNSS: list of supported message IDs
        char* gnssstr = "GVGNSP,GVGNSC,GVGNSAC,GVGNSSAT";
        if(strstr(gnssstr, msgId) != NULL)
        {
            LOG_DEBUG(gContext,"Sending Packet to %s:%d",ipaddr,PORT1);
            LOG_DEBUG(gContext,"MsgID:%s", msgId);
            LOG_DEBUG(gContext,"Len:%d", (int)strlen(buf));
            LOG_DEBUG(gContext,"Data:%s", buf);

            si_other.sin_port = htons(PORT1);
            if(sendto(s, buf, strlen(buf)+1, 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                LOG_ERROR_MSG(gContext,"sendto() failed!");
                return EXIT_FAILURE;
            }
        }

        //SNS: list of supported message IDs
        //char* snsstr = "GVVEHSP,GVGYRO,GVGYROCONF,GVDRVDIR,GVODO,GVWHTK,GVWHTKCONF";
        char* snsstr = "GVSNSVEHSP,GVSNSGYRO,GVSNSWHTK"; //subset currently supported for new log format
        if(strstr(snsstr, msgId) != NULL)
        {
            LOG_DEBUG(gContext,"Sending Packet to %s:%d",ipaddr,PORT2);
            LOG_DEBUG(gContext,"MsgID:%s", msgId);
            LOG_DEBUG(gContext,"Len:%d", (int)strlen(buf));
            LOG_DEBUG(gContext,"Data:%s", buf);

            si_other.sin_port = htons(PORT2);
            if(sendto(s, buf, strlen(buf)+1, 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                LOG_ERROR_MSG(gContext,"sendto() failed!");
                return EXIT_FAILURE;
            }
        }
        //VHL: list of supported message IDs
        char* vhlstr = "GVVEHVER,GVVEHENGSPEED,GVVEHFUELLEVEL,GVVEHFUELCONS,GVVEHTOTALODO";
        if(strstr(vhlstr, msgId) != NULL)
        {
            LOG_DEBUG(gContext,"Sending Packet to %s:%d",ipaddr,PORT3);
            LOG_DEBUG(gContext,"MsgID:%s", msgId);
            LOG_DEBUG(gContext,"Len:%d", (int)strlen(buf));
            LOG_DEBUG(gContext,"Data:%s", buf);

            si_other.sin_port = htons(PORT3);
            if(sendto(s, buf, strlen(buf)+1, 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                LOG_ERROR_MSG(gContext,"sendto() failed!");
                return EXIT_FAILURE;
            }
        }
    }

    close(s);

    return EXIT_SUCCESS;
}


