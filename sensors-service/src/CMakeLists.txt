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
message(STATUS "WITH_SENSORS = ${WITH_SENSORS}")
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
elseif(WITH_SENSORS)
    #generate library using real sensors as input
    if(IMU_TYPE)
        #supported IMU types: MPU6050, LSM9DS1, ...
        add_definitions(-DIMU_TYPE_${IMU_TYPE})
    elseif(IMU_TYPE)
        add_definitions(-DIMU_TYPE_MPU6050)
    endif(IMU_TYPE)
    #TODO: move FindI2CDEV stuff to a separate file?
    find_file(I2CDEV_H
        i2c-dev.h
        PATHS
            /usr/include/linux/
    )
    if (I2CDEV_H)
        message(STATUS "OK: i2c-dev.h found: ${I2CDEV_H}")
    else (I2CDEV_H)
        message(STATUS "WARNING: i2c-dev.h not found. To enable I2C communication, install the respective package, e.g. libi2c-dev or i2c-tools")
        add_definitions(-DI2C_NOT_AVAILABLE)
    endif (I2CDEV_H)
    set(LIB_SRC_USE_SENSORS ${CMAKE_CURRENT_SOURCE_DIR}/sns-use-sensors.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/i2ccomm.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/mpu6050.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/lsm9ds1.cpp
             ${CMAKE_CURRENT_SOURCE_DIR}/gyroscope.c
             ${CMAKE_CURRENT_SOURCE_DIR}/acceleration.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-data.c
             ${CMAKE_CURRENT_SOURCE_DIR}/vehicle-speed.c
             ${CMAKE_CURRENT_SOURCE_DIR}/wheeltick.c
             ${CMAKE_CURRENT_SOURCE_DIR}/sns-meta-data.c)
    add_library(sensors-service-use-sensors SHARED ${LIB_SRC_USE_SENSORS})
    target_link_libraries(sensors-service-use-sensors ${LIBRARIES})
    install(TARGETS sensors-service-use-sensors DESTINATION lib)
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















