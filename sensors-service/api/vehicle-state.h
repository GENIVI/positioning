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

#ifndef INCLUDED_GENIVI_VEHICLESTATE
#define INCLUDED_GENIVI_VEHICLESTATE

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TVehicleStateData::validityBits provides information about the currently valid signals of the vehicle state data.
 * It is a or'ed bitmask of the EVehicleStateValidityBits values.
 */
typedef enum {
    VEHICLESTATE_ANTILOCKBRAKESYSTEMACTIVE_VALID            = 0x00000001,   /**< Validity bit for field antiLockBrakeSystemActive::antiLockBrakeSystemActive. */
    VEHICLESTATE_BRAKEACTIVE_VALID                          = 0x00000002,   /**< Validity bit for field antiLockBrakeSystemActive::brakeActive. */
    VEHICLESTATE_ELECTRONICSTABILITYPROGRAMACTIVE_VALID     = 0x00000004,   /**< Validity bit for field antiLockBrakeSystemActive::electronicStabilityProgramActive. */
    VEHICLESTATE_TRACTIONCONTROLACTIVE_VALID                = 0x00000008,   /**< Validity bit for field antiLockBrakeSystemActive::tractionControlActive. */
} EVehicleStateValidityBits;

/**
 * The VehicleStateData delivers the sensor values of the vehicle state.
 * You must check the valid bits before usage.
 */
typedef struct {
    uint64_t timestamp;                         /**< Timestamp of the acquisition of the accelerometer signal [ms].
                                                     All sensor/GNSS timestamps must be based on the same time source. */
    bool antiLockBrakeSystemActive;             /**< If true and signal is valid the ABS is currently engaged. */
    bool brakeActive;                           /**< If true and signal is valid the brakes are currently engaged. */
    bool electronicStabilityProgramActive;      /**< If true and signal is valid the electronic stability system (ESP or DSC) is currently engaged. */
    bool tractionControlActive;                 /**< If true and signal is valid the traction control (ASR) is currently engaged. */
    uint32_t validityBits;                      /**< Bit mask indicating the validity of each corresponding value.
                                                    [bitwise or'ed @ref EVehicleStateValidityBits values].
                                                    Must be checked before usage. */
} TVehicleStateData;

/**
 * Callback type for vehicle state sensor service.
 * Use this type of callback if you want to register for vehicle state data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param vehicleSpeedData pointer to an array of TVehicleStateData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided.
 */
typedef void (*VehicleStateCallback)(const TVehicleStateData vehicleState[], uint16_t numElements);

/**
 * Initialization of the vehicle state sensor service.
 * Must be called before using the vehicle state sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsVehicleStateInit();

/**
 * Destroy the VehicleState sensor service.
 * Must be called after using the VehicleState sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsVehicleStateDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsVehicleStateGetMetaData(TSensorMetaData *data);

/**
 * Method to get the vehicle state data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param vehicleState After calling the method the currently available vehicle state data is written into vehicleState.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsVehicleStateGetVehicleStateData(TVehicleStateData* vehicleState);

/**
 * Register vehicle state callback.
 * This is the recommended method for continuously accessing the vehicle state data.
 * The callback will be invoked when new vehicle state data is available.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsVehicleStateRegisterCallback(VehicleStateCallback callback);

/**
 * Deregister vehicle state callback.
 * After calling this method no new vehicle state data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsVehicleStateDeregisterCallback(VehicleStateCallback callback);

/**
 * Method to get the vehicle state sensor status at a specific point in time.
 * @param status After calling the method the current vehicle state sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsVehicleStateGetStatus(TSensorStatus* status);

/**
 * Register vehicle state sensor status callback.
 * This is the recommended method for continuously monitoring the vehicle state sensor status.
 * The callback will be invoked when new vehicle state sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsVehicleStateRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister vehicle state sensor status callback.
 * After calling this method no new vehicle state sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsVehicleStateDeregisterStatusCallback(SensorStatusCallback callback);


#ifdef __cplusplus
}
#endif
#endif /* INCLUDED_GENIVI_VEHICLESTATE */
