###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: GNSSService
#
# Author: Marco Residori
#
# Copyright (C) 2013, XS Embedded GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Update (2014/12/02) : Philippe Colliot <philippe.colliot@mpsa.com>,
#				PSA Peugeot Citroen
#		- introduce debug flag to disable verbosity
# @licence end@
###########################################################################

message(STATUS "LIB-GNSS-SERVICE")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_NMEA = ${WITH_NMEA}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_TESTS = ${WITH_TESTS}")
message(STATUS "WITH_DEBUG = ${WITH_DEBUG}")

include_directories("${PROJECT_SOURCE_DIR}/api")

find_package(PkgConfig)

set(LIBRARIES pthread)

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

if(WITH_DEBUG)
    add_definitions("-DDEBUG_ENABLED=1")
endif()

if(WITH_GPSD)
    pkg_check_modules(GPSD libgps>=2.94)
    #generate library using gpsd as input
    set(LIB_SRC_USE_GPSD ${CMAKE_CURRENT_SOURCE_DIR}/gnss-use-gpsd.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss-impl.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss-meta-data.c)
    add_library(gnss-service-use-gpsd SHARED ${LIB_SRC_USE_GPSD})
    target_link_libraries(gnss-service-use-gpsd gps ${LIBRARIES})
    install(TARGETS gnss-service-use-gpsd DESTINATION lib)
elseif(WITH_NMEA)
    #generate library using NMEA parser as input
    add_definitions(-DGNSS_DEVICE=${GNSS_DEVICE})
    add_definitions(-DGNSS_BAUDRATE=${GNSS_BAUDRATE})
    if(GNSS_CHIPSET)
        add_definitions(-DGNSS_CHIPSET_${GNSS_CHIPSET})
    endif(GNSS_CHIPSET)
    if(GNSS_DELAY)
        add_definitions(-DGNSS_DELAY=${GNSS_DELAY})
    endif(GNSS_DELAY)
    set(LIB_SRC_USE_NMEA ${CMAKE_CURRENT_SOURCE_DIR}/gnss-use-nmea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hnmea.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss-impl.c
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss-meta-data.c)
    add_library(gnss-service-use-nmea SHARED ${LIB_SRC_USE_NMEA})
    target_link_libraries(gnss-service-use-nmea pthread ${LIBRARIES})
    install(TARGETS gnss-service-use-nmea DESTINATION lib)
    #for glibc <2.17, clock_gettime is in librt: http://linux.die.net/man/2/clock_gettime
    #TODO: is there a nice way to detect glibc version in CMake?
    set(LIBRARIES ${LIBRARIES} rt)
elseif(WITH_REPLAYER)
    #generate library using replayer as input
    set(LIB_SRC_USE_REPLAYER ${CMAKE_CURRENT_SOURCE_DIR}/gnss-use-replayer.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss-impl.c 
    ${CMAKE_CURRENT_SOURCE_DIR}/gnss-meta-data.c)
    add_library(gnss-service-use-replayer SHARED ${LIB_SRC_USE_REPLAYER})
    target_link_libraries(gnss-service-use-replayer ${LIBRARIES})
    install(TARGETS gnss-service-use-replayer DESTINATION lib)
else()
    message(STATUS "Invalid cmake options!")
endif()











