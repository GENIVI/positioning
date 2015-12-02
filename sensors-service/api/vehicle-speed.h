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

#ifndef INCLUDED_GENIVI_VEHICLESPEED
#define INCLUDED_GENIVI_VEHICLESPEED

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TVehicleSpeedData::validityBits provides information about the currently valid signals of the vehicle speed data.
 * It is a or'ed bitmask of the EVehicleSpeedValidityBits values.
 */
typedef enum {
    VEHICLESPEED_VEHICLESPEED_VALID       = 0x00000001    /**< Validity bit for field TVehicleSpeedData::vehicleSpeed. */
} EVehicleSpeedValidityBits;

/**
 * Vehicle speed sensor service provides the current speed of the vehicle.
 */
typedef struct {
    uint64_t timestamp;         /**< Timestamp of the acquisition of the vehicle speed signal [ms].
                                     All sensor/GNSS timestamps must be based on the same time source. */
    float vehicleSpeed;         /**< Filtered vehicle speed in [m/s] with a frequency of at least 5Hz. Direction is given by the sign of this value.*/
    uint32_t validityBits;      /**< Bit mask indicating the validity of each corresponding value.
                                    [bitwise or'ed @ref EVehicleSpeedValidityBits values].
                                    Must be checked before usage. */
} TVehicleSpeedData;

/**
 * Callback type for vehicle speed sensor service.
 * Use this type of callback if you want to register for vehicle speed data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param vehicleSpeedData pointer to an array of TVehicleSpeedData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*VehicleSpeedCallback)(const TVehicleSpeedData vehicleSpeedData[], uint16_t numElements);

/**
 * Initialization of the vehicle speed sensor service.
 * Must be called before using the vehicle speed sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsVehicleSpeedInit();

/**
 * Destroy the VehicleSpeed sensor service.
 * Must be called after using the VehicleSpeed sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsVehicleSpeedDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsVehicleSpeedGetMetaData(TSensorMetaData *data);

/**
 * Method to get the vehicle speed data at a specific point in time.
 * @param vehicleSpeed After calling the method the currently available vehicle speed data is written into vehicleSpeed.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsVehicleSpeedGetVehicleSpeedData(TVehicleSpeedData* vehicleSpeed);

/**
 * Register vehicle speed callback.
 * The callback will be invoked when new vehicle speed data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsVehicleSpeedRegisterCallback(VehicleSpeedCallback callback);

/**
 * Deregister vehicle speed callback.
 * After calling this method no new vehicle speed data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsVehicleSpeedDeregisterCallback(VehicleSpeedCallback callback);

/**
 * Method to get the vehicle speed sensor status at a specific point in time.
 * @param status After calling the method the current vehicle speed sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsVehicleSpeedGetStatus(TSensorStatus* status);

/**
 * Register vehicle speed sensor status callback.
 * This is the recommended method for continuously monitoring the vehicle speed sensor status.
 * The callback will be invoked when new vehicle speed sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsVehicleSpeedRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister vehicle speed sensor status callback.
 * After calling this method no new vehicle speed sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsVehicleSpeedDeregisterStatusCallback(SensorStatusCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_GENIVI_VEHICLESPEED */
