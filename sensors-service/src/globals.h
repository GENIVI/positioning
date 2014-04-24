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

#include "sns.h"
#include "wheel.h"
#include "gyroscope.h"
#include "vehicle-speed.h"
#include "sns-meta-data.h"

extern pthread_mutex_t mutexCb;
extern pthread_mutex_t mutexData;

extern TGyroscopeData gGyroscopeData;
extern TWheelticks gWheelticks;
TVehicleSpeedData gVehicleSpeedData;
extern const TSensorMetaData gSensorsMetaData[];

extern WheeltickCallback cbWheelticks;
extern GyroscopeCallback cbGyroscope;
VehicleSpeedCallback cbVehicleSpeed;

#endif /* GLOBALS_H */
