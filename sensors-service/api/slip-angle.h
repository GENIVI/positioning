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

#ifndef INCLUDE_GENIVI_SLIP_ANGLE
#define INCLUDE_GENIVI_SLIP_ANGLE

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TSlipAngleData::validityBits provides information about the currently valid signals of the slip angle data.
 * It is a or'ed bitmask of the ESlipAngleValidityBits values.
 */
typedef enum {
    SLIPANGLE_SLIPANGLE_VALID       = 0x00000001    /**< Validity bit for field TSlipAngleData::slipAngle. */
} ESlipAngleValidityBits;

/**
 * Slip angle sensor service provides the slip angle value. 
 * @ref ISO 8855:2011, section 5.2.9 "vehicle sideslip angle"
 * The reference coordinate system for the slip angle is defined in ISO 8855:2011
 *
 * You must check the valid bit(s) before usage.
 */
typedef struct {
    uint64_t timestamp;                 /**< Timestamp of the acquisition of the slip angle signal [ms].
                                             All sensor/GNSS timestamps must be based on the same time source. */
    float slipAngle;                    /**< Delivers the value slip angle in [degrees].
                                             It is defined as the angle between the fixed car axis (direction of driving) and the real direction of vehicle movement.
                                             The direction and sign is defined equal to the yaw rate. */
    uint32_t validityBits;              /**< Bit mask indicating the validity of each corresponding value.
                                            [bitwise or'ed @ref ESlipAngleValidityBits values].
                                            Must be checked before usage. */
} TSlipAngleData;

/**
 * Callback type for slip angle sensor service.
 * Use this type of callback if you want to register for slip angle data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param slipAngleData pointer to an array of TSlipAngleData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.  
 */
typedef void (*SlipAngleCallback)(const TSlipAngleData slipAngleData[], uint16_t numElements);

/**
 * Initialization of the slip angle sensor service.
 * Must be called before using the slip angle sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsSlipAngleInit();

/**
 * Destroy the SlipAngle sensor service.
 * Must be called after using the SlipAngle sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsSlipAngleDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsSlipAngleGetMetaData(TSensorMetaData *data);

/**
 * Method to get the slip angle data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param data After calling the method the currently available inclination data is written into this parameter.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsSlipAngleGetSlipAngleData(TSlipAngleData* data);

/**
 * Register slip angle callback.
 * This is the recommended method for continuously accessing the slip angle data.
 * The callback will be invoked when new slip angle data is available.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsSlipAngleRegisterCallback(SlipAngleCallback callback);

/**
 * Deregister slip angle callback.
 * After calling this method no new slip angle data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsSlipAngleDeregisterCallback(SlipAngleCallback callback);

/**
 * Method to get the slip angle sensor status at a specific point in time.
 * @param status After calling the method the current slip angle sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsSlipAngleGetStatus(TSensorStatus* status);

/**
 * Register slip angle sensor status callback.
 * This is the recommended method for continuously monitoring the slip angle sensor status.
 * The callback will be invoked when new slip angle sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsSlipAngleRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister slip angle sensor status callback.
 * After calling this method no new slip angle sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsSlipAngleDeregisterStatusCallback(SensorStatusCallback callback);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_SLIP_ANGLE */
