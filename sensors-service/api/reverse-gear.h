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

#ifndef INCLUDED_GENIVI_REVERSEGEAR
#define INCLUDED_GENIVI_REVERSEGEAR

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TReverseGearData::validityBits provides information about the currently valid signals of the reverse gear data.
 * It is a or'ed bitmask of the EReverseGearValidityBits values.
 */
typedef enum {
    REVERSEGEAR_REVERSEGEAR_VALID    = 0x00000001    /**< Validity bit for field TReverseGearData::isReverseGear. */
} EReverseGearValidityBits;

/**
 * Reverse gear sensor service provides the current status of the reverse gear of the vehicle.
 * This information is explicitly restricted to provide only the information if the reverse gear is engaged.
 * The direction of movement is provided by the direction of the vehicle speed.
 */
typedef struct {
    uint64_t timestamp;                 /**< Timestamp of the acquisition of the reverse gear signal [ms].
                                             All sensor/GNSS timestamps must be based on the same time source. */
    bool isReverseGear;                 /**< True if the reverse gear is currently used. False otherwise. */
    uint32_t validityBits;              /**< Bit mask indicating the validity of each corresponding value.
                                            [bitwise or'ed @ref EReverseGearValidityBits values].
                                            Must be checked before usage. */
} TReverseGearData;

/**
 * Callback type for reverse gear sensor service.
 * Use this type of callback if you want to register for reverse gear data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param reverseGearData pointer to an array of TReverseGearData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.  
 */
typedef void (*ReverseGearCallback)(const TReverseGearData reverseGearData[], uint16_t numElements);

/**
 * Initialization of the reverse gear sensor service.
 * Must be called before using the reverse gear sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsReverseGearInit();

/**
 * Destroy the ReverseGear sensor service.
 * Must be called after using the ReverseGear sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsReverseGearDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsReverseGearGetMetaData(TSensorMetaData *data);

/**
 * Method to get the reverse gear data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param reverseGear After calling the method the currently available reverse gear data is written into reverseGear.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsReverseGearGetReverseGearData(TReverseGearData* reverseGear);

/**
 * Register reverse gear callback.
 * The callback will be invoked when new reverse gear data is available.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsReverseGearRegisterCallback(ReverseGearCallback callback);

/**
 * Deregister reverse gear callback.
 * After calling this method no new reverse gear data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsReverseGearDeregisterCallback(ReverseGearCallback callback);

/**
 * Method to get the reverse gear sensor status at a specific point in time.
 * @param status After calling the method the current reverse gear sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsReverseGearGetStatus(TSensorStatus* status);

/**
 * Register reverse gear sensor status callback.
 * This is the recommended method for continuously monitoring the reverse gear sensor status.
 * The callback will be invoked when new reverse gear sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsReverseGearRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister reverse gear sensor status callback.
 * After calling this method no new reverse gear sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsReverseGearDeregisterStatusCallback(SensorStatusCallback callback);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_GENIVI_REVERSEGEAR */
