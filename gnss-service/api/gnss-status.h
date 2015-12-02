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

#ifndef INCLUDE_GENIVI_GNSS_STATUS
#define INCLUDE_GENIVI_GNSS_STATUS

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Enumeration to describe the status of the GNSS receiver
 */
typedef enum {
    GNSS_STATUS_NOTAVAILABLE  = 0,    /**< GNSS is not available at all, based on configuration data. */    
    GNSS_STATUS_INITIALIZING  = 1,    /**< Initial status when the connection to the GNSS is set up for the first time. */
    GNSS_STATUS_AVAILABLE     = 2,    /**< GNSS is available and running as expected. */
    GNSS_STATUS_RESTARTING    = 3,    /**< GNSS is restarted, i.e. due to communication loss. */
    GNSS_STATUS_FAILURE       = 4,    /**< GNSS is not operating properly. Restarting did not help. */
    GNSS_STATUS_OUTOFSERVICE  = 5     /**< GNSS is temporarily not available, due to some known external condition, e.g. firmware update or switch off for antenna supervision. */
} EGNSSStatus;

/**
 * Enumeration to describe the status of the GNSS antenna
 */
typedef enum {
    GNSS_ANT_STATUS_NORMAL          = 0,    /**< GNSS antenna is working in normal operation. */
    GNSS_ANT_STATUS_OVERCURRENT     = 1,    /**< GNSS antenna is working but the antenna current is higher than expected. */
    GNSS_ANT_STATUS_OPEN            = 2,    /**< GNSS antenna is not working because not connected (antenna current too low). */
    GNSS_ANT_STATUS_SHORT_GND       = 3,    /**< GNSS antenna is not working due to short-circuit to ground. */
    GNSS_ANT_STATUS_SHORT_BATT      = 4,    /**< GNSS antenna is not working due to short-circuit to battery. */
    GNSS_ANT_STATUS_OUTOFSERVICE    = 5     /**< GNSS antenna is temporarily not available, due to some known external condition. */
} EGNSSAntennaStatus;


/**
 * TGNSSStatus::validityBits provides information about the currently valid signals of the TGNSSStatus struct.
 * It is a or'ed bitmask of the EGNSSStatusValidityBits values.
 */
typedef enum {
    GNSS_STATUS_STATUS_VALID         = 0x00000001,   /**< Validity bit for field TGNSSStatus::status. */
    GNSS_STATUS_ANT_STATUS_VALID     = 0x00000002    /**< Validity bit for field TGNSSStatus::antStatus. */
} EGNSSStatusValidityBits;

/**
 * Container for GNSS status information
 */
typedef struct {
    uint64_t timestamp;             /**< Timestamp of the GNSS status transition [ms].
                                         All sensor/GNSS timestamps must be based on the same time source. */
    EGNSSStatus status;             /**< Status of the GNSS receiver */
    EGNSSAntennaStatus antStatus;   /**< Status of the GNSS antenna */
    uint32_t validityBits;          /**< Bit mask indicating the validity of each corresponding value.
                                        [bitwise or'ed @ref EGNSSStatusValidityBits values].
                                        Must be checked before usage. */
} TGNSSStatus;    

/**
* Callback type for gnss status.
* Use this type of callback if you want to register for gnss status updates data.
* @param status the gnss status
*/
typedef void (*GNSSStatusCallback)(const TGNSSStatus *status);
/**
* Method to get the gyroscope status at a specific point in time.
* @param status After calling the method the current gyroscope status is written into status
* @return Is true if data can be provided and false otherwise, e.g. missing initialization
*/
bool gnssGetStatus(TGNSSStatus* status);
/**
* Register gnss status callback.
* This is the recommended method for continuously monitoring the gyroscope status.
* The callback will be invoked when new gyroscope status data is available.
* @param callback The callback which should be registered.
* @return True if callback has been registered successfully.
*/
bool gnssRegisterStatusCallback(GNSSStatusCallback callback);
/**
* Deregister gnss status callback.
* After calling this method no new gnss status updates will be delivered to the client.
* @param callback The callback which should be deregistered.
* @return True if callback has been deregistered successfully.
*/
bool gnssDeregisterStatusCallback(GNSSStatusCallback callback);


#ifdef __cplusplus
}
#endif

#endif //#ifndef INCLUDE_GENIVI_GNSS_STATUS
