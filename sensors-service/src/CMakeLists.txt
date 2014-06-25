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

message(STATUS "LIB-SENSORS-SERVICE")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_IPHONE = ${WITH_IPHONE}")
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

if(WITH_IPHONE)
    #generate library using iphone as input
    set(LIB_SRC_USE_IPHONE ${CMAKE_SOURCE_DIR}/src/sns-use-iphone.c 
             ${CMAKE_SOURCE_DIR}/src/wheeltick.c 
             ${CMAKE_SOURCE_DIR}/src/gyroscope.c
             ${CMAKE_SOURCE_DIR}/src/vehicle-speed.c
             ${CMAKE_SOURCE_DIR}/src/sns-meta-data.c)

    add_library(sensors-service-use-iphone SHARED ${LIB_SRC_USE_IPHONE})
    target_link_libraries(sensors-service-use-iphone ${LIBRARIES})
elseif(WITH_REPLAYER)
    #generate library using replayer as input
    set(LIB_SRC_USE_REPLAYER ${CMAKE_SOURCE_DIR}/src/sns-use-replayer.c 
             ${CMAKE_SOURCE_DIR}/src/wheeltick.c 
             ${CMAKE_SOURCE_DIR}/src/gyroscope.c 
             ${CMAKE_SOURCE_DIR}/src/vehicle-speed.c
             ${CMAKE_SOURCE_DIR}/src/sns-meta-data.c)

    add_library(sensors-service-use-replayer SHARED ${LIB_SRC_USE_REPLAYER})
    target_link_libraries(sensors-service-use-replayer ${LIBRARIES})
else()
    message(STATUS "Invalid cmake options!")
endif()















