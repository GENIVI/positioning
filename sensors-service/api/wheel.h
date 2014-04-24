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

#ifndef INCLUDE_GENIVI_WHEELTICK
#define INCLUDE_GENIVI_WHEELTICK

#include "sns-meta-data.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Defines the type for which the wheel tick information is provided.
 * Usually wheel ticks are either provided for the non-driven axle or for each wheel individually.
 */
typedef enum {
    WHEEL_INVALID       = 0,       /**< Wheel tick data is invalid / the field is unused. */
    WHEEL_UNKNOWN       = 1,       /**< No information available where the wheel tick data is from. */
    WHEEL_AXLE_NONDRIVEN = 2,      /**< Wheel tick data is an average from the non driven axle. Can be front or rear depending on the type of drivetrain. */
    WHEEL_AXLE_FRONT    = 3,       /**< Wheel tick data is an average from the front axle. */
    WHEEL_AXLE_REAR     = 4,       /**< Wheel tick data is an average from the rear axle. */
    WHEEL_LEFT_FRONT    = 5,       /**< Wheel tick data is from the left front wheel. */
    WHEEL_RIGHT_FRONT   = 6,       /**< Wheel tick data is from the right front wheel. */
    WHEEL_LEFT_REAR     = 7,       /**< Wheel tick data is from the left rear wheel. */
    WHEEL_RIGHT_REAR    = 8        /**< Wheel tick data is from the right rear wheel. */
} EWheelId;

/**
 * Maximum number of wheel elements per structure.
 * A fix value is used because a flat data structure has advantages like simple copying, indexed access.
 */
#define WHEEL_NUM_ELEMENTS 4

/**
 * A single wheel tick information.
 * No valid flags are provided for the wheel ticks as this information is transferred as an array.
 * If data is not valid the data is simply omitted from transfer.
 */
typedef struct {
    EWheelId wheeltickIdentifier;        /**< Type for which this tick information is provided. */
    uint32_t wheeltickCounter;           /**< The actual wheel tick counter which provides the ticks for the type
                                             specified in wheeltickIdentifier. 
                                             This is a running counter which overflows only at a vehicle specific threshold. 
                                             This must be tracked by the client.
                                             The threshold value can be determined by calling snsWheeltickGetWheelticksCountMax() */
} TWheeltickDetail;

/**
 * Wheel tick information for multiple wheels.
 * Container with timestamp as used by callback and get function.
 */
typedef struct {
    uint64_t timestamp;                             /**< Timestamp of the acquisition of this wheel tick signal [ms].
                                                         All sensor/GNSS timestamps must be based on the same time source. */
    TWheeltickDetail elements[WHEEL_NUM_ELEMENTS];   /**< The array of wheeltick elements. */
} TWheelticks;

/**
 * Callback type for wheel tick sensor service.
 * Use this type of callback if you want to register for wheel tick data.
 * Wheel tick data can be provided for multiple WheelIds. E.g. data can be provided for all 4 wheels.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param ticks pointer to an array of TWheelticks with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*WheeltickCallback)(const TWheelticks ticks[], uint16_t numElements);

/**
 * Initialization of the wheel tick sensor service.
 * Must be called before using the wheel tick sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsWheeltickInit();

/**
 * Destroy the wheel tick sensor service.
 * Must be called after using the wheel tick sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsWheeltickDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsWheelTickGetMetaData(TSensorMetaData *data);

/**
 * Wheel ticks per revolution
 * Static configuration value that specifies how many wheel ticks there are per a single revolution of one wheel.
 * @param ticks The number of ticks for a single revolution of one wheel
 * @return True if configuration data is available.
 */
bool snsWheeltickGetWheelticksPerRevolution(uint16_t* ticks);

/**
 * Maximum value of vehicle specific wheel tick rolling counters
 * Static configuration value that specifies _after_ which value the wheel tick counter will roll over.
 * Some examples
 *   if full 15bit are used for the vehicle specifc rolling wheel counters, the maximum value would be 0x7FFF = 32767
 *   if 15bit are used but the last value 0x7FFF is reserved as SNA then the maximum value would be 0x7FFE = 32766
 * @param max_count The maximum value of wheel tick rolling counters
 * @return True if configuration data is available. If false no value could be provided and an overflow only at the uint32 size shall be assumed.
 */
bool snsWheeltickGetWheelticksCountMax(uint32_t* max_count);

/**
 * Method to get the wheel tick data at a specific point in time.
 * @param ticks After calling the method the currently available wheel tick data is written into the array.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsWheeltickGetWheelticks(TWheelticks *ticks);

/**
 * Register callback for multiple wheel tick information.
 * This is the recommended method for continuously accessing the wheel tick data.
 * The callback will be invoked when new wheel tick data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsWheeltickRegisterCallback(WheeltickCallback callback);

/**
 * Deregister multiple wheel tick callback.
 * After calling this method no new wheel tick data will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsWheeltickDeregisterCallback(WheeltickCallback callback);

/**
 * A single angular wheel speed information.
 * No valid flags are provided for the angular wheel speeds as this information is transferred as an array.
 * If data is not valid the data is simply omitted from transfer.
 */
typedef struct {
    EWheelId id;
    float rotations; /**< unit is [1/s] rotation per seconds */
} TWheelspeedAngularDetail;

