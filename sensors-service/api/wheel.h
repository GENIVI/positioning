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

#ifndef INCLUDE_GENIVI_WHEEL
#define INCLUDE_GENIVI_WHEEL

#include "sns-meta-data.h"
#include "sns-status.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This header file defines the interface for wheel rotation data.
 * Wheel rotation data may be provided as wheel ticks, as wheel speed 
 * or as angular wheel speed.
 *
 * It is vehicle specific which kind of wheel rotation data is available
 * and for which wheels the data is provided.
 * The vehicle specific configuration can be queried using the 
 * snsWheelGetConfiguration() function.
 *
 * Note: The driving direction (forward/backward) shall always be coded as
 * sign of the wheel rotation data.
 * This will reduce synchronization effort with separate reverse gear info 
 * by the client application which is in particular useful with buffered data.
 * How the driving direction can be derived is vehicle specific.
 */

/**
 * Defines the wheel or axle for which the wheel rotation information is provided.
 * Usually wheel rotation data are either provided for the non-driven axle or for each wheel individually.
 */
typedef enum {
    WHEEL_INVALID       = 0,       /**< Wheel data is invalid / the field is unused. */
    WHEEL_UNKNOWN       = 1,       /**< No information available where the wheel rotation data is from. */
    WHEEL_AXLE_NONDRIVEN = 2,      /**< Wheel rotation data is an average from the non driven axle. 
                                        Can be front or rear depending on the type of drivetrain. */
    WHEEL_AXLE_FRONT    = 3,       /**< Wheel rotation data is an average from the front axle. */
    WHEEL_AXLE_REAR     = 4,       /**< Wheel rotation data is an average from the rear axle. */
    WHEEL_LEFT_FRONT    = 5,       /**< Wheel rotation data is from the left front wheel. */
    WHEEL_RIGHT_FRONT   = 6,       /**< Wheel rotation data is from the right front wheel. */
    WHEEL_LEFT_REAR     = 7,       /**< Wheel rotation data is from the left rear wheel. */
    WHEEL_RIGHT_REAR    = 8        /**< Wheel rotation data is from the right rear wheel. */
} EWheelId;

/**
 * Additional status information for wheel data, in particular when provided as wheel ticks.
 * This may help the client application to estimate the reliability of the wheel data.
 * TWheelData::statusBits is a or'ed bitmask of the EWheelStatusBits values.
 * 
 * Background information
 * ----------------------
 * Wheel ticks are typically provided by the ABS/ESC ECU as rolling counters
 * on the vehicle bus.
 * To calculate the wheel ticks per time interval as provided by this API in the 
 * TWheelData structure, the difference between the two _valid_ rolling 
 * counter values at end and start of the time interval has to be calculated 
 * (taking into account potential rollover).
 * If any of the rolling counter values is invalid or if there has been a reset 
 * of the rolling counter in the time interval, then no valid difference can be 
 * calculated. Therefore an appropriate error/exception handling has to be 
 * implemented in the library translating the rolling counters from the vehicle bus
 * to the wheel ticks per time interval provided by this API.
 *
 * Besides to the validity indication provided with each wheel rotation update,
 * the client (typically the EnhancedPositionService) using the wheel rotation API 
 * may be interested to know whether wheel rotation information may have been lost. 
 * In such a case it could adapt its error estimation related to 
 * wheel ticks or even make an internal reset of its corresponding states.
 * The status bits in enum EWheelStatusBits are defined to provide such information.
 *
 * Further Background
 * ------------------
 * This section gives an additional overview about the possible signal path 
 * of wheel tick data and the resulting possible exceptional situations:
 * There may be a gateway between the ABS/ESC ECU and the IVI system to separate
 * vehicle networks. This gateway may reduce the update rate of the CAN messages, 
 * e.g. from 20ms cycle time sent by the ABS ECU to 100ms provided for the IVI system.
 * When the update rate is reduced, the rolling counters may have to be resampled e.g.
 * from 8 bit to 13 bit to avoid rollover within the update period.
 * The rolling counters typically start from 0 when either the ABS ECU or the gateway is 
 * started/restarted, e.g. at an ignition cycle.
 * The rolling counters are often accompanied with additional status information to indicate 
 * validity, reset conditions or to allow to detect loss of wheel tick data during transmission
 * on vehicle bus (such as sequence numbers).
 * This status information has to be evaluated to determine whether the difference calculation 
 * between subsequent rolling counters yields a valid result or not.
 * The kind of status information provided alongside with the wheel ticks is very OEM specific 
 * - sometimes even additional context information such as ignition status has to considered.
 * Nearly all above mentioned parameters are OEM or vehicle specific: update rate, 
 * size of rolling counters, status indications, lifecycle of ECU, gateway, vehicle bus, ...
 * The status bits in enum EWheelStatusBits attempt to provide an appropriate abstraction for the
 * relevant vehicle specific status information.
 * 
 */
