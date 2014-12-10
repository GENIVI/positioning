/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup GNSSService
*
* \copyright  Copyright (C) BMW Car IT GmbH 2011
*             Copyright (C) 2013, XS Embedded GmbH
*
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/

#ifndef INCLUDE_LOG
#define INCLUDE_LOG

// turn-on via cmake define:
// $ cmake -DWITH_DLT=1 -DDEBUG_ENABLED=1 ..

#if (!DLT_ENABLED)
/*****************************************************************************/
// use printf
#include <stdio.h>

// some type-name used instead of DLT context
typedef const char* NoDltContext;

#define DLT_DECLARE_CONTEXT(CONTEXT) \
    NoDltContext CONTEXT;

#define DLT_IMPORT_CONTEXT(CONTEXT) \
    extern NoDltContext CONTEXT;

#define DLT_REGISTER_CONTEXT(CONTEXT, CONTEXT_ID, DESC) \
    CONTEXT = CONTEXT_ID;

#define DLT_REGISTER_APP(CONTEXT, DESC) ;

#define DLT_UNREGISTER_CONTEXT(CONTEXT) ;
#define DLT_UNREGISTER_APP() ;
#define dlt_free() ;

// log calls
#if (!DEBUG_ENABLED)

#define LOG_VERBOSE_MSG(context, msg) ;
#define LOG_VERBOSE(context, fmt, ...) ;

#define LOG_DEBUG_MSG(context, msg) ;
#define LOG_DEBUG(context, fmt, ...) ;

#define LOG_INFO_MSG(context, msg) ;
#define LOG_INFO(context, fmt, ...) ;

#define LOG_WARNING_MSG(context, msg) ;
#define LOG_WARNING(context, fmt, ...) ;

#else

#define LOG_VERBOSE_MSG(context, msg) \
        fprintf(stderr, "[VERBO][%4s] " msg "\n", context)
#define LOG_VERBOSE(context, fmt, ...) \
        fprintf(stderr, "[VERBO][%4s] " fmt "\n", context, __VA_ARGS__)

#define LOG_DEBUG_MSG(context, msg) \
        fprintf(stderr, "[DEBUG][%4s] " msg "\n", context)
#define LOG_DEBUG(context, fmt, ...) \
        fprintf(stderr, "[DEBUG][%4s] " fmt "\n", context, __VA_ARGS__)

#define LOG_INFO_MSG(context, msg) \
        fprintf(stderr, "[INFO ][%4s] " msg "\n", context)
#define LOG_INFO(context, fmt, ...) \
        fprintf(stderr, "[INFO ][%4s] " fmt "\n", context, __VA_ARGS__)

#define LOG_WARNING_MSG(context, msg) \
        fprintf(stderr, "[WARN ][%4s] " msg "\n", context)
#define LOG_WARNING(context, fmt, ...) \
        fprintf(stderr, "[WARN ][%4s] " fmt "\n", context, __VA_ARGS__)
#endif

#define LOG_ERROR_MSG(context, msg) \
        fprintf(stderr, "[ERROR][%4s] " msg "\n", context)
#define LOG_ERROR(context, fmt, ...) \
        fprintf(stderr, "[ERROR][%4s] " fmt "\n", context, __VA_ARGS__)

#define LOG_FATAL_MSG(context, msg) \
        fprintf(stderr, "[FATAL][%4s] " msg "\n", context)
#define LOG_FATAL(context, fmt, ...) \
        fprintf(stderr, "[FATAL][%4s] " fmt "\n", context, __VA_ARGS__)

#else   /* DLT_ENABLED */
/*****************************************************************************/
// use DLT
#include "dlt.h"

typedef const char* Context;

#define LOG_VERBOSE_MSG(context, msg) \
        DLT_LOG(context, DLT_LOG_VERBOSE, DLT_STRING(msg));
#define LOG_VERBOSE(context, fmt, ...) \
        { \
            char logBuffer[256]; \
            sprintf(logBuffer, fmt, __VA_ARGS__); \
            DLT_LOG(context, DLT_LOG_VERBOSE, DLT_STRING(logBuffer)); \
        }
#define LOG_DEBUG_MSG(context, msg) \
        DLT_LOG(context, DLT_LOG_DEBUG, DLT_STRING(msg));
#define LOG_DEBUG(context, fmt, ...) \
        { \
            char logBuffer[256]; \
            sprintf(logBuffer, fmt, __VA_ARGS__); \
            DLT_LOG(context, DLT_LOG_DEBUG, DLT_STRING(logBuffer)); \
        }
#define LOG_INFO_MSG(context, msg) \
        DLT_LOG(context, DLT_LOG_INFO, DLT_STRING(msg));
#define LOG_INFO(context, fmt, ...) \
        { \
            char logBuffer[256]; \
            sprintf(logBuffer, fmt, __VA_ARGS__); \
            DLT_LOG(context, DLT_LOG_INFO, DLT_STRING(logBuffer)); \
        }
#define LOG_WARNING_MSG(context, msg) \
        DLT_LOG(context, DLT_LOG_WARN, DLT_STRING(msg));
#define LOG_WARNING(context, fmt, ...) \
        { \
            char logBuffer[256]; \
            sprintf(logBuffer, fmt, __VA_ARGS__); \
            DLT_LOG(context, DLT_LOG_WARN, DLT_STRING(logBuffer)); \
        }
#define LOG_ERROR_MSG(context, msg) \
        DLT_LOG(context, DLT_LOG_ERROR, DLT_STRING(msg));
#define LOG_ERROR(context, fmt, ...) \
        { \
            char logBuffer[256]; \
            sprintf(logBuffer, fmt, __VA_ARGS__); \
            DLT_LOG(context, DLT_LOG_ERROR, DLT_STRING(logBuffer)); \
        }
#define LOG_FATAL_MSG(context, msg) \
        DLT_LOG(context, DLT_LOG_FATAL, DLT_STRING(msg));
#define LOG_FATAL(context, fmt, ...) \
        { \
            char logBuffer[256]; \
            sprintf(logBuffer, fmt, __VA_ARGS__); \
            DLT_LOG(context, DLT_LOG_FATAL, DLT_STRING(logBuffer)); \
        }

#endif  /* DLT_ENABLED */

#endif /* INCLUDE_LOG */