/**
 * Angular wheel speed information for multiple wheels.
 * Container with timestamp as used by callback and get function.
 */
typedef struct {
    uint64_t timestamp;                             /**< Timestamp of the acquisition of this angular wheel speed signal [ms].
                                                         All sensor/GNSS timestamps must be based on the same time source. */
    TWheelspeedAngularDetail elements[WHEEL_NUM_ELEMENTS];   /**< The array of angular wheel speed elements. */
} TWheelspeedAngular;

/**
 * Callback type for angular wheel speed sensor service.
 * Use this type of callback if you want to register for angular wheel speed data.
 * Wheel tick data can be provided for multiple WheelIds. E.g. data can be provided for all 4 wheels.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param ticks pointer to an array of TWheelspeedAngular with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*WheelspeedAngularCallback)(const TWheelspeedAngular rotations[], uint16_t numElements);

/**
 * Initialization of the angular wheel speed sensor service.
 * Must be called before using the angular wheel speed sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsWheelspeedAngularInit();

/**
 * Destroy the angular wheel speed sensor service.
 * Must be called after using the angular wheel speed sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsWheelspeedAngularDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsWheelspeedAngularGetMetaData(TSensorMetaData *data);

/**
 * Method to get the angular wheel speed data at a specific point in time.
 * @param wsa After calling the method the currently available angular wheel speed data is written into the array.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsWheelspeedAngularGet(TWheelspeedAngular* wsa);

/**
 * Register callback for multiple angular wheel speed information.
 * This is the recommended method for continuously accessing the angular wheel speed data.
 * The callback will be invoked when new angular wheel speed data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsWheelspeedAngularRegisterCallback(WheelspeedAngularCallback callback);

/**
 * Deregister multiple angular wheel speed callback.
 * After calling this method no new angular wheel speed will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsWheelspeedAngularDeregisterCallback(WheelspeedAngularCallback callback);

/**
 * A single wheel speed information.
 * No valid flags are provided for the wheel speeds as this information is transferred as an array.
 * If data is not valid the data is simply omitted from transfer.
 */
typedef struct {
    EWheelId id;
    float speedOfWheel; /**< [m/s] */
} TWheelspeedDetail;
/**
 * Wheel speed information for multiple wheels.
 * Container with timestamp as used by callback and get function.
 */
typedef struct {
    uint64_t timestamp;                             /**< Timestamp of the acquisition of this wheel speed signal [ms].
                                                         All sensor/GNSS timestamps must be based on the same time source. */
    TWheelspeedDetail elements[WHEEL_NUM_ELEMENTS];   /**< The array of  wheel speed elements. */
} TWheelspeed;

/**
 * Callback type for wheel speed sensor service.
 * Use this type of callback if you want to register for wheel speed data.
 * Wheel tick data can be provided for multiple WheelIds. E.g. data can be provided for all 4 wheels.
 * This callback may return buffered data (numElements >1) for different reasons
 *   for (large) portions of data buffered at startup
 *   for data buffered during regular operation e.g. for performance optimization (reduction of callback invocation frequency)
 * If the array contains (numElements >1), the elements will be ordered with rising timestamps
 * @param wheelspeed pointer to an array of TWheelspeed with size numElements
 * @param numElements: allowed range: >=1. If numElements >1, buffered data are provided. 
 */
typedef void (*WheelspeedCallback)(const TWheelspeed wheelspeed[], uint16_t numElements);

/**
 * Initialization of the wheel speed sensor service.
 * Must be called before using the wheel tick sensor service to set up the service.
 * @return True if initialization has been successfull.
 */
bool snsWheelspeedInit();

/**
 * Destroy the wheel tick sensor service.
 * Must be called after using the wheel speed sensor service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool snsWheelspeedDestroy();

/**
 * Provide meta information about sensor service.
 * The meta data of a sensor service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool snsWheelspeedGetMetaData(TSensorMetaData *data);

/**
 * Method to get the wheel speed data at a specific point in time.
 * @param wheelspeed After calling the method the currently available wheel speed data is written into the array.
 * @return Is true if data can be provided and false otherwise, e.g. missing initialization
 */
bool snsWheelspeedGet(TWheelspeed* wheelspeed);

/**
 * Register callback for multiple wheel speed information.
 * This is the recommended method for continuously accessing the wheel speed data.
 * The callback will be invoked when new angular wheel speed data is available.
 * @param callback The callback which should be registered.
 * @return True if callback has been registered successfully.
 */
bool snsWheelspeedRegisterCallback(WheelspeedCallback callback);

/**
 * Deregister multiple wheel speed callback.
 * After calling this method no new wheel speed will be delivered to the client.
 * @param callback The callback which should be deregistered.
 * @return True if callback has been deregistered successfully.
 */
bool snsWheelspeedDeregisterCallback(WheelspeedCallback callback);

/**
 * Tire rolling circumference as provided in the official documents. This is a static configuration. Unit: [m].
 * Static configuration value that specifies the distance travelled on the ground per a single revolution of one wheel.
 * This may be useful for calculations based on wheel ticks or angular wheel speeds
 * @param circumference The tire rolling circumference in m is written to this parameter as a result.
 * @return True if configuration data is available. If false no value could be provided.
 */
bool snsWheelGetTireRollingCircumference(float* circumference);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_WHEELTICK */
