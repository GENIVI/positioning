###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
#
# Author: Thomas Bader
#
# Copyright (C) 2012, BMW Car IT GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# List of changes:
# 
# 15/6/2014, Philippe Colliot, migrate to Qt5
#
# @licence end@
###########################################################################

cmake_minimum_required(VERSION 2.8.11)
project(enhanced-position-service)

message(STATUS)
message(STATUS "---------------------------------------------------------")

set(gnss-service_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../gnss-service/api")
set(gnss-service_LIBRARY_DIRS "${PROJECT_BINARY_DIR}/../gnss-service/src")

if(WITH_REPLAYER)
    set(gnss-service_LIBRARIES "gnss-service-use-replayer")
elseif(WITH_GPSD)
	set(gnss-service_LIBRARIES "gnss-service-use-gpsd")
endif()

set(sensors-service_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/../sensors-service/api")
set(sensors-service_LIBRARY_DIRS "${PROJECT_BINARY_DIR}/../sensors-service/src")

if(WITH_REPLAYER)
   set(sensors-service_LIBRARIES "sensors-service-use-replayer")
endif()

set(QT_USE_QTDBUS TRUE)

include_directories(api src test ${gnss-service_INCLUDE_DIRS} ${sensors-service_INCLUDE_DIRS})

add_subdirectory(src)
message(STATUS "---------------------------------------------------------")

if(WITH_TESTS)
add_subdirectory(test)
message(STATUS "---------------------------------------------------------")

add_subdirectory(test/compliance-test)
message(STATUS "---------------------------------------------------------")
endif()

message(STATUS)
