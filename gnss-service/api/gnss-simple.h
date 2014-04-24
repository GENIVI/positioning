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

#ifndef INCLUDE_GENIVI_GNSS_SIMPLE
#define INCLUDE_GENIVI_GNSS_SIMPLE

#include "gnss-meta-data.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reference level for the GNSS altitude.
 */
typedef enum {
    GNSS_ALTITUDE_UNKNOWN = 0,                   /**< Reference level is unknown. */
    GNSS_ALTITUDE_ELLIPSOIDE = 1,                /**< Reference level is the WGS-84 ellopsoid. */
    GNSS_ALTITUDE_ABOVE_MEAN_SEA_LEVEL = 2       /**< Reference level is the geoid (mean sea level). */
} EGNSSAltitudeType;

/**
 * TGNSSSimpleConfiguration::validityBits provides information about the currently valid values of GNSS configuration data.
 * It is a or'ed bitmask of the EGNSSSimpleConfigurationValidityBits values.
 */
typedef enum {
    GNSS_SIMPLE_CONFIG_ALTITUDE_TYPE_VALID    = 0x00000001    /**< Validity bit for field TGNSSSimpleConfiguration::typeOfAltitude. */
} EGNSSSimpleConfigurationValidityBits;

/**
 * GNSS Simple Configuration Data.
 */
typedef struct {
    EGNSSAltitudeType altitudeType; /**< Reference level for the GNSS altitude. */
    uint32_t validityBits;          /**< Bit mask indicating the validity of each corresponding value.
                                         [bitwise or'ed @ref EGNSSSimpleConfigurationValidityBits values].
                                         Must be checked before usage. */
} TGNSSSimpleConfiguration;

/**
 * TGNSSPosition::validityBits provides information about the currently valid signals of the GNSS position data.
 * It is a or'ed bitmask of the EGNSSPositionValidityBits values.
 */
typedef enum {
    GNSS_POSITION_LATITUDE_VALID    = 0x00000001,    /**< Validity bit for field TGNSSPosition::latitude. */
    GNSS_POSITION_LONGITUDE_VALID   = 0x00000002,    /**< Validity bit for field TGNSSPosition::longitude. */
    GNSS_POSITION_ALTITUDE_VALID    = 0x00000004,    /**< Validity bit for field TGNSSPosition::altitude. */
} EGNSSPositionValidityBits;

/**
 * Main position information.
 */
typedef struct {
    uint64_t timestamp;         /**< Timestamp of the acquisition of the position data. [ms] */
    double latitude;            /**< Latitude in WGS84 in degrees. */
    double longitude;           /**< Longitude in WGS84 in degrees. */
    float altitude;             /**< Altitude in [m]. See TGNSSSimpleConfiguration.typeOfAltitude for reference level (ellipsoid or MSL). */
    uint32_t validityBits;      /**< Bit mask indicating the validity of each corresponding value.
                                     [bitwise or'ed @ref EGNSSPositionValidityBits values].
                                     Must be checked before usage. */
} TGNSSPosition;

/**
 * TGNSSCourse::validityBits provides information about the currently valid signals of the GNSS course data.
 * It is a or'ed bitmask of the EGNSSCourseValidityBits values.
 */
typedef enum {
    GNSS_COURSE_SPEED_VALID         = 0x00000001,    /**< Validity bit for field TGNSSCourse::speed. */
    GNSS_COURSE_CLIMB_VALID         = 0x00000002,    /**< Validity bit for field TGNSSCourse::climb. */
    GNSS_COURSE_HEADING_VALID       = 0x00000004,    /**< Validity bit for field TGNSSCourse::heading. */
} EGNSSCourseValidityBits;

/**
 * GNSS course provides information about the currently course of the receiver.
 * There is an extended service providing the speed for each axis seperately in GNSS-Extended.
 */
typedef struct {
    uint64_t timestamp;         /**< Timestamp of the acquisition of the GNSS course signal in [ms].
                                     To enable an accurate DR filtering a defined clock has to be used. */
    float speed;                /**< Speed measured by the GPS receiver [m/s]. */
    float climb;                /**< Incline / decline in degrees [degree]. */
    float heading;              /**< GNSS course angle [degree] (0 => north, 90 => east, 180 => south, 270 => west, no negative values). */
    uint32_t validityBits;      /**< Bit mask indicating the validity of each corresponding value.
                                     [bitwise or'ed @ref EGNSSCourseValidityBits values].
                                     Must be checked before usage. */
} TGNSSCourse;

/**
 * Callback type for GNSS position.
 * Use this type of callback if you want to register for GNSS position data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps 
 * @param pos pointer to an array of TGNSSPosition with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.
 */
typedef void (*GNSSPositionCallback)(const TGNSSPosition pos[], uint16_t numElements);

/**
 * Callback type for GNSS course.
 * Use this type of callback if you want to register for GNSS course data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps 
 * @param course pointer to an array of TGNSSCourse with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*GNSSCourseCallback)(const TGNSSCourse course[], uint16_t numElements);

/**
 * Initialization of the GNSS service.
 * Must be called before using the GNSS service to set up the service.
 * @return True if initialization has been successfull.
 */
bool gnssSimpleInit();

/**
 * Destroy the GNSS service.
 * Must be called after using the GNSS service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool gnssSimpleDestroy();

/**
 * Accessing static configuration information about the GNSS sensor.
 * @param gnssConfig After calling the method the currently available GNSS configuration data is written into gnssConfig.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool gnssSimpleGetConfiguration(TGNSSSimpleConfiguration* gnssConfig);

/**
 * Provide meta information about GNSS service.
 * The meta data of a service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool gnssSimpleGetMetaData(TGnssMetaData *data);

/**
 * Method to get the GNSS position data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param pos After calling the method the currently available position data is written into this parameter.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool gnssSimpleGetPosition(TGNSSPosition* pos);

/**
 * Register GNSS position callback.
 * The callback will be invoked when new position data is available from the GNSS receiver.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool gnssSimpleRegisterPositionCallback(GNSSPositionCallback callback);

/**
 * Deregister GNSS Position callback.
 * After calling this method no new GNSS position data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool gnssSimpleDeregisterPositionCallback(GNSSPositionCallback callback);

/**
 * Method to get the GNSS course data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param course After calling the method the currently available position data is written into this parameter.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool gnssSimpleGetCourse(TGNSSCourse* course);

/**
 * Register GNSS course callback.
 * The callback will be invoked when new course data is available from the GNSS receiver.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool gnssSimpleRegisterCourseCallback(GNSSCourseCallback callback);

/**
 * Deregister GNSS course callback.
 * After calling this method no new GNSS course data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool gnssSimpleDeregisterCourseCallback(GNSSCourseCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_GNSS_SIMPLE */