typedef enum {
    WHEEL_STATUS_GAP             = 0x00000001,  /**< There has been a gap in data collection, 
                                                      i.e. an unknown number of wheel revolutions has been lost. 
                                                      The reason for such a gap can be for example
                                                      - wheel tick data on the vehicle bus explicitly tagged as invalid 
                                                      - interrupted reception of vehicle bus messages */
    WHEEL_STATUS_INIT            = 0x00000002   /**< This is the first wheel data of a bus or ignition lifecycle, 
                                                      i.e. the wheel tick difference calculation may be less reliable. */
} EWheelStatusBits;

/**
 * TWheelData::validityBits provides information which fields in TWheelData contain valid measurement data. 
 * It is a or'ed bitmask of the EWheelValidityBits values.
 * Note: The assignment of the fields  to the wheels of the vehicle is provided by the function snsWheelGetConfiguration().
 */
typedef enum {
    WHEEL1_VALID                = 0x00000001,    /**< Validity bit for field TWheelData::wheel1. */
    WHEEL2_VALID                = 0x00000002,    /**< Validity bit for field TWheelData::wheel2. */
    WHEEL3_VALID                = 0x00000004,    /**< Validity bit for field TWheelData::wheel3. */
    WHEEL4_VALID                = 0x00000008     /**< Validity bit for field TWheelData::wheel4. */
} EWheelValidityBits;

/**
 * Defines the measurement unit in which the wheel rotation data is provided.
 *
 * The wheel rotation direction is given by the sign of the wheel rotation value:
 * Positive values indicate forward driving.
 * Negative values indicate backward driving.

 */
typedef enum {
    WHEEL_UNIT_TICKS            = 1,    /**< Wheel rotation data is provided as number of wheel ticks accumulated within measurement interval. 
                                             Note 1: Therefore, if the wheel ticks on the vehicle bus are represented as rolling counters, 
                                             this is the difference between two subsequent rolling counter values 
                                             taking the vehicle specific roll-over boundary into account. 
                                             Note 2: It is safe to store integer values such as for wheel ticks 
                                             without precision loss in float variables for values up to 2^23. */
    WHEEL_UNIT_SPEED            = 2,    /**< Wheel rotation data is provided as speed in [m/s]. */
    WHEEL_UNIT_ANGULAR_SPEED    = 3,    /**< Wheel rotation data is provided as angular speed in [1/s] rotation per seconds. */
} EWheelUnit;


/**
 * TWheelConfiguration::validityBits provides information about the currently valid signals of the wheel configuration data.
 * It is a or'ed bitmask of the EWheelConfigValidityBits values.
 */
typedef enum {
    WHEEL_CONFIG_TICKS_PER_REV      = 0x00000001,    /**< Validity bit for field TWheelConfiguration::wheelticksPerRevolution. */
    WHEEL_CONFIG_TIRE_CIRC          = 0x00000002     /**< Validity bit for field TWheelConfiguration::tireRollingCircumference. */

} EWheelConfigValidityBits;

/**
 * Static configuration data for the wheel sensor service.
 */
typedef struct {
    EWheelUnit wheelUnit;       /**< Measurement unit in which the wheel rotation data is provided. Valid for all wheels. */
    EWheelId wheel1Id;          /**< Identifies the wheel of the vehicle assigned to @ref TWheelData::wheel1 */      
    EWheelId wheel2Id;          /**< Identifies the wheel of the vehicle assigned to @ref TWheelData::wheel2 */
    EWheelId wheel3Id;          /**< Identifies the wheel of the vehicle assigned to @ref TWheelData::wheel3 */
    EWheelId wheel4Id;          /**< Identifies the wheel of the vehicle assigned to @ref TWheelData::wheel4 */
    uint16_t wheelticksPerRevolution;   /**< Number of ticks for a single revolution of one wheel */
    float tireRollingCircumference;     /**< Distance travelled on the ground per a single revolution of one wheel. Unit: [m]. */
    uint32_t validityBits;      /**< Bit mask indicating the validity of each corresponding value.
                                     [bitwise or'ed @ref EWheelConfigValidityBits values].
                                     Must be checked before usage. 
                                     Note: wheelUnit and wheel1 .. wheel4 are always valid. Therefore no dedicated validityBits are requried.*/
} TWheelConfiguration;


