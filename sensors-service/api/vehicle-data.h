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

#ifndef INCLUDE_GENIVI_VEHICLE_DATA
#define INCLUDE_GENIVI_VEHICLE_DATA

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/**
 * Description of driven axles. This is currently restricted to passenger cars.
 */
typedef enum {
    SNS_AXLE_UNKNOWN = 0,   /**< It is not known which axles are driven. */
    SNS_AXLE_FRONT = 1,     /**< Only the front axle is driven. */
    SNS_AXLE_REAR = 2,      /**< Only the rear axle is driven. */
    SNS_AXLE_BOTH = 3       /**< Both axles are driven (4 wheel drive). */
} EAxleType;

/**
 * Description of the vehicle type. This is for future extensions.
 * Currently the specification is based mostly on cars. Other vehicle types are just referenced for future extensions.
 */
typedef enum {
    SNS_CAR = 0,                /**< Passenger car with 4 wheels. */
    SNS_TRUCK = 1,              /**< Truck */
    SNS_MOTORBIKE = 2,          /**< Motorbike with 2 wheels. */
    SNS_BUS = 3                 /**< Passenger bus. */
} EVehicleType;

/**
 * 3 dimensional distance used for description of geometric descriptions within the vehicle reference system.
 */
typedef struct {
    float x;        /**< Distance in x direction in [m] according to the reference coordinate system. */
    float y;        /**< Distance in y direction in [m] according to the reference coordinate system. */
    float z;        /**< Distance in z direction in [m] according to the reference coordinate system. */
} TDistance3D;


/**
 * Type of the vehicle. This is a static configuration.
 * @param type The vehicle type is written to this parameter as a result as defined in the enumeration.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetVehicleType(EVehicleType* type);

/**
 * Type of the driven axles of the vehicle as provided in the official documents. This is a static configuration.
 * @param axles The driven axles type is written to this parameter as a result as defined in the enumeration.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetDrivenAxles(EAxleType* axles);

/**
 * Number of seats of the vehicle as provided in the official documents. This is a static configuration.
 * @param seatCount The number of seats is written to this parameter as a result.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetSeatCount(uint8_t* seatCount);

/**
 * Track width of the vehicle as provided in the official documents. This is a static configuration. Unit: [m].
 * If the vehicle has different track widths at the front and rear axles, the rear axle track is referred to.
 * @param width The vehicle track width in m is written to this parameter as a result.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetTrackWidth(float* width);

/**
 * Front axle track width of the vehicle as provided in the official documents. This is a static configuration. Unit: [m].
 * @param width The vehicle track width of the front axle in m is written to this parameter as a result.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetFrontAxleTrackWidth(float* width);

/**
 * Wheel base of the vehicle as provided in the official documents. This is a static configuration. Unit: [m].
 * The wheel base is basically the distance between the front axle and the rear axle. 
 * For an exact definition, see ISO 8855.
 * @param width The wheel base in m is written to this parameter as a result.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetWheelBase(float* wheelbase);

/**
 * Mass of the vehicle as provided in the official documents. This is a static configuration value and does not include the current load. Unit: [kg].
 * @param mass The vehicle mass in kg is written to this parameter as a result.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetVehicleMass(float* mass);

/**
 * Width of the vehicle as provided in the official documents. This is a static configuration. Unit: [m].
 * @param width The vehicle width in m is written to this parameter as a result.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetVehicleWidth(float* width);

/**
 * Distance of the center of gravity to the reference point in 3 dimensions. Unit: [m].
 * @param dist The distance result in 3 dimensions is written to this parameter.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetDistCoG2RefPoint(TDistance3D *dist);

/**
 * Distance of front axle to the reference point in 3 dimensions. Unit: [m].
 * @param dist The distance result in 3 dimensions is written to this parameter.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetDistFrontAxle2RefPoint(TDistance3D *dist);

/**
 * Distance of rear axle to the reference point in 3 dimensions. Unit: [m].
 * @param dist The distance result in 3 dimensions is written to this parameter.
 * @return True if the configuration value is available. If false no value could be provided.
 */
bool vehicleDataGetDistRearAxle2RefPoint(TDistance3D *dist);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_VEHICLE_DATA */
