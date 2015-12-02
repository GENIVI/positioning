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

#ifndef INCLUDE_GENIVI_GYROSCOPE
#define INCLUDE_GENIVI_GYROSCOPE

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * TGyroscopeConfiguration::validityBits provides information about the currently valid signals of the gyroscope configuration data.
 * It is a or'ed bitmask of the EGyroscopeConfigValidityBits values.
 */
typedef enum {
    GYROSCOPE_CONFIG_ANGLEYAW_VALID         = 0x00000001,    /**< Validity bit for field TGyroscopeConfiguration::angleYaw. */
    GYROSCOPE_CONFIG_ANGLEPITCH_VALID       = 0x00000002,    /**< Validity bit for field TGyroscopeConfiguration::anglePitch. */
    GYROSCOPE_CONFIG_ANGLEROLL_VALID        = 0x00000004,    /**< Validity bit for field TGyroscopeConfiguration::angleRoll. */
    GYROSCOPE_CONFIG_MOMENTYAW_VALID        = 0x00000008,    /**< Validity bit for field TGyroscopeConfiguration::momentOfYawInertia. */
    GYROSCOPE_CONFIG_SIGMAGYROSCOPE_VALID   = 0x00000010,    /**< Validity bit for field TGyroscopeConfiguration::sigmaGyroscope. */
    GYROSCOPE_CONFIG_TYPE_VALID             = 0x00000020     /**< Validity bit for field TGyroscopeConfiguration::typeBits. */
} EGyroscopeConfigValidityBits;

/**
 * Gyroscope type
 * TGyroscopeConfiguration::typeBits provides information about the type of the gyroscope and the interpretation of the signals.
 * It is a or'ed bitmask of the EGyroscopeTypeBits values.
 */
typedef enum {
    GYROSCOPE_TEMPERATURE_COMPENSATED       = 0x00000001,   /**< Temperature bias compensation already applied to gyroscope signals. */
    GYROSCOPE_YAWRATE_PROVIDED              = 0x00000002,   /**< A measurement for the z/yaw-axis is provided */
    GYROSCOPE_PITCHRATE_PROVIDED            = 0x00000004,   /**< A measurement for the y/pitch-axis is provided */
    GYROSCOPE_ROLLRATE_PROVIDED             = 0x00000008,   /**< A measurement for the x/roll-axis is provided */
    GYROSCOPE_TEMPERATURE_PROVIDED          = 0x00000010    /**< A measurement for the temperature is provided */
} EGyroscopeTypeBits;

/**
 * Static configuration data for the gyroscope sensor service.
 *
 * BEGIN Explanation of the angleYaw, anglePitch angleRoll parameters
 * The orientation of the gyroscope hardware (Xg, Yg, Zg)
 * with respect to the vehicle axis system (Xv, Yv, Zv)
 * can be described using the angles (angleYaw, anglePitch, angleRoll)
 * following the approach defined in ISO 8855:2011, section 5.2, table 1
 * Apply 3 rotations on the vehicle axis system until it matches the gyroscope axis system
 * The rotation sequence is as follows
 *  - first rotate by angleYaw about the Zv axis
 *  - second rotate by anglePitch about the new (intermediate) Y axis
 *  - third rotate by angleRoll about the new X axis
 * Notes 
 * - the angles are frequently called "Euler angles" and the rotations "Euler rotations"
 * - a different order of the rotations would lead to a different orientation
 * - as the vehicle axis system is right-handed, also the gyroscope axis system must be right-handed
 *
 * The vehicle axis system as defined in ISO 8855:2011(E).
 * In this system, the axes (Xv, Yv, Zv) are oriented as follows
 * - Xv is in the horizontal plane, pointing forwards
 * - Yv is in the horizontal plane, pointing to the left
 * - Zv is perpendicular to the horizontal plane, pointing upwards
 * For an illustration, see https://collab.genivi.org/wiki/display/genivi/LBSSensorServiceRequirementsBorg#LBSSensorServiceRequirementsBorg-ReferenceSystem
 *
 * When the gyroscope axes are not aligned with the vehicle axes, i.e. 
 * if any of the angles (angleYaw, anglePitch, angleRoll) is not zero
 * then the raw measurement values of the gyroscope Z, Y, X axes may have to be be transformed
 * to the vehicle axis system by the client of this interface, depending on the type of application.
 * Raw measurements are provided in @TGyroscopeData instead of already transformed values, because
 * - for gyroscopes with less than 3 axes, the transformation is mathematically not well-defined
 * - some types of calibration operations are better performed on raw data
 *
 * Implementors hint: The mathematics of this kind of transformation, 
 * like the derivation of the rotation matrixes is described in literature on strapdown navigation
 * E.g. "Strapdown Inertial Navigation Technology", 2nd Edition by David Titterton and John Weston, section 3.6
 * END Explanation of the angleYaw, anglePitch angleRoll parameters 
 *
 */
typedef struct {
    float angleYaw;             /**< Euler angle of first rotation, around yaw axis, to describe gyroscope orientation [degree]. For details, see above. */
    float anglePitch;           /**< Euler angle of second rotation, around pitch axis, to describe gyroscope orientation [degree]. For details, see above. */
    float angleRoll;            /**< Euler angle of third rotation, around roll axis, to describe gyroscope orientation [degree]. For details, see above. */
    float momentOfYawInertia;   /**< Moment of yaw inertia. The pitch and roll inertia moments are not provided as they are not relevant for positioning. [kg*m^2] */
    float sigmaGyroscope;       /**< Standard error estimate of the gyroscope for all directions. [degree/s] */
    uint32_t typeBits;          /**< Bit mask indicating the type of the used gyroscope. 
                                     [bitwise or'ed @ref EGyroscopeTypeBits values]. */
    uint32_t validityBits;      /**< Bit mask indicating the validity of each corresponding value.
                                     [bitwise or'ed @ref EGyroscopeConfigValidityBits values].
                                     Must be checked before usage. */
} TGyroscopeConfiguration;

