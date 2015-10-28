/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \brief Logging service for data from positioning and similar sources
*        Log data can be forwarded to different sinks in parallel,
*        e.g. to DLT and to a file or a socket
*        Additionally a custom callback can be registered as log sink
*        Log data can be provided from different threads running parallel
*        Logging is done synchronously, so it may block temporarily 
*        depending on the type of sinks which are active
*        However using the callback sink, an asynchronous logging
*        can be implemented without impact to the threads providing log data
*        Log data must be provided as ASCII strings 
*        The clients are responsible for string formatting 
*        according the positioning log format specification
*        
*
* \author Helmut Schmidt <https://github.com/huirad>
*
* \copyright Copyright (C) 2015, Helmut Schmidt
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#ifndef INCLUDE_GENIVI_POS_LOG
#define INCLUDE_GENIVI_POS_LOG

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * EPoslogReleaseLevel indicates the release level of this API
 * This enum follows the release level convention used by python
 * @ref https://docs.python.org/3/c-api/apiabiversion.html
 */
typedef enum {
    POSLOG_REL_ALPHA        = 0xA0,    /**< API is in alpha state, i.e. work in progress. */
    POSLOG_REL_BETA         = 0xB0,    /**< API is in beta state, i.e. close to be finished. */
    POSLOG_REL_CANDIDATE    = 0xC0,    /**< API is in release candidate state. */
    POSLOG_REL_FINAL        = 0xF0,    /**< API is in final state, i.e. officially approved. */
} EPoslogReleaseLevel;  

    
// API Version
#define GENIVI_POSLOG_MAJOR 1
#define GENIVI_POSLOG_MINOR 0
#define GENIVI_POSLOG_MICRO 0
#define GENIVI_POSLOG_LEVEL POSLOG_REL_ALPHA


/**
 * TPoslogSinks is used to indicate which sinks are active. 
 * It is a or'ed bitmask of the EPoslogSinks values.
 * Multiple sinks can be active in parallel
 */
typedef uint32_t TPoslogSinks;
typedef enum {
    POSLOG_SINK_DLT    = 0x00000001,    /**< Bit is set to indicate logging to GENIVI DLT. */
    POSLOG_SINK_SYSLOG = 0x00000002,    /**< Bit is set to indicate logging to Linux syslog. 
                                             Note: on most systemd based systems, syslog is not running.
                                             But on systems without systemd, this might be still useful. */
    POSLOG_SINK_FD     = 0x00000004,    /**< Bit is set to indicate logging to a file descriptor.
                                             The file descriptor can e.g. refer to a file, pipe, socket, ... . 
                                             The file descriptor must be registered using poslogSetFD(). */
    POSLOG_SINK_CB     = 0x00000008,    /**< Bit is set to indicate logging to custom callback function.
                                             The callback function must be registered using poslogSetCB(). */
} EPoslogSinks;  
  
/**
 * TPoslogSeq is used to allow uninterrupted logging of a sequence of strings
 * It is a or'ed bitmask of the EPoslogSeq values.
 */
typedef uint16_t TPoslogSeq;
typedef enum {
    POSLOG_SEQ_CONT         = 0x0000,    /**< String to be logged is in the middle of a sequence. */
    POSLOG_SEQ_START        = 0x0001,    /**< String to be logged is at the start of a sequence. */
    POSLOG_SEQ_STOP         = 0x0002,    /**< String to be logged is at the end of a sequence. */
    POSLOG_SEQ_SINGLE       = 0x0003,    /**< String to be logged is not part of a sequence. */
} EPoslogSeq;  


/**
 * Callback type for a custom log sink provided by the application.
 * Use this type of callback if you want to send log data
 * to sinks which are not directly supported by the positioning logger.
 * @param logstring String to be added to the log.
 */
typedef void (*PoslogCallback)(const char* string);

  
/**
 * Initialization of the positioning logging service.
 * Must be called before using the positioning logging service to set up the service.
 * If the DLT sink shall be used, the caller is responsible to call DLT_REGISTER_APP()
 * before poslogInit().
 * If the syslog sink shall be used, the caller is responsible to call openlog()
 * before poslogInit().
 * @return True if initialization has been successfull.
 */
bool poslogInit();

/**
 * Destroy the positioning logging service.
 * Must be called after using the positioning logging service to shut down the service.
 * @return True if shutdown has been successfull.
 */
bool poslogDestroy();

/**
 * Positioning logging services version information.
 * @param major Major version number. Changes in this number are used for incompatible API change.
 * @param minor Minor version number. Changes in this number are used for compatible API change.
 * @param micro Micro version number. Changes in this number are used for minor changes.
 * @param level Release level of this API
 */
void poslogGetVersion(int *major, int *minor, int *micro, EPoslogReleaseLevel *level);

/**
 * Control which sinks are active
 * @param sinks An or'ed bitmask of the EPoslogSinks values. 
 * A sink will be activated when the corresponding bit is set.
 * For file descriptor or callback sinks it is advised to set the 
 * corresponding file descriptor or callback before, otherwise logs may be lost.
 * This function is thread safe and can be called during logging
 * @return void
 */
void poslogSetActiveSinks(TPoslogSinks sinks);

/**
 * Determine which sinks are active
 * @return sinks An or'ed bitmask of the EPoslogSinks values. 
 * A sink will is activate when the corresponding bit is set.
 */
TPoslogSinks poslogGetActiveSinks();

/**
 * Set the file descriptor for the file descriptor sink
 * Only one file descriptor can be used at a time
 * The file descriptor must refer to an *open* file/socket/pipe... 
 * This function is thread safe and can be called during logging
 * Calling this function will not automatically activate the fd sink,
 * use @ref poslogSetActiveSinks() for this.
 * @param fd The file descriptor to be used as logging sink
 * @return The previously set file descriptor, -1 if no file descriptor was set
 */
int poslogSetFD(int fd);

/**
 * Set the file descriptor for the file descriptor sink
 * Only one file descriptor can be used at a time
 * This function is thread safe and can be called during logging.
 * Calling this function will not automatically activate the fd sink,
 * use @ref poslogSetActiveSinks() for this. 
 * @param cb The callback to be used as logging sink
 * @return The previously set callback, NULL if no callback was set
 */
PoslogCallback poslogSetCB(PoslogCallback cb);

/**
 * Add a string to the log.
 * The string will be sent to all currently active sinks.
 * This function is thread safe. Log strings can be provided from concurrent threads.
 * Note: The log string shall *not* contain a trailing newline.
 * @note Depending on the type of sink there might be length limitations.
 * @param logstring String to be added to the log.
 * @param seq Bitmask of the EPoslogSeq values indicating where in a sequence the string is.
 */
void poslogAddString(const char* logstring, TPoslogSeq seq = POSLOG_SEQ_SINGLE);

#ifdef __cplusplus
}
#endif

#endif
