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
# @licence end@
###########################################################################

message(STATUS "GNSS-SERVICE-COMPLIANCE-TEST")
message( STATUS "WITH_DLT = ${WITH_DLT}")
message( STATUS "WITH_GPSD = ${WITH_GPSD}")
message( STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")

include_directories("${PROJECT_SOURCE_DIR}/api")
include_directories("${PROJECT_SOURCE_DIR}/src")

find_package(PkgConfig)

set(SRCS ${CMAKE_SOURCE_DIR}/test/compliance-test/gnss-service-compliance-test.c)

add_executable(gnss-service-compliance-test ${SRCS})

if(WITH_GPSD)
    pkg_check_modules(GPSD libgps>=2.94)
    set(LIBRARIES gnss-service-use-gpsd gps)  
elseif(WITH_REPLAYER)
    set(LIBRARIES gnss-service-use-replayer) 
endif()

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

target_link_libraries(gnss-service-compliance-test ${LIBRARIES})

