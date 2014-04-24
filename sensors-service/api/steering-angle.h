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

#ifndef INCLUDE_GENIVI_STEERINGANGLE
#define INCLUDE_GENIVI_STEERINGANGLE

#include "sns-meta-data.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TSteeringAngleData::validityBits provides information about the currently valid signals of the steering angle data.
 * It is a or'ed bitmask of the ESteeringAngleValidityBits values.
 */
typedef enum {
    STEERINGANGLE_FRONT_VALID           = 0x00000001,   /**< Validity bit for field TSteeringAngleData::front. */
    STEERINGANGLE_REAR_VALID            = 0x00000002,   /**< Validity bit for field TSteeringAngleData::rear. */
    STEERINGANGLE_STEERINGWHEEL_VALID   = 0x00000004    /**< Validity bit for field TSteeringAngleData::steeringWheel. */
} ESteeringAngleValidityBits;

/**
 * The SteeringAngle delivers the sensor values of the steering angle.
 * You must check the valid bits before usage.
 */
typedef struct {
    uint64_t timestamp;             /**< Timestamp of the acquisition of the steering angle signal [ms].
                                         All sensor/GNSS timestamps must be based on the same time source. */
    float front;                    /**< Returns the steering angle of the front wheels [degree]. */
    float rear;                     /**< Returns the steering angle of the rear wheels [degree]. This is only useful for vehicles with a steerable rear axis. */
    float steeringWheel;            /**< Returns the angle of the steering wheel [degree]. Useful for vehicles where the wheel angles are not available.
                                         Must be used in combination with the steeringRatio. */
    uint32_t validityBits;          /**< Bit mask indicating the validity of each corresponding value.
                                        [bitwise or'ed @ref ESteeringAngleValidityBits values].
                                        Must be checked before usage. */
} TSteeringAngleData;

/**
 * The SteeringAngleConfiguration delivers the static configuration values of the steering wheel sensor service.
 */
typedef struct {
    float sigmaSteeringAngle;       /**< Standard deviation of the steering front angle in [degree]. */
    float sigmaSteeringWheelAngle;  /**< Standard deviation of the steering wheel angle in [degree]. */
    float steeringRatio;            /**< Steering ratio between steering wheel and wheels. Only valid when static: Unit: [-] */
} TSteeringAngleConfiguration;

/**
 * Callback type for steering angle sensor service.
 * Use this type of callback if you want to register for steering angle data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param steeringAngleData pointer to an array of TSteeringAngleData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.  
 */
typedef void (*SteeringAngleCallback)(const TSteeringAngleData steeringAngleData[], uint16_t numElements);

/**
 * Initialization of the steering angle sensor service.
 * Must be called before using the steering angle sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsSteeringAngleInit();

/**
 * Destroy the SteeringAngle sensor service.
 * Must be called after using the SteeringAngle sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsSteeringAngleDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsSteeringAngleGetMetaData(TSensorMetaData *data);

/**
 * Method to get the steering angle data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param angleData After calling the method the currently available steering angle data is written into angleData.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsSteeringAngleGetSteeringAngleData(TSteeringAngleData* angleData);

/**
 * Accessing static configuration information about the steering angle sensor.
 * @param config After calling the method the currently available static steering angle configuration data is written into config.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsSteeringAngleGetConfiguration(TSteeringAngleConfiguration* config);

/**
 * Register steering angle callback.
 * This is the recommended method for continuously accessing the steering angle data.
 * The callback will be invoked when new steering angle data is available.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsSteeringAngleRegisterCallback(SteeringAngleCallback callback);

/**
 * Deregister steering angle callback.
 * After calling this method no new steering angle data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsSteeringAngleDeregisterCallback(SteeringAngleCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_STEERINGANGLE */
