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

#ifndef INCLUDED_GENIVI_ACCELERATION
#define INCLUDED_GENIVI_ACCELERATION

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TAccelerationConfiguration::validityBits provides information about the currently valid signals of the acceleration configuration data.
 * It is a or'ed bitmask of the EAccelerationConfigValidityBits values.
 */
typedef enum {
    ACCELERATION_CONFIG_DISTX_VALID     = 0x00000001,    /**< Validity bit for field TAccelerationConfiguration::dist2RefPointX. */
    ACCELERATION_CONFIG_DISTY_VALID     = 0x00000002,    /**< Validity bit for field TAccelerationConfiguration::dist2RefPointY. */
    ACCELERATION_CONFIG_DISTZ_VALID     = 0x00000004,    /**< Validity bit for field TAccelerationConfiguration::dist2RefPointZ. */
    ACCELERATION_CONFIG_ANGLEYAW_VALID  = 0x00000008,    /**< Validity bit for field TAccelerationConfiguration::angleYaw. */
    ACCELERATION_CONFIG_ANGLEPITCH_VALID= 0x00000010,    /**< Validity bit for field TAccelerationConfiguration::anglePitch. */
    ACCELERATION_CONFIG_ANGLEROLL_VALID = 0x00000020,    /**< Validity bit for field TAccelerationConfiguration::angleRoll. */
    ACCELERATION_CONFIG_SIGMAX_VALID    = 0x00000040,    /**< Validity bit for field TAccelerationConfiguration::sigmaX. */
    ACCELERATION_CONFIG_SIGMAY_VALID    = 0x00000080,    /**< Validity bit for field TAccelerationConfiguration::sigmaX. */
    ACCELERATION_CONFIG_SIGMAZ_VALID    = 0x00000100,    /**< Validity bit for field TAccelerationConfiguration::sigmaZ. */
    ACCELERATION_CONFIG_TYPE_VALID      = 0x00000200     /**< Validity bit for field TAccelerationConfiguration::typeBits. */    
} EAccelerationConfigValidityBits;


/**
 * Accelerometer type
 * TAccelerationConfiguration::typeBits provides information about the type of the accelerometer and the interpretation of the signals.
 * It is a or'ed bitmask of the EAccelerationTypeBits values.
 */
typedef enum {
    ACCELERATION_X_PROVIDED             = 0x00000001,   /**< An acceleration measurement for the x-axis is provided */
    ACCELERATION_Y_PROVIDED             = 0x00000002,   /**< An acceleration measurement for the y-axis is provided */
    ACCELERATION_Z_PROVIDED             = 0x00000004,   /**< An acceleration measurement for the z-axis is provided */
    ACCELERATION_TEMPERATURE_PROVIDED   = 0x00000008    /**< A measurement for the temperature is provided */
} EAccelerationTypeBits;


/**
 * Static configuration data for the acceleration sensor service.
 *
 * BEGIN Explanation of the angleYaw, anglePitch angleRoll parameters
 * The orientation of the accelerometer hardware (Xa, Ya, Za)
 * with respect to the vehicle axis system (Xv, Yv, Zv)
 * can be described using the angles (angleYaw, anglePitch, angleRoll)
 * following the approach defined in ISO 8855:2011, section 5.2, table 1
 * Apply 3 rotations on the vehicle axis system until it matches the accelerometer axis system
 * The rotation sequence is as follows
 *  - first rotate by angleYaw about the Zv axis
 *  - second rotate by anglePitch about the new (intermediate) Y axis
 *  - third rotate by angleRoll about the new X axis
 * Notes 
 * - the angles are frequently called "Euler angles" and the rotations "Euler rotations"
 * - a different order of the rotations would lead to a different orientation
 * - as the vehicle axis system is right-handed, also the accelerometer axis system must be right-handed
 *
 * The vehicle axis system as defined in ISO 8855:2011(E).
 * In this system, the axes (Xv, Yv, Zv) are oriented as follows
 * - Xv is in the horizontal plane, pointing forwards
 * - Yv is in the horizontal plane, pointing to the left
 * - Zv is perpendicular to the horizontal plane, pointing upwards
 * For an illustration, see https://collab.genivi.org/wiki/display/genivi/LBSSensorServiceRequirementsBorg#LBSSensorServiceRequirementsBorg-ReferenceSystem
 *
 * When the accelerometer axes are not aligned with the vehicle axes, i.e. 
 * if any of the angles (angleYaw, anglePitch, angleRoll) is not zero
 * then the raw measurement values of the accelerometer X, Y, Z axes may have to be be transformed
 * to the vehicle axis system by the client of this interface, depending on the type of application.
 * Raw measurements are provided in @TAccelerationData instead of already transformed values, because
 * - for accelerometers with less than 3 axes, the transformation is mathematically not well-defined
 * - some types of calibration operations are better performed on raw data
 *
 * Implementors hint: The mathematics of this kind of transformation, 
 * like the derivation of the rotation matrixes is described in literature on strapdown navigation
 * E.g. "Strapdown Inertial Navigation Technology", 2nd Edition by David Titterton and John Weston, section 3.6
 * END Explanation of the angleYaw, anglePitch angleRoll parameters  */
