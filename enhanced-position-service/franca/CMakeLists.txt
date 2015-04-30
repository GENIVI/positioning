###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
#
# Author: Marco Residori
#
# Copyright (C) 2014, XS Embedded GmbH
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

project(enhanced-position-service-franca)
cmake_minimum_required(VERSION 2.6.0)

option(WITH_DLT
    "Enable DLT logging" OFF)

option(WITH_GPSD
    "Use GPSD as source of GPS data" OFF)

option(WITH_REPLAYER
    "Use REPLAYER as source of GPS data" ON)

option(WITH_TESTS
    "Compile test applications" OFF)

option(WITH_DEBUG 
	"Enable the debug messages" OFF)

set(gnss-service_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../gnss-service/api")
set(gnss-service_LIBRARY_DIRS "${PROJECT_BINARY_DIR}/../../gnss-service/src")

if(WITH_GPSD)
    set(gnss-service_LIBRARIES "gnss-service-use-gpsd")
elseif(WITH_REPLAYER)
    set(gnss-service_LIBRARIES "gnss-service-use-replayer")
else()
    message(STATUS "Invalid cmake options!")
endif()

set(sensors-service_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../../sensors-service/api")
set(sensors-service_LIBRARY_DIRS "${PROJECT_BINARY_DIR}/../../sensors-service/src")

if(WITH_IPHONE)
    set(sensors-service_LIBRARIES "sensors-service-use-iphone")
elseif(WITH_REPLAYER)
    set(sensors-service_LIBRARIES "sensors-service-use-replayer")
else()
    message(STATUS "Invalid cmake options!")
endif()

include_directories(src test ${gnss-service_INCLUDE_DIRS} ${sensors-service_INCLUDE_DIRS})

#add_subdirectory(api)
add_subdirectory(src)


