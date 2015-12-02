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

#ifndef INCLUDE_GENIVI_SNS_STATUS
#define INCLUDE_GENIVI_SNS_STATUS

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumeration to describe the status of the sensor
 */
typedef enum {
    SENSOR_STATUS_NOTAVAILABLE  = 0,    /**< Sensor is not available at all, based on configuration data. */
    SENSOR_STATUS_INITIALIZING  = 1,    /**< Initial status when the connection to the Sensor is set up for the first time. */
    SENSOR_STATUS_AVAILABLE     = 2,    /**< Sensor is available and running as expected. */
    SENSOR_STATUS_RESTARTING    = 3,    /**< Sensor is restarted, i.e. due to communication loss. */
    SENSOR_STATUS_FAILURE       = 4,    /**< Sensor is not operating properly. Restarting did not help. */
    SENSOR_STATUS_OUTOFSERVICE  = 5     /**< Sensor is temporarily not available, due to some known external condition, vehicle bus or external ECU providing he signal being off. */
} ESensorStatus;

/**
 * TSensorStatus::validityBits provides information about the currently valid signals of the TSensorStatus struct.
 * It is a or'ed bitmask of the ESensorStatusValidityBits values.
 */
typedef enum {
    SENSOR_STATUS_STATUS_VALID          = 0x00000001    /**< Validity bit for field TSensorStatus::status. */
} ESensorStatusValidityBits;

/**
 * Container for sensor status information
 */
typedef struct {
    uint64_t timestamp;             /**< Timestamp of the sensor status transition [ms].
                                         All sensor/GNSS timestamps must be based on the same time source. */
    ESensorStatus status;           /**< Status of the sensor */
    uint32_t validityBits;          /**< Bit mask indicating the validity of each corresponding value.
                                        [bitwise or'ed @ref ESensorStatusValidityBits values].
                                        Must be checked before usage. */
} TSensorStatus;

/**
 * Callback type for sensor status.
 * Use this type of callback if you want to register for sensor status updates data.
 * @param status the  sensor status
 */
typedef void (*SensorStatusCallback)(const TSensorStatus *status);

#ifdef __cplusplus
}
#endif

#endif //#ifndef INCLUDE_GENIVI_SNS_STATUS
