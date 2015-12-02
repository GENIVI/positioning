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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include "sns-init.h"
#include "wheel.h"
#include "acceleration.h"
#include "gyroscope.h"
#include "vehicle-speed.h"
#include "sns-meta-data.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const TSensorMetaData gSensorsMetaData[];

bool iAccelerationInit();
bool iAccelerationDestroy();
void updateAccelerationData(const TAccelerationData accelerationData[], uint16_t numElements);
void updateAccelerationStatus(const TSensorStatus* status);

bool iGyroscopeInit();
bool iGyroscopeDestroy();
void updateGyroscopeData(const TGyroscopeData gyroData[], uint16_t numElements);
void updateGyroscopeStatus(const TSensorStatus* status);


bool iWheelInit();
bool iWheelDestroy();
void updateWheelData(const TWheelData wheelData[], uint16_t numElements);
void updateWheelStatus(const TSensorStatus* status);

bool iVehicleSpeedInit();
bool iVehicleSpeedDestroy();
void updateVehicleSpeedData(const TVehicleSpeedData vehicleSpeedData[], uint16_t numElements);
void updateVehicleSpeedStatus(const TSensorStatus* status);

#ifdef __cplusplus
}
#endif

#endif /* GLOBALS_H */
