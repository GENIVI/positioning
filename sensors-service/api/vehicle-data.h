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
 *
 * The vehicle axis system as defined in ISO 8855:2011(E).
 * In this system, the axes (Xv, Yv, Zv) are oriented as follows
 * - Xv is in the horizontal plane, pointing forwards
 * - Yv is in the horizontal plane, pointing to the left
 * - Zv is perpendicular to the horizontal plane, pointing upwards
 * For an illustration, see https://collab.genivi.org/wiki/display/genivi/LBSSensorServiceRequirementsBorg#LBSSensorServiceRequirementsBorg-ReferenceSystem
 *
 * The reference point of the vehicle lies underneath the center of the rear axle on the surface of the road.
 */
typedef struct {
    float x;        /**< Distance in x direction in [m] according to the reference coordinate system. */
    float y;        /**< Distance in y direction in [m] according to the reference coordinate system. */
    float z;        /**< Distance in z direction in [m] according to the reference coordinate system. */
} TDistance3D;


/**
 * TVehicleData::validityBits provides information about the valid fields of the vehicle data.
 * It is a or'ed bitmask of the EVehicleDataConfigurationValidityBits values.
 */
typedef enum {
    VEHICLESTATUS_VEHICLETYPE_VALID             = 0x00000001,    /**< Validity bit for field TVehicleDataConfiguration::vehicleType. */
    VEHICLESTATUS_DRIVENAXLES_VALID             = 0x00000002,    /**< Validity bit for field TVehicleDataConfiguration::drivenAxles. */
    VEHICLESTATUS_SEATCOUNT_VALID               = 0x00000004,    /**< Validity bit for field TVehicleDataConfiguration::seatCount. */
    VEHICLESTATUS_TRACKWIDTH_VALID              = 0x00000008,    /**< Validity bit for field TVehicleDataConfiguration::trackWidth. */
    VEHICLESTATUS_FRONTAXLETRACKWIDTH_VALID     = 0x00000010,    /**< Validity bit for field TVehicleDataConfiguration::frontAxleTrackWidth. */
    VEHICLESTATUS_WHELLBASE_VALID               = 0x00000020,    /**< Validity bit for field TVehicleDataConfiguration::wheelBase. */
    VEHICLESTATUS_VEHICLEMASS_VALID             = 0x00000040,    /**< Validity bit for field TVehicleDataConfiguration::vehicleMass. */
    VEHICLESTATUS_VEHICLEWIDTH_VALID            = 0x00000080,    /**< Validity bit for field TVehicleDataConfiguration::vehicleWidth. */
    VEHICLESTATUS_DISTCOG2REFPOINT_VALID        = 0x00000100,    /**< Validity bit for field TVehicleDataConfiguration::distCoG2RefPoint. */
    VEHICLESTATUS_DISTFRONTAXLE2REFPOINT_VALID  = 0x00000200,    /**< Validity bit for field TVehicleDataConfiguration::distFrontAxle2RefPoint. */
    VEHICLESTATUS_DISTREARAXLE2REFPOINT_VALID   = 0x00000400     /**< Validity bit for field TVehicleDataConfiguration::vehicleType. */
} EVehicleDataConfigurationValidityBits;


/**
 * Static configuration data of the vehicle which are relevant for positioning.
 */
typedef struct {
    EVehicleType    vehicleType;            /**< Type of the vehicle. */
    EAxleType       drivenAxles;            /**< Type of the driven axles of the vehicle. */
    uint8_t         seatCount;              /**< Number of seats of the vehicle. */
    float           trackWidth;             /**< Track width of the vehicle. Unit: [m]. 
                                                 If the vehicle has different track widths at the front and rear axles, the rear axle track is referred to. */
    float           frontAxleTrackWidth;    /**< Front axle track width of the vehicle. Unit: [m]. */
    float           wheelBase;              /**< Wheel base of the vehicle. Unit: [m].
                                                 The wheel base is basically the distance between the front axle and the rear axle.
                                                 For an exact definition, see ISO 8855. */
    float           vehicleMass;            /**< Mass of the vehicle not including the current load. Unit: [kg].*/
    float           vehicleWidth;           /**< Width of the vehicle. Unit: [m]. */
    TDistance3D     distCoG2RefPoint;       /**< Distance of the center of gravity to the reference point in 3 dimensions. Unit: [m]. */
    TDistance3D     distFrontAxle2RefPoint; /**< Distance of front axle to the reference point in 3 dimensions. Unit: [m]. */
    TDistance3D     distRearAxle2RefPoint;  /**< Distance of rear axle to the reference point in 3 dimensions. Unit: [m]. */
    uint32_t        validityBits;           /**< Bit mask indicating the validity of each corresponding value.
                                                 [bitwise or'ed @ref EVehicleDataConfigurationValidityBits values].
                                                 Must be checked before usage. */
} TVehicleDataConfiguration;


/**
 * Initialization of the vehicle data sensor service.
 * Must be called before using the vehicle data sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsVehicleDataInit();

/**
 * Destroy the vehicle data sensor service.
 * Must be called after using the vehicle data sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsVehicleDataDestroy();

/**
 * Accessing static configuration information about the vehicle.
 * @param vehicleData After calling the method the available vehicle configuration data is written into vehicleData.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsVehicleDataGetConfiguration(TVehicleDataConfiguration* vehicleDataConfiguration);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_VEHICLE_DATA */
