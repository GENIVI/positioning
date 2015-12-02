/**************************************************************************
 * Component Name: SensorsService
 * Compliance Level: Abstract Component
 * Copyright (C) 2012, BMW Car IT GmbH, Continental Automotive GmbH, PCA Peugeot Citroën, XS Embedded GmbH
 * License:
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
 * this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 **************************************************************************/

#ifndef INCLUDE_GENIVI_SNS
#define INCLUDE_GENIVI_SNS

// API Version
#define GENIVI_SNS_API_MAJOR 4
#define GENIVI_SNS_API_MINOR 0
#define GENIVI_SNS_API_MICRO 0

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialization of the sensor service system infrastructure.
 * Must be called before using any of the sensor services to set up general structures and connections
 * to the sensors or signal providers.
 * If not called before a sensor init the system behaviour is not defined.
 * @return True if initialization has been successful.
 */
bool snsInit();

/**
 * Destroy the sensor services.
 * Must be called after using the sensor services for shut down.
 * After this call no sensor will be able to receive sensor data.
 * The individual sensors must be shut down before this call. Otherwise system behaviour is not defined.
 * @return True if shutdown has been successful.
 */
bool snsDestroy();

/**
 * Sensor services version information.
 * This information is for the sensor services system structure.
 * The version information for each sensor can be obtained via the metadata.
 * @param major Major version number. Changes in this number are used for incompatible API change.
 * @param minor Minor version number. Changes in this number are used for compatible API change.
 * @param micro Micro version number. Changes in this number are used for minor changes.
 */
void snsGetVersion(int *major, int *minor, int *micro);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_SNS */
