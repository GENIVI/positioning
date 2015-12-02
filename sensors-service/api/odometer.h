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

#ifndef INCLUDE_GENIVI_ODOMETER
#define INCLUDE_GENIVI_ODOMETER

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TOdometerData::validityBits provides information about the currently valid signals of the odometer data.
 * It is a or'ed bitmask of the EOdometerValidityBits values.
 */
typedef enum {
    ODOMETER_TRAVELLEDDISTANCE_VALID    = 0x00000001    /**< Validity bit for field TOdometerData::travelledDistance. */
} EOdometerValidityBits;

/**
 * Odometer sensor service provides the travelled distance.
 */
typedef struct {
    uint64_t timestamp;             /**< Timestamp of the acquisition of the odometer signal [ms].
                                         All sensor/GNSS timestamps must be based on the same time source. */
    uint16_t travelledDistance;     /**< Distance in [cm] with at least 5Hz. Implemented as a running counter
                                         with overflow to support multiple clients and getter methods.
                                         As the representation of this value is done using 16 Bits the value can provide
                                         distances up 32767cm or 327.67m before overflowing. */
    uint32_t validityBits;          /**< Bit mask indicating the validity of each corresponding value.
                                        [bitwise or'ed @ref EOdometerValidityBits values].
                                        Must be checked before usage. */
} TOdometerData;

/**
 * Callback type for odometer sensor service.
 * Use this type of callback if you want to register for odometer data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param odometerData pointer to an array of TOdometerData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.  
 */
typedef void (*OdometerCallback)(const TOdometerData odometerData[], uint16_t numElements);

/**
 * Initialization of the odometer sensor service.
 * Must be called before using the odometer sensor service to set up the service.
 * @return True if initialization has been successfull.
 *  */
bool snsOdometerInit();

/**
 * Destroy the odometer sensor service.
 * Must be called after using the odometer sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsOdometerDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsOdometerGetMetaData(TSensorMetaData *data);

/**
 * Method to get the odometer data at a specific point in time.
 * Be careful when using this method to read data often enough to avoid missing an overflow.
 * With reasonable car speeds it should be enough to read the data every 3s.
 * The recommended usage for the odometer data is the callback interface.
 * The get method is provided for consistency reasons of the sensor service API and might be used
 * for determining the distance between a few points in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param odometer After calling the method the currently available odometer data is written into this parameter.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsOdometerGetOdometerData(TOdometerData* odometer);

/**
 * Register odometer callback.
 * This is the recommended method for continuously accessing the odometer data.
 * The callback will be invoked when new odometer data is available.
 * Overflow handling must be done by the caller.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsOdometerRegisterCallback(OdometerCallback callback);

/**
 * Deregister odometer callback.
 * After calling this method no new odometer data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsOdometerDeregisterCallback(OdometerCallback callback);

/**
 * Method to get the odometer sensor status at a specific point in time.
 * @param status After calling the method the current odometer sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsOdometerGetStatus(TSensorStatus* status);

/**
 * Register odometer sensor status callback.
 * This is the recommended method for continuously monitoring the odometer sensor status.
 * The callback will be invoked when new odometer sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsOdometerRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister odometer sensor status callback.
 * After calling this method no new odometer sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsOdometerDeregisterStatusCallback(SensorStatusCallback callback);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_ODOMETER */