typedef struct {
    float dist2RefPointX;   /**< Distance of acceleration sensor from vehicle reference point (x-coordinate) [m]. */
    float dist2RefPointY;   /**< Distance of acceleration sensor from vehicle reference point (y-coordinate) [m]. */
    float dist2RefPointZ;   /**< Distance of acceleration sensor from vehicle reference point (z-coordinate) [m]. */
    float angleYaw;         /**< Euler angle of first rotation, around yaw axis, to describe acceleration sensor orientation [degree]. For details, see above. */
    float anglePitch;       /**< Euler angle of second rotation, around pitch axis, to describe acceleration sensor orientation [degree]. For details, see above. */
    float angleRoll;        /**< Euler angle of third rotation, around roll axis, to describe acceleration sensor orientation [degree]. For details, see above. */
    float sigmaX;           /**< Standard error estimate of the x-acceleration [m/s^2]. */
    float sigmaY;           /**< Standard error estimate of the y-acceleration [m/s^2]. */
    float sigmaZ;           /**< Standard error estimate of the z-acceleration [m/s^2]. */
    uint32_t typeBits;      /**< Bit mask indicating the type of the used accelerometer. 
                                 [bitwise or'ed @ref EAccelerationTypeBits values]. */    
    uint32_t validityBits;  /**< Bit mask indicating the validity of each corresponding value.
                                 [bitwise or'ed @ref EAccelerationConfigValidityBits values].
                                 Must be checked before usage. */
} TAccelerationConfiguration;

/**
 * TAccelerationData::validityBits provides information about the currently valid signals of the acceleration data.
 * It is a or'ed bitmask of the EAccelerationValidityBits values.
 * Note: 
 * - The general availability of the signals is provided per TAccelerationConfiguration::typeBits. 
 * - If a signal is generally provided but temporarily not available, then the corresponding typeBit will be set but not the validityBit 
 */
typedef enum {
    ACCELERATION_X_VALID                = 0x00000001,    /**< Validity bit for field TAccelerationData::x. */
    ACCELERATION_Y_VALID                = 0x00000002,    /**< Validity bit for field TAccelerationData::y. */
    ACCELERATION_Z_VALID                = 0x00000004,    /**< Validity bit for field TAccelerationData::z. */
    ACCELERATION_TEMPERATURE_VALID      = 0x00000008     /**< Validity bit for field TAccelerationData::temperature. */
} EAccelerationValidityBits;

/**
 * The AccelerationData delivers the sensor values of the accelerometer.
 * The coordinate system is the axis system of the accelerometer sensor, 
 * i.e. the x, y, z values are raw measurements 
 * without any conversion except probably averaging of multiple 
 * sensor readings over the measurement interval.
 *
 * @see TAccelerationConfiguration for an explanation how to convert these values to the vehicle axis system
 *
 * It is possible that not all values are populated, e.g. when only a 1-axis accelerometer is used.
 * You must check the valid bits before usage.
 */
typedef struct {
    uint64_t timestamp;             /**< Timestamp of the acquisition of the accelerometer signal [ms].
                                         All sensor/GNSS timestamps must be based on the same time source. */
    float x;                        /**< The acceleration in direction of the X-axis of the accelerometer sensor [m/s^2].*/
    float y;                        /**< The acceleration in direction of the Y-axis of the accelerometer sensor [m/s^2].*/
    float z;                        /**< The acceleration in direction of the Z-axis of the accelerometer sensor [m/s^2]. */
    float temperature;              /**< Temperature reading of the accelerometer sensor.
                                         If available it can be used for temperature compensation. 
                                         The measurement unit is unspecified. 
                                         Degrees celsius are preferred but any value linearly dependent on the temperature is fine.*/
    uint32_t validityBits;          /**< Bit mask indicating the validity of each corresponding value.
                                         [bitwise or'ed @ref EAccelerationValidityBits values].
                                         Must be checked before usage. */
} TAccelerationData;

/**
 * Callback type for acceleration sensor service.
 * Use this type of callback if you want to register for acceleration data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param accelerationData pointer to an array of TAccelerationData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*AccelerationCallback)(const TAccelerationData accelerationData[], uint16_t numElements);

/**
 * Initialization of the acceleration sensor service.
 * Must be called before using the acceleration sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsAccelerationInit();

/**
 * Destroy the acceleration sensor service.
 * Must be called after using the acceleration sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsAccelerationDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsAccelerationGetMetaData(TSensorMetaData *data);

/**
 * Accessing static configuration information about the acceleration sensor.
 * @param config After calling the method the currently available acceleration configuration data is written into this parameter.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsAccelerationGetAccelerationConfiguration(TAccelerationConfiguration* config);

/**
 * Method to get the acceleration data at a specific point in time.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param accelerationData After calling the method the currently available acceleration data is written into accelerationData.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsAccelerationGetAccelerationData(TAccelerationData* accelerationData);

/**
 * Register acceleration callback.
 * This is the recommended method for continuously accessing the acceleration data.
 * The callback will be invoked when new acceleration data is available.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsAccelerationRegisterCallback(AccelerationCallback callback);

/**
 * Deregister acceleration callback.
 * After calling this method no new acceleration data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsAccelerationDeregisterCallback(AccelerationCallback callback);

/**
 * Method to get the acceleration sensor status at a specific point in time.
 * @param status After calling the method the current acceleration sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsAccelerationGetStatus(TSensorStatus* status);

/**
 * Register acceleration sensor status callback.
 * This is the recommended method for continuously monitoring the acceleration sensor status.
 * The callback will be invoked when new acceleration sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsAccelerationRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister acceleration sensor status callback.
 * After calling this method no new acceleration sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsAccelerationDeregisterStatusCallback(SensorStatusCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_GENIVI_ACCELERATION */
