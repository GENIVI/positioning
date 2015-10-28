/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \brief Utility functions to create and log SNS specific log strings
*        
*
* \author Helmut Schmidt <https://github.com/huirad>
*
* \copyright Copyright (C) 2015, Helmut Schmidt
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#ifndef INCLUDE_GENIVI_SNS_LOG
#define INCLUDE_GENIVI_SNS_LOG

#include "poslog.h"
#include "acceleration.h"
#include "gyroscope.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide a system timestamp in milliseconds.
 * @return system timestamp in milliseconds
 */
uint64_t snslogGetTimestamp();
    
/**
 * Convert a TAccelerationData structure to a log string.
 * @note: The log string will *not* contain a line break (\n) at the end.
 * @param timestamp Timestamp for the current time in ms to be added to the header of the log string.
 *   This timestamp shall be based on the same time source as the timestamp within the accelerationData parameter.
 * @param countdown Countdown value to be added to the header of the log string.
 *   This is the number of following acceleration data in same sequence
 * @param accelerationData TAccelerationData structure to be converted to the log string
 * @param str Pointer to a string variable where the log string will be written to.
 * @param size Size of the string variable where the log string will be written to.
 */
void accelerationDataToString(uint64_t timestamp, uint16_t countdown, const TAccelerationData* accelerationData, char *str, size_t size);

/**
 * Write acceleration data to the position log.
 * 
 * @param timestamp Timestamp when the acceleration data have been received [ms]
 * @param accelerationData Pointer to an array of TAccelerationData with size numElements 
 * @param numElements Number of TAccelerationData elements in array accelerationData
 */
void accelerationDataLog(uint64_t timestamp, const TAccelerationData accelerationData[], uint16_t numElements);

/**
 * Convert a TGyroscopeData structure to a log string.
 * @note: The log string will *not* contain a line break (\n) at the end.
 * @param timestamp Timestamp for the current time in ms to be added to the header of the log string.
 *   This timestamp shall be based on the same time source as the timestamp within the gyroData parameter.
 * @param countdown Countdown value to be added to the header of the log string.
 *   This is the number of following acceleration data in same sequence
 * @param gyroData TGyroscopeData structure to be converted to the log string
 * @param str Pointer to a string variable where the log string will be written to.
 * @param size Size of the string variable where the log string will be written to.
 */
void gyroscopeDataToString(uint64_t timestamp, uint16_t countdown, const TGyroscopeData* gyroData, char *str, size_t size);

/**
 * Write acceleration data to the position log.
 * 
 * @param timestamp Timestamp when the acceleration data have been received [ms]
 * @param gyroData Pointer to an array of TGyroscopeData with size numElements 
 * @param numElements Number of TGyroscopeData elements in array gyroData
 */
void gyroscopeDataLog(uint64_t timestamp, const TGyroscopeData gyroData[], uint16_t numElements);


#ifdef __cplusplus
}
#endif

#endif