/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup PositionWebService
* \author Marco Residori <marco.residori@xse.de>
*
* \copyright Copyright (C) 2014, XS Embedded GmbH
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#include <stdio.h>
#include "SharedData.h"

SharedData::SharedData()
: mLatitude(0)
, mLongitude(0)
, mAltitude(0)
, mSpeed(0)
{
}

void SharedData::setLatitude(double val)
{
   pthread_mutex_lock(&mMutex);
   mLatitude = val;
   pthread_mutex_unlock(&mMutex);
}

void SharedData::getLatitude(double& val)
{
   pthread_mutex_lock(&mMutex);
   val = mLatitude;
   pthread_mutex_unlock(&mMutex);
}

void SharedData::setLongitude(double val)
{
   pthread_mutex_lock(&mMutex);
   mLongitude = val;
   pthread_mutex_unlock(&mMutex);
}

void SharedData::getLongitude(double& val)
{
   pthread_mutex_lock(&mMutex);
   val = mLongitude;
   pthread_mutex_unlock(&mMutex);
}

void SharedData::setAltitude(double val)
{
   pthread_mutex_lock(&mMutex);
   mAltitude = val;
   pthread_mutex_unlock(&mMutex);
}

void SharedData::getAltitude(double& val)
{
   pthread_mutex_lock(&mMutex);
   val = mAltitude;
   pthread_mutex_unlock(&mMutex);
}

void SharedData::setSpeed(double val)
{
   pthread_mutex_lock(&mMutex);
   mSpeed = val;
   pthread_mutex_unlock(&mMutex);
}

void SharedData::getSpeed(double& val)
{
   pthread_mutex_lock(&mMutex);
   val = mSpeed;
   pthread_mutex_unlock(&mMutex);
}