/**
 * Wheel rotation data information for multiple wheels.
 * Container with timestamp as used by callback and get function.
 *
 * Wheel rotation data is provided for up to 4 wheels.
 * The assignment of the fields wheel1, wheel2, wheel3, wheel4 
 * to the wheels of the vehicle and the measurement unit
 * is provided as static configuration by the function snsWheelGetConfiguration().
 *
 */
typedef struct {
    uint64_t timestamp;     /**< Timestamp of the acquisition of this wheel tick signal [ms].
                                 All sensor/GNSS timestamps must be based on the same time source. */
                                                         
    float wheel1;           /**< Wheel rotation data for the wheel identified by TWheelConfiguration::wheel1 in measurement unit identified by TWheelConfiguration::wheelUnit. */
    float wheel2;           /**< Wheel rotation data for the wheel identified by TWheelConfiguration::wheel2 in measurement unit identified by TWheelConfiguration::wheelUnit. */
    float wheel3;           /**< Wheel rotation data for the wheel identified by TWheelConfiguration::wheel3 in measurement unit identified by TWheelConfiguration::wheelUnit. */
    float wheel4;           /**< Wheel rotation data for the wheel identified by TWheelConfiguration::wheel4 in measurement unit identified by TWheelConfiguration::wheelUnit. */
    
    uint32_t statusBits;    /**< Bit mask providing additional status information.
                                 [bitwise or'ed @ref EWheelStatusBits values]. */

    uint32_t validityBits;  /**< Bit mask indicating the validity of each corresponding value.
                                 [bitwise or'ed @ref EWheelValidityBits values].
                                  Must be checked before usage. */                                                         
} TWheelData;

/**
 * Callback type for wheel sensor service.
 * Use this type of callback if you want to register for wheel rotation data.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * All wheel data belonging to the same timestamp will be provided in the same structure,
 * i.e. there will be never two callbacks or array elements with the same timestamp.
 * @param wheelData pointer to an array of TWheelData with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*WheelCallback)(const TWheelData wheelData[], uint16_t numElements);

/**
 * Callback type for wheel sensor status.
 * Use this type of callback if you want to register for wheel sensor status updates data.
 * @param status the wheel sensor status
 */
typedef void (*WheelStatusCallback)(const TSensorStatus *status);

/**
 * Initialization of the wheel sensor service.
 * Must be called before using the wheel sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsWheelInit();

/**
 * Destroy the wheel sensor service.
 * Must be called after using the wheel sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsWheelDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsWheelGetMetaData(TSensorMetaData *data);

 /**
 * Accessing static configuration information about the wheel sensor.
 * @param config After calling the method the currently available acceleration wheel data is written into this parameter.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsWheelGetConfiguration(TWheelConfiguration* config);

/**
 * Method to get the wheel rotation data at a specific point in time.
 * @param wheelData After calling the method the currently available wheel rotation data is written into the array.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsWheelGetWheelData(TWheelData *wheelData);

/**
 * Register callback for multiple wheel rotation data information.
 * This is the recommended method for continuously accessing the wheel data.
 * The callback will be invoked when new rotation data data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsWheelRegisterCallback(WheelCallback callback);

/**
 * Deregister multiple wheel rotation data callback.
 * After calling this method no new rotation data data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsWheelDeregisterCallback(WheelCallback callback);

/**
 * Method to get the wheel sensor status at a specific point in time.
 * @param status After calling the method the current wheel sensor status is written into status
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsWheelGetStatus(TSensorStatus* status);

/**
 * Register wheel sensor status callback.
 * This is the recommended method for continuously monitoring the wheel sensor status.
 * The callback will be invoked when new wheel sensor status data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsWheelRegisterStatusCallback(SensorStatusCallback callback);

/**
 * Deregister wheel sensor status callback.
 * After calling this method no new wheel sensor status updates will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsWheelDeregisterStatusCallback(SensorStatusCallback callback);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_WHEEL */
