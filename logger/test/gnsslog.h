/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \brief Utility functions to create and log GNSS specific log strings
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

#ifndef INCLUDE_GENIVI_GNSS_LOG
#define INCLUDE_GENIVI_GNSS_LOG

#include "poslog.h"  
#include "gnss.h"
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide a system timestamp in milliseconds.
 * @return system timestamp in milliseconds
 */
uint64_t gnsslogGetTimestamp();
    
/**
 * Convert a TGNSSPosition structure to a log string.
 * @note: The log string will *not* contain a line break (\n) at the end.
 * @param timestamp Timestamp for the current time in ms to be added to the header of the log string.
 *   This timestamp shall be based on the same time source as the timestamp within the position parameter.
 * @param countdown Countdown value to be added to the header of the log string.
 *   This is the number of following GNSS position data in same sequence
 * @param position TGNSSPosition structure to be converted to the log string
 * @param str Pointer to a string variable where the log string will be written to.
 * @param size Size of the string variable where the log string will be written to.
 */
void gnssPositionToString(uint64_t timestamp, uint16_t countdown, const TGNSSPosition* position, char *str, size_t size);

/**
 * Convert a TGNSSTime structure to a log string.
 * @note: The log string will *not* contain a line break (\n) at the end.
 * @param timestamp Timestamp for the current time in ms to be added to the header of the log string.
 *   This timestamp shall be based on the same time source as the timestamp within the time parameter.
 * @param countdown Countdown value to be added to the header of the log string.
 *   This is the number of following GNSS time data in same sequence
 * @param time TGNSSTime structure to be converted to the log string
 * @param str Pointer to a string variable where the log string will be written to.
 * @param size Size of the string variable where the log string will be written to.
 */
void gnssTimeToString(uint64_t timestamp, uint16_t countdown, const TGNSSTime* time, char *str, size_t size);

/**
 * Convert a TGNSSSatelliteDetail structure to a log string.
 * @note: The log string will *not* contain a line break (\n) at the end.
 * @param timestamp Timestamp for the current time in ms to be added to the header of the log string.
 *   This timestamp shall be based on the same time source as the timestamp within the satellite detail parameter.
 * @param countdown Countdown value to be added to the header of the log string.
 *   This is the number of following GNSS satellite data in same sequence
 * @param satelliteDetails TGNSSSatelliteDetail structure to be converted to the log string
 * @param str Pointer to a string variable where the log string will be written to.
 * @param size Size of the string variable where the log string will be written to.
 */
void gnssSatelliteDetailToString(uint64_t timestamp, uint16_t countdown, const TGNSSSatelliteDetail* satelliteDetails, char *str, size_t size);

/**
 * Write GNSS position data to the position log.
 * 
 * @param timestamp Timestamp when the GNSS position data have been received [ms]
 * @param position Pointer to an array of TGNSSPosition with size numElements 
 * @param numElements Number of TGNSSPosition elements in array position
 */
void gnssPositionLog(uint64_t timestamp, const TGNSSPosition position[], uint16_t numElements);

/**
 * Write GNSS time data to the position log.
 * 
 * @param timestamp Timestamp when the GNSS time data have been received [ms]
 * @param time Pointer to an array of TGNSSTime with size numElements 
 * @param numElements: Number of TGNSSTime elements in array time.  
 */
void gnssTimeLog(uint64_t timestamp, const TGNSSTime time[], uint16_t numElements);

/**
 * Write GNSS satellite detail data to the position log.
 * 
 * @param timestamp Timestamp when the GNSS satellite detail data have been received [ms]
 * @param satelliteDetail Pointer to an array of TGNSSSatelliteDetail with size numElements 
 * @param numElements: Number of TGNSSSatelliteDetail elements in array satelliteDetail.  
 */
void gnssSatelliteDetailLog(uint64_t timestamp, const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements);

#ifdef __cplusplus
}
#endif

#endif