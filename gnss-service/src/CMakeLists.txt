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

message(STATUS "LIB-GNSS-SERVICE")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_TESTS = ${WITH_TESTS}")

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

if(WITH_GPSD)
    pkg_check_modules(GPSD libgps>=2.94)
    #generate library using gpsd as input
    set(LIB_SRC_USE_GPSD ${CMAKE_SOURCE_DIR}/src/gnss-use-gpsd.c 
    ${CMAKE_SOURCE_DIR}/src/gnss-impl.c 
    ${CMAKE_SOURCE_DIR}/src/gnss-meta-data.c)
    add_library(gnss-service-use-gpsd SHARED ${LIB_SRC_USE_GPSD})
    target_link_libraries(gnss-service-use-gpsd gps ${LIBRARIES})
elseif(WITH_REPLAYER)
    #generate library using replayer as input
    set(LIB_SRC_USE_REPLAYER ${CMAKE_SOURCE_DIR}/src/gnss-use-replayer.c 
    ${CMAKE_SOURCE_DIR}/src/gnss-ext.c 
    ${CMAKE_SOURCE_DIR}/src/gnss-meta-data.c)
    add_library(gnss-service-use-replayer SHARED ${LIB_SRC_USE_REPLAYER})
    target_link_libraries(gnss-service-use-replayer ${LIBRARIES})
else()
    message(STATUS "Invalid cmake options!")
endif()











