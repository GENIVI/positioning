###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: SensorsService
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
# @licence end@
###########################################################################

message(STATUS "SENSORS-SERVICE-CLIENT")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_IPHONE = ${WITH_IPHONE}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")

include_directories("${PROJECT_SOURCE_DIR}/api")
include_directories("${PROJECT_SOURCE_DIR}/src")

find_package(PkgConfig)

if(WITH_REPLAYER)
    set(LIBRARIES sensors-service-use-replayer)
elseif(WITH_IPHONE)
    set(LIBRARIES sensors-service-use-iphone)
else()
    message(STATUS "Invalid cmake option!")
endif()

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    link_directories ( ${DLT_LIBRARY_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

set(SRCS ${CMAKE_SOURCE_DIR}/test/sensors-service-client.c)
add_executable(sensors-service-client ${SRCS})
target_link_libraries(sensors-service-client ${LIBRARIES})


