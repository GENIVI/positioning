/**************************************************************************
 * @licence app begin@
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * \ingroup GNSSService
 * \brief Compliance Level: Abstract Component
 * \copyright Copyright (C) 2012, BMW Car IT GmbH, Continental Automotive GmbH, PCA Peugeot Citroën, XS Embedded GmbH
 * 
 * \license
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
 * this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * @licence end@
 **************************************************************************/

#ifndef INCLUDE_GENIVI_GNSS
#define INCLUDE_GENIVI_GNSS

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


// API Version
#define GENIVI_GNSS_API_MAJOR 4
#define GENIVI_GNSS_API_MINOR 0
#define GENIVI_GNSS_API_MICRO 0

/**
 * Initialization of the GNSS service.
 * Must be called before using the GNSS service to set up the service.
 * @return True if initialization has been successfull.
 */
bool gnssInit();

/**
 * Destroy the GNSS service.
 * Must be called after using the GNSS service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool gnssDestroy();

/**
 * GNSS services version information.
 * This information is for the GNSS services system structure.
 * The version information for each specific GNSS component can be obtained via the metadata.
 * @param major Major version number. Changes in this number are used for incompatible API change.
 * @param minor Minor version number. Changes in this number are used for compatible API change.
 * @param micro Micro version number. Changes in this number are used for minor changes.
 */
void gnssGetVersion(int *major, int *minor, int *micro);


#ifdef __cplusplus
}
#endif

#endif
