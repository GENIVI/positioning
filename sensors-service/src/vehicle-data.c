/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup SensorsService
* \author Helmut Schmidt <Helmut.3.Schmidt@continental-corporation.com>
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
#include "vehicle-data.h"
#include "sns-meta-data.h"

static pthread_mutex_t mutexCb  = PTHREAD_MUTEX_INITIALIZER;   //protects the callbacks
static pthread_mutex_t mutexData = PTHREAD_MUTEX_INITIALIZER;  //protects the data

TVehicleDataConfiguration gVehicleDataConfiguration = {0};


bool iVehicleDataInit()
{

    pthread_mutex_lock(&mutexData);
    //example vehicle data configuration
    gVehicleDataConfiguration.vehicleType = SNS_CAR;
    gVehicleDataConfiguration.drivenAxles = SNS_AXLE_FRONT;    
    gVehicleDataConfiguration.trackWidth = 1.52;
    gVehicleDataConfiguration.wheelBase = 2.31;
    gVehicleDataConfiguration.validityBits = 
      VEHICLESTATUS_VEHICLETYPE_VALID | 
      VEHICLESTATUS_DRIVENAXLES_VALID |
      VEHICLESTATUS_TRACKWIDTH_VALID |
      VEHICLESTATUS_WHELLBASE_VALID;
    pthread_mutex_unlock(&mutexData);

    return true;
}

bool iVehicleDataDestroy()
{
    return true;
}


bool snsVehicleDataGetConfiguration(TVehicleDataConfiguration* vehicleDataConfiguration)
{
    bool retval = false; 
    
    if(vehicleDataConfiguration) 
    {
        pthread_mutex_lock(&mutexData);
        *vehicleDataConfiguration = gVehicleDataConfiguration;
        pthread_mutex_unlock(&mutexData);
        retval = true;
    }

    return retval;
}

