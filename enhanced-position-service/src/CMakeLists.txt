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

project(enhanced-position-service)
cmake_minimum_required(VERSION 2.6.0)

message(STATUS "ENHANCED-POSITION-SERVICE")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_TESTS = ${WITH_TESTS}")
message(STATUS "WITH_DEBUG = ${WITH_DEBUG}")

find_package(PkgConfig REQUIRED)

pkg_check_modules(DBUS_CPP dbus-c++-1)

include_directories("${PROJECT_SOURCE_DIR}/../api")
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${DBUS_CPP_INCLUDE_DIRS})
include_directories(${gnss-service_INCLUDE_DIRS})
include_directories(${sensors-service_INCLUDE_DIRS})
#include_directories(${enhanced-position-service_INCLUDE_DIRS})

# Generate the client glue code before building the client
add_custom_command(
   OUTPUT  enhanced-position-adaptor.h 
   COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-enhancedposition.xml 
   --adaptor=enhanced-position-adaptor.h
   OUTPUT  position-feedback-adaptor.h 
   COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-positionfeedback.xml 
   --adaptor=position-feedback-adaptor.h
   OUTPUT  configuration-adaptor.h 
   COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-configuration.xml
   --adaptor=configuration-adaptor.h
   OUTPUT positioning-constants.h 
   COMMAND xsltproc ${CMAKE_CURRENT_SOURCE_DIR}/../api/enum.xsl ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-constants.xml > positioning-constants.h
)

link_directories(
    ${DBUS_CPP_LIBRARY_DIRS}
    ${gnss-service_LIBRARY_DIRS}
    ${sensors-service_LIBRARY_DIRS})

add_executable(enhanced-position-service
    main.cpp
    enhanced-position.cpp
    enhanced-position.h
    enhanced-position-adaptor.h
    position-feedback.cpp
    position-feedback.h
    position-feedback-adaptor.h
    configuration.cpp
    configuration.h
    configuration-adaptor.h
)

set(LIBRARIES 
    ${DBUS_CPP_LIBRARIES}
    ${gnss-service_LIBRARIES}
    ${sensors-service_LIBRARIES}
)

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/..")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

if(WITH_DEBUG)
    add_definitions("-DDEBUG_ENABLED=1")
endif()

target_link_libraries(enhanced-position-service ${LIBRARIES})

message(STATUS "DBUS_CPP_LIBRARIES: " ${DBUS_CPP_LIBRARIES})
message(STATUS "GNSS_SERVICE_LIBRARIES: " ${gnss-service_LIBRARIES})
message(STATUS "SENSORS_SERVICE_LIBRARIES: " ${sensors-service_LIBRARIES})



