/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup SensorsService
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
#include "sns-meta-data.h"

#define NUM_SENSORS 4

const TSensorMetaData gSensorsMetaData[NUM_SENSORS] = {
    {GENIVI_SNS_API_MAJOR,        //version
     SENSOR_CATEGORY_PHYSICAL,    //category
     SENSOR_TYPE_WHEELTICK,       //type
     100},                        //cycleTime in ms
    {GENIVI_SNS_API_MAJOR,        //version
     SENSOR_CATEGORY_PHYSICAL,    //category
     SENSOR_TYPE_GYROSCOPE,       //type
     100},                        //cycleTime in ms
    {GENIVI_SNS_API_MAJOR,        //version
     SENSOR_CATEGORY_PHYSICAL,    //category
     SENSOR_TYPE_VEHICLE_SPEED,   //type
     100},                         //cycleTime in ms
    {GENIVI_SNS_API_MAJOR,        //version
     SENSOR_CATEGORY_PHYSICAL,    //category
     SENSOR_TYPE_ACCELERATION,    //type
     100}                         //cycleTime in ms     
};

int32_t getSensorMetaDataList(const TSensorMetaData** metadata)
{
    *metadata = gSensorsMetaData;

    return NUM_SENSORS;
}
