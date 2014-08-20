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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdbool.h>
#include <pthread.h>
#include <time.h>

#include "gnss-init.h"
#include "gnss-ext.h"
#include "gnss-meta-data.h"

extern pthread_mutex_t mutexCb;
extern pthread_mutex_t mutexData;

extern TGNSSSatelliteDetail gSatelliteDetail; //TODO: buffer full set of satellite details for one point in time
extern TGNSSPosition gPosition;
extern TGNSSTime gTime;


extern const TGnssMetaData gMetaData;

extern GNSSSatelliteDetailCallback cbSatelliteDetail;
extern GNSSPositionCallback cbPosition;
extern GNSSTimeCallback cbTime;

#endif /* GLOBALS_H */
