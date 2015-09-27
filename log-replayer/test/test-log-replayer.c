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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <memory.h>

#define BUFLEN 256
//#define PORT 9930

#include "log.h"

DLT_DECLARE_CONTEXT(gContext);

int main(int argc, char* argv[])
{
    struct sockaddr_in si_me, si_other;
    int s;
    socklen_t slen = sizeof(si_other);
    ssize_t readBytes = 0;
    char buf[BUFLEN];
    int port = 0;
    
    DLT_REGISTER_APP("RPLY", "REPLAYER");
    DLT_REGISTER_CONTEXT(gContext,"RCLT","Global Context");

    if(argc < 2)
    {
        LOG_ERROR_MSG(gContext,"missing input parameter: port");
        return EXIT_FAILURE;
    }

    port = atoi(argv[1]);

    if((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
    {
        LOG_ERROR_MSG(gContext,"socket() failed!");
        return EXIT_FAILURE;
    }

    memset((char *) &si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;

    si_me.sin_port = htons(port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s,  (struct sockaddr *) &si_me, sizeof(si_me))==-1)
    {
        LOG_ERROR_MSG(gContext,"socket() failed!");
        return EXIT_FAILURE;
    }

    while(1)
    {
        readBytes = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen);

        if(readBytes < 0)
        {
            LOG_ERROR_MSG(gContext,"recvfrom() failed!");
            return EXIT_FAILURE;
        }

        buf[readBytes] = '\0';

        LOG_INFO(gContext,"Received packet from %s:%d", 
                 inet_ntoa(si_other.sin_addr), 
                 ntohs(si_other.sin_port));

        LOG_INFO(gContext,"Data: %s", buf);
    }

    close(s);

    return EXIT_SUCCESS;
}
