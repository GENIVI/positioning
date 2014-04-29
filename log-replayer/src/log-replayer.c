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
* Update (2014/01/14) : Fabien Hernandez <fabien.hernandez@mpsa.com>,
* 		- add vehicle data
*		- loops exit conditions improvment
*		- add comments
*
* @licence end@
**************************************************************************/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
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
#define IPADDR "127.0.0.1"

DLT_DECLARE_CONTEXT(gContext);

// process a line from the log file
bool getStrToSend(FILE* file, char* line, int dim)
{
    static long unsigned int lastTimestamp = 0;
    long unsigned int timestamp = 0;
    long unsigned int delta = 0;

	// exit if the size of the line buffer is null or negative
    if(dim <= 0)
    {
       LOG_ERROR_MSG(gContext,"dim <= 0");
       return false;
    }

	// extract the line from the file and insert newline caracter at buffer end
    char* ptrStr = fgets(line, dim, file);
    line[dim -1] = '\0';

    LOG_DEBUG(gContext, "read line %s", line);

	// test if a line has been read
    if(ptrStr == NULL)
    {
        return false;
    }

	//skip comment line - no impact on delta times
    if(strchr(line, '#') != 0)
    {  
        return true; 
    }

	// read the timestamp
    sscanf(line,  "%lu", &timestamp);

	// process the delta time between two messages
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

	// check if delta is valid and wait this time before next step
    if(delta >= 0)
    {
        usleep(delta*1000); // TODO time drift issues
    }
    else
    {
        LOG_WARNING(gContext,"timestamp steps backward at %ld, ignoring line", timestamp);
    }

    lastTimestamp = timestamp;

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

    if(argc < 2)
    {
       LOG_ERROR_MSG(gContext,"missing input parameter: logfile");
       return EXIT_FAILURE;
    }

    DLT_REGISTER_APP("RPLY", "LOG-REPLAYER");
    DLT_REGISTER_CONTEXT(gContext,"RSRV", "Global Context");

    LOG_INFO_MSG(gContext,"------------------------------------------------");
    LOG_INFO_MSG(gContext,"LOG REPLAYER STARTED");
    LOG_INFO_MSG(gContext,"------------------------------------------------");

	// socket initialization
    if((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        LOG_ERROR_MSG(gContext,"socket() failed!");
        return EXIT_FAILURE;
    }
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    
	if(inet_aton(IPADDR, &si_other.sin_addr) == 0)
    {
        LOG_ERROR_MSG(gContext,"inet_aton() failded!");
        return EXIT_FAILURE;
    }

	// open the log file (name passed as a parameter to the main fonction)
	filename = argv[1];
    logfile = fopen(filename, "r");
    
	if(logfile == NULL)
    {
    	LOG_ERROR(gContext,"error trying to open file %s",filename);
    	return EXIT_FAILURE;
    }

    LOG_INFO(gContext,"Start reading log file %s",filename);

	// loop until the end the file or error    
	while(getStrToSend(logfile,buf,BUFLEN) && !feof(logfile))
    {
		// extract the Message ID
        sscanf(buf, "%*[^'$']$%[^',']", msgId);

		LOG_INFO(gContext, "read line %s", buf);

    	//GNSS: list of supported message IDs
		char* gnssstr = "GVGNSVER,GVGNSP,GVGNSC,GVGNSSAC,GVGNSC3D,GVGNSSSAT,"
					"GVGNSSUTCT,GVGNSSUTCD";
		// Messages identifiers in the GNSS list are sent on PORT1
		// Comment lines are not treated
        if(strstr(gnssstr, msgId) != NULL && buf[0] != '#')
        {
            LOG_DEBUG(gContext,"Sending Packet to %s:%d",IPADDR,PORT1);
            LOG_DEBUG(gContext,"MsgID:%s", msgId);
            LOG_DEBUG(gContext,"Len:%d", (int)strlen(buf));
            LOG_DEBUG(gContext,"Data:%s", buf);

			// set the socket port and send the message
            si_other.sin_port = htons(PORT1);
            if(sendto(s, buf, strlen(buf)+1, 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                LOG_ERROR_MSG(gContext,"sendto() failed!");
                return EXIT_FAILURE;
            }
        }
       
        //SNS: list of supported message IDs
		char* snsstr = "GVSNSVER,GVSNSACC,GVSNSACCCONF,GVSNSGYRO,GVSNSGYROCONF,"
			"GVSNSINCL,GVSNSODO,GVSNSREV,GVSNSSLIP,GVSTEER,GVSNSVEHCONF,"
			"GVSNSVEHSP,GVSNSVEHST,GVSNSWHTK,GVSNSWHTKCONF,GVSNSWHA,GVSNSWHS,"
			"GVSNSDRVDIR,";
		// Message identifiers in the SNS list are sent on PORT2
		// Comment lines are not treated
        if(strstr(snsstr, msgId) != NULL && buf[0] != '#')
        {
            LOG_DEBUG(gContext,"Sending Packet to %s:%d",IPADDR,PORT2);
            LOG_DEBUG(gContext,"MsgID:%s", msgId);
            LOG_DEBUG(gContext,"Len:%d", (int)strlen(buf));
            LOG_DEBUG(gContext,"Data:%s", buf);

			// change the socket port and send the message
            si_other.sin_port = htons(PORT2);
            if(sendto(s, buf, strlen(buf)+1, 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                LOG_ERROR_MSG(gContext,"sendto() failed!");
                return EXIT_FAILURE;
            }
        }

        //VHL: list of supported message IDs
		char* vhlstr = "GVVEHVER,GVVEHENGSPEED,GVVEHFUELLEVEL,GVVEHFUELCONS,"
			"GVVEHTOTALODO,GVVEHWHRDCONF,GVVEHFRTKWDCONF,GVVEHRRTKWDCONF,"
			"GVVEHFRWHBSCONF,GVVEHRRWHBSCONF";
		// Message identifiers in the VHL list are sent on PORT3
        // Comment lines are not treated
        if(strstr(vhlstr, msgId) != NULL && buf[0] != '#')
        {
            LOG_DEBUG(gContext,"Sending Packet to %s:%d",IPADDR,PORT3);
            LOG_DEBUG(gContext,"MsgID:%s", msgId);
            LOG_DEBUG(gContext,"Len:%d", (int)strlen(buf));
            LOG_DEBUG(gContext,"Data:%s", buf);
	
			// change the socket port and send the message
            si_other.sin_port = htons(PORT3);
            if(sendto(s, buf, strlen(buf)+1, 0, (struct sockaddr *)&si_other, slen) == -1)
            {
                LOG_ERROR_MSG(gContext,"sendto() failed!");
                return EXIT_FAILURE;
            }
        }
    }

	// check why the loop exit to adjust return value
	if(feof(logfile))
		return EXIT_SUCCESS;
	else
    	return EXIT_FAILURE;
}


