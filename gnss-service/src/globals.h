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
#include "gnss.h"
#include "gnss-meta-data.h"
#include "gnss-status.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const TGnssMetaData gMetaData;

bool iGnssInit();
bool iGnssDestroy();

void updateGNSSTime(const TGNSSTime time[], uint16_t numElements);
void updateGNSSPosition(const TGNSSPosition position[], uint16_t numElements);
void updateGNSSSatelliteDetail(const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements);
void updateGNSSStatus(const TGNSSStatus* status);

#ifdef __cplusplus
}
#endif

#endif /* GLOBALS_H */
