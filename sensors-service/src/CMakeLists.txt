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
# Update (2014/12/02) : Philippe Colliot <philippe.colliot@mpsa.com>,
#				PSA Peugeot Citroen
#		- introduce debug flag to disable verbosity
# @licence end@
###########################################################################

message(STATUS "LIB-SENSORS-SERVICE")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_IPHONE = ${WITH_IPHONE}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_MPU6050 = ${WITH_MPU6050}")
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

if(WITH_IPHONE)
    #generate library using iphone as input
    set(LIB_SRC_USE_IPHONE ${CMAKE_CURRENT_SOURCE_DIR}/sns-use-iphone.c 
             ${CMAKE_CURRENT_SOURCE_DIR}/wheeltick.c 
             ${CMAKE_CURRENT_SOURCE_DIR}/gyroscope.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-data.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-speed.c
             ${CMAKE_CURRENT_SOURCE_DIR}/sns-meta-data.c)

    add_library(sensors-service-use-iphone SHARED ${LIB_SRC_USE_IPHONE})
    target_link_libraries(sensors-service-use-iphone ${LIBRARIES})
    install(TARGETS sensors-service-use-iphone DESTINATION lib)
elseif(WITH_MPU6050)
    #generate library using replayer as input
    set(LIB_SRC_USE_MPU6050 ${CMAKE_CURRENT_SOURCE_DIR}/sns-use-mpu6050.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/mpu6050.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/gyroscope.c
             ${CMAKE_CURRENT_SOURCE_DIR}/acceleration.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-data.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-speed.c
             ${CMAKE_CURRENT_SOURCE_DIR}/wheeltick.c
             ${CMAKE_CURRENT_SOURCE_DIR}/sns-meta-data.c)
    add_library(sensors-service-use-mpu6050 SHARED ${LIB_SRC_USE_MPU6050})
    target_link_libraries(sensors-service-use-mpu6050 ${LIBRARIES})
    install(TARGETS sensors-service-use-mpu6050 DESTINATION lib)
    #for glibc <2.17, clock_gettime is in librt: http://linux.die.net/man/2/clo$
    #TODO: is there a nice way to detect glibc version in CMake?
    set(LIBRARIES ${LIBRARIES} rt)
elseif(WITH_REPLAYER)
    #generate library using replayer as input
    set(LIB_SRC_USE_REPLAYER ${CMAKE_CURRENT_SOURCE_DIR}/sns-use-replayer.c 
             ${CMAKE_CURRENT_SOURCE_DIR}/wheeltick.c 
             ${CMAKE_CURRENT_SOURCE_DIR}/gyroscope.c 
             ${CMAKE_CURRENT_SOURCE_DIR}/acceleration.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-data.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-speed.c
             ${CMAKE_CURRENT_SOURCE_DIR}/sns-meta-data.c)

    add_library(sensors-service-use-replayer SHARED ${LIB_SRC_USE_REPLAYER})
    target_link_libraries(sensors-service-use-replayer ${LIBRARIES})
    install(TARGETS sensors-service-use-replayer DESTINATION lib)
else()
    message(STATUS "Invalid cmake options!")
endif()















