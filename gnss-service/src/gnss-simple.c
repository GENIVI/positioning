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

#include "globals.h"
#include "gnss-simple.h"

GNSSPositionCallback cbPosition = 0;
GNSSCourseCallback cbCourse = 0;
TGNSSPosition gPosition;
TGNSSCourse gCourse;
TGNSSSimpleConfiguration gConfiguration;

bool gnssSimpleInit()
{
    gPosition.validityBits = 0;
    gConfiguration.validityBits = 0;
    gCourse.validityBits = 0;

    cbPosition = 0;
    cbCourse = 0;

    return true;
}

bool gnssSimpleDestroy()
{
    return true;
}

bool gnssSimpleGetConfiguration(TGNSSSimpleConfiguration* gnssConfig)
{
    if(!gnssConfig)
    {
    	return false;
    }

    *gnssConfig = gConfiguration;

    return true;
}

bool gnssSimpleRegisterPositionCallback(GNSSPositionCallback callback)
{
    //printf("gnssSimpleRegisterPositionCallback\n");
    if(cbPosition != 0) 
    {
        return false;
    }

    pthread_mutex_lock(&mutexCb);
    cbPosition = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool gnssSimpleDeregisterPositionCallback(GNSSPositionCallback callback)
{
    //printf("gnssSimpleDeregisterPositionCallback\n");
    if (cbPosition == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbPosition = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }
    return false;
}

bool gnssSimpleGetPosition(TGNSSPosition* pos)
{
   if(!pos)
   {
      return false;
   }

   pthread_mutex_lock(&mutexData);
   *pos = gPosition;
   pthread_mutex_unlock(&mutexData);

   return true;
}

bool gnssSimpleRegisterCourseCallback(GNSSCourseCallback callback)
{
    //printf("gnssSimpleRegisterCourseCallback\n");
    if(cbCourse != 0) 
    {
        return false;
    }

    pthread_mutex_lock(&mutexCb);
    cbCourse = callback;
    pthread_mutex_unlock(&mutexCb);

    return true;
}

bool gnssSimpleDeregisterCourseCallback(GNSSCourseCallback callback)
{
    //printf("gnssSimpleDeregisterCourseCallback\n");
    if(cbCourse == callback && callback != 0)
    {
        pthread_mutex_lock(&mutexCb);
        cbCourse = 0;
        pthread_mutex_unlock(&mutexCb);

        return true;
    }

    return false;
}

bool gnssSimpleGetCourse(TGNSSCourse* course)
{
    if(!course)
    {
        return false;
    }

    pthread_mutex_lock(&mutexData);
    *course = gCourse;
    pthread_mutex_unlock(&mutexData);
	
    return true;
}