/**
 * TGyroscopeData::validityBits provides information which fields in TGyroscopeData contain valid measurement data. 
 * It is a or'ed bitmask of the EGyroscopeValidityBits values.
 * Note: 
 * - The general availability of the signals is provided per TGyroscopeConfiguration::typeBits. 
 * - If a signal is generally provided but temporarily not available, then the corresponding typeBit will be set but not the validityBit
 */
typedef enum {
    GYROSCOPE_YAWRATE_VALID             = 0x00000001,    /**< Validity bit for field TGyroscopeData::yawRate. */
    GYROSCOPE_PITCHRATE_VALID           = 0x00000002,    /**< Validity bit for field TGyroscopeData::pitchRate. */
    GYROSCOPE_ROLLRATE_VALID            = 0x00000004,    /**< Validity bit for field TGyroscopeData::rollRate. */
    GYROSCOPE_TEMPERATURE_VALID         = 0x00000008     /**< Validity bit for field TGyroscopeData::temperature. */
} EGyroscopeValidityBits;

/**
 * The GyroscopeData delivers the sensor values of the gyroscope.
 * The coordinate system is the axis system of the gyroscope sensor, 
 * i.e. the yawRate, pitchRate, rollRate values are raw measurements 
 * without any conversion except probably averaging of multiple 
 * sensor readings over the measurement interval.
 *
 * @see TGyroscopeConfiguration for an explanation how to convert these values to the vehicle axis system
 *
 * It is possible that not all values are populated, e.g. when only a 1-axis gyroscope is used.
 * You must check the valid bits before usage.
 */
typedef struct {
    uint64_t timestamp;     /**< Timestamp of the acquisition of the gyroscope signal [ms].
                                 All sensor/GNSS timestamps must be based on the same time source. */
    float yawRate;          /**< Current angular rate measurement around the z/yaw-axis of the gyroscope sensor [degree/s].
                                 Value range -100 / +100 degree/s. Frequency of at least 5Hz. Preferrably 50Hz. 
                                 A rotation to the left is indicated by a positive sign. */
    float pitchRate;        /**< Current angular rate measurement around the y/pitch-axis of the gyroscope sensor [degree/s].
                                 Value range -100 / +100 degree/s. Frequency of at least 5Hz. Preferrably 50Hz. 
                                 A rotation front down is indicated by a positive sign. */
    float rollRate;         /**< Current angular rate measurement around the x/roll-axis of the gyroscope sensor [degree/s].
                                 Value range -100 / +100 degree/s. Frequency of at least 5Hz. Preferrably 50Hz. 
                                 A rotation right down is indicated by a positive sign. */
    float temperature;      /**< Temperature reading of the gyroscope sensor.
                                 If available it can be used for temperature compensation. 
                                 The measurement unit is unspecified. 
                                 Degrees celsius are preferred but any value linearly dependent on the temperature is fine.*/
    uint32_t validityBits;  /**< Bit mask indicating the validity of each corresponding value.
                                 [bitwise or'ed @ref EGyroscopeValidityBits values].
                                  Must be checked before usage. */
} TGyroscopeData;

/**
 * Callback type for gyroscope sensor service.
 * Use this type of callback if you want to register for gyroscope data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param gyroData pointer to an array of TGyroscopeData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*GyroscopeCallback)(const TGyroscopeData gyroData[], uint16_t numElements);

/**
 * Initialization of the gyroscope sensor service.
 * Must be called before using the gyroscope sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsGyroscopeInit();

/**
 * Destroy the gyroscope sensor service.
 * Must be called after using the gyroscope sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsGyroscopeDestroy();

/**
 * Provide meta information about sensor service.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsGyroscopeGetMetaData(TSensorMetaData *data);

/**
 * Accessing static configuration information about the gyroscope sensor.
 * @param gyroConfig After calling the method the currently available gyroscope configuration data is written into gyroConfig.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsGyroscopeGetConfiguration(TGyroscopeConfiguration* gyroConfig);

/**
 * Method to get the gyroscope data at a specific point in time.
 * Be careful when using this method to read data often enough as gyro data are rotation rates per second.
 * The recommended usage for the gyroscope data is the callback interface.
 * The get method is provided for consistency reasons of the sensor service API and might be used
 * for determining the rotation rate in certain situations.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param gyroData After calling the method the currently available gyroscope data is written into gyroData.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsGyroscopeGetGyroscopeData(TGyroscopeData* gyroData);

/**
 * Register gyroscope callback.
 * This is the recommended method for continuously accessing the gyroscope data, i.e. rotation rates.
 * The callback will be invoked when new gyroscope data is available.
 * All valid flags are updated. The data is only guaranteed to be updated when the valid flags are true.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsGyroscopeRegisterCallback(GyroscopeCallback callback);

/**
 * Deregister gyroscope callback.
 * After calling this method no new gyroscope data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsGyroscopeDeregisterCallback(GyroscopeCallback callback);

/**
 * Method to get the gyroscope status at a specific point in time.
 * @param status After calling the method the current gyroscope status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsGyroscopeGetStatus(TSensorStatus* status);

/**
 * Register gyroscope status callback.
 * This is the recommended method for continuously monitoring the gyroscope status.
 * The callback will be invoked when new gyroscope status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsGyroscopeRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister gyroscope status callback.
 * After calling this method no new gyroscope status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsGyroscopeDeregisterStatusCallback(SensorStatusCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_GYROSCOPE */
