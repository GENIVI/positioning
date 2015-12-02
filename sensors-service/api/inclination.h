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

#ifndef INCLUDED_GENIVI_INCLINATION
#define INCLUDED_GENIVI_INCLINATION

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Current status of the inclination sensors signals.
 */
typedef enum {
    INCLINATION_INITIALIZING,                           /**< System is currently in the initialization phase. */
    INCLINATION_SENSOR_BASED,                           /**< Signals are available and are based on sensors. */
    INCLINATION_UNSTABLE_DRIVING_CONDITION,             /**< Unstable driving conditions (limited accuracy). */
    INCLINATION_MODEL_BASED,                            /**< Signals are available and are based on a model. */
    INCLINATION_NOT_AVAILABLE                           /**< Signals are not available. */
} EInclinationSensorStatus;

/**
 * TInclinationData::validityBits provides information about the currently valid signals of the inclination data.
 * It is a or'ed bitmask of the EInclinationValidityBits values.
 */
typedef enum {
    INCLINATION_LONGITUDINAL_VALID  = 0x00000001,    /**< Validity bit for field TInclinationData::longitudinalGradientRoadway. */
    INCLINATION_TRAVERSE_VALID      = 0x00000002,    /**< Validity bit for field TInclinationData::traverseGradientRoadway. */
    INCLINATION_STATUS_VALID        = 0x00000004     /**< Validity bit for field TInclinationData::status. */
} EInclinationValidityBits;

/**
 * Inclination sensor service provides the inclination values.
 */
typedef struct {
    uint64_t timestamp;                     /**< Timestamp of the acquisition of the inclination signal [ms].
                                                 All sensor/GNSS timestamps must be based on the same time source. */
    float longitudinalGradientRoadway;      /**< The inclination of the road in longitudinal direction, i.e. in the direction of movement [degree]. In instable driving situations this value might not be available. */
    float traverseGradientRoadway;          /**< Estimated gradient of the road in transverse direction [degree]. In instable driving situations this value might not be available. */
    EInclinationSensorStatus status;        /**< Status of the signals. */
    uint32_t validityBits;                  /**< Bit mask indicating the validity of each corresponding value.
                                                [bitwise or'ed @ref EInclinationValidityBits values].
                                                Must be checked before usage. */
} TInclinationData;

/**
 * Callback type for inclination sensor service.
 * Use this type of callback if you want to register for inclination data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param inclinationData pointer to an array of TInclinationData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.  
 */
typedef void (*InclinationCallback)(const TInclinationData inclinationData[], uint16_t numElements);

/**
 * Initialization of the inclination sensor service.
 * Must be called before using the inclination sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsInclinationInit();

/**
 * Destroy the inclination sensor service.
 * Must be called after using the inclination sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsInclinationDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsInclinationGetMetaData(TSensorMetaData *data);

/**
 * Method to get the inclination data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param inclination After calling the method the currently available inclination data is written into this parameter.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsInclinationGetInclinationData(TInclinationData* inclination);

/**
 * Register inclination callback.
 * This is the recommended method for continuously accessing the inclination data.
 * The callback will be invoked when new inclination data is available.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsInclinationRegisterCallback(InclinationCallback callback);

/**
 * Deregister inclination callback.
 * After calling this method no new inclination data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsInclinationDeregisterCallback(InclinationCallback callback);

/**
 * Method to get the inclination sensor status at a specific point in time.
 * @param status After calling the method the current inclination sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsInclinationGetStatus(TSensorStatus* status);

/**
 * Register inclination sensor status callback.
 * This is the recommended method for continuously monitoring the inclination sensor status.
 * The callback will be invoked when new inclination sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsInclinationRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister inclination sensor status callback.
 * After calling this method no new inclination sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsInclinationDeregisterStatusCallback(SensorStatusCallback callback);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_GENIVI_INCLINATION */
