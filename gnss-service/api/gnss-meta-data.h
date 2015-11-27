/**************************************************************************
 * @licence app begin@
 *
 * SPDX-License-Identifier: MPL-2.0
 *
 * \ingroup GNSSService
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


#ifndef INCLUDE_GENIVI_GNSS_METADATA
#define INCLUDE_GENIVI_GNSS_METADATA

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The GNSS category introduces the concept that sensor information can also be derived information
 * computed by combining several signals.
 */
typedef enum {
    GNSS_CATEGORY_UNKNOWN,       /**< Unknown category. Should not be used. */
    GNSS_CATEGORY_LOGICAL,       /**< A logical GNSS service can combine the signal of a GNSS receiver with additional sources. */
    GNSS_CATEGORY_PHYSICAL       /**< A physical GNSS service, i.e. a stand-alone GNSS receiver. */
} EGnssCategory;

/**
 * TGnssMetaData:typeBits provides information about the sources used for the GNSS calculation
 * It is a or'ed bitmask of the EGnssTypeBits values. 
 */
typedef enum {
    GNSS_TYPE_GNSS      = 0x00000001,  /**< GNSS receiver. Should always be set. */
    GNSS_TYPE_ASSISTED  = 0x00000002,  /**< GNSS receiver with support for Assisted GNSS. E.g. ephemeris or clock data can be provided over network for faster TTFF */
    GNSS_TYPE_SBAS      = 0x00000004,  /**< GNSS receiver with support for SBAS (satellite based augmentation system), such as WAAS, EGNOS, ... */
    GNSS_TYPE_DGPS      = 0x00000008,  /**< GNSS receiver with support for differential GPS */
    GNSS_TYPE_DR        = 0x00000010,  /**< GNSS receiver with built in dead reckoning sensor fusion */
} EGnssTypeBits;


/**
 * The software platform provides the following information about the GNSS output signals.
 * GNSS clients need the meta data information in order to correctly handle data provided by GNSS service and
 * to adapt to the variation in the signal data delivery.
 */
typedef struct {
    uint32_t version;               /**< Version of the GNSS service. */
    EGnssCategory category;         /**< GNSS Category (Physical/Logical). */
    uint32_t typeBits;              /**< GNSS Type: combination of bits defined in @ref EGnssTypeBits. */
    uint32_t cycleTime;             /**< GNSS cycle time (update interval) in ms. 0 for irregular updates */
    uint16_t numChannels;           /**< Number of GNSS receiver channels for satellite signal reception. */
} TGnssMetaData;

/**
 * Provide meta information about GNSS service.
 * The meta data of a service provides information about it's name, version, type, subtype, sampling frequency etc.
 * @param data Meta data content about the sensor service.
 * @return True if meta data is available.
 */
bool gnssGetMetaData(TGnssMetaData *data);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_GENIVI_GNSS_METADATA */
