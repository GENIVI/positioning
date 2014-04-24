/**************************************************************************
 * @licence app begin@
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * \ingroup SensorsService
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

#ifndef INCLUDE_SNS_METADATA
#define INCLUDE_SNS_METADATA

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The sensor category introduces the concept that sensor information can also be derived information
 * computed by combining several signals.
 */
typedef enum {
    SENSOR_CATEGORY_UNKNOWN,       /**< Unknown category. Should not be used. */
    SENSOR_CATEGORY_LOGICAL,       /**< A logical sensor can combine signals of several sensors. */
    SENSOR_CATEGORY_PHYSICAL       /**< A physical sensor provides signals from physically available sensors. */
} ESensorCategory;

/**
 * The sensor type identifies which physical quantity is measured.
 * For each sensor type, there is a corresponding API header with data structures, callback notifications and getter functions defined
 * Note that for all 3 wheel sensor types there is a combined API header.
 *
 */
typedef enum {
    SENSOR_TYPE_UNKNOWN,             /**< Unknown sensor type. Should not be used. */
    SENSOR_TYPE_ACCELERATION,        /**< Acceleration sensor*/
    SENSOR_TYPE_GYROSCOPE,           /**< Gyroscope sensor*/
    SENSOR_TYPE_INCLINATION,         /**< Inclination sensor*/
    SENSOR_TYPE_ODOMETER,            /**< Odometer sensor*/
    SENSOR_TYPE_REVERSE_GEAR,        /**< Reverse gear sensor*/
    SENSOR_TYPE_SLIP_ANGLE,          /**< Slip angle sensor*/
    SENSOR_TYPE_STEERING_ANGLE,      /**< Steering angle sensor*/
    SENSOR_TYPE_VEHICLE_SPEED,       /**< Vehicle speed sensor*/
    SENSOR_TYPE_VEHICLE_STATE,       /**< Vehicle state sensor*/
    SENSOR_TYPE_WHEELTICK,           /**< Wheel tick sensor*/
    SENSOR_TYPE_WHEELSPEEDANGULAR,   /**< Wheel speed angular sensor*/
    SENSOR_TYPE_WHEELSPEED,          /**< Wheel speed sensor*/
} ESensorType;

/**
 * The software platform provides the following information about the Sensor and the related output Signals.
 * Sensor clients need the meta data information in order to correctly handle data provided by sensor service and
 * to adapt to the variation in the signal data delivery.
 */
typedef struct {
    uint32_t version;               /**< Version of the sensor service. */
    ESensorCategory category;       /**< Sensor Category (Physical/Logical). */
    ESensorType type;               /**< Sensor Type (Odometer, Gyroscope, etc.). */
    uint32_t cycleTime;             /**< Sensor cycle time (update interval) in ms. 0 for irregular updates */
} TSensorMetaData;

/**
 * Retrieve the metadata of all available sensors.
 * @param metadata returns a a pointer an array of TSensorMetaData (maybe NULL if no metadata is available)
 * @return number of elements in the array of TSensorMetaData
 */
int32_t getSensorMetaDataList(const TSensorMetaData** metadata);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SNS_METADATA */
