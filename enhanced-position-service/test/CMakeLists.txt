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
# @licence end@
###########################################################################

project(enhanced-position-client)
cmake_minimum_required(VERSION 2.6.0)

message(STATUS "ENHANCED-POSITION-CLIENT")
message(STATUS "WITH_DLT = ${WITH_DLT}")

find_package(PkgConfig REQUIRED)

pkg_check_modules(DBUS_CPP dbus-c++-1)

include_directories(${DBUS_CPP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${DBUS_CPP_INCLUDE_DIRS})

# Generate the client glue code before building the client
add_custom_command(
  OUTPUT  enhanced-position-proxy.h 
  COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-enhancedposition.xml 
  --proxy=enhanced-position-proxy.h
  OUTPUT  position-feedback-proxy.h 
  COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-positionfeedback.xml 
  --proxy=position-feedback-proxy.h
  OUTPUT  configuration-proxy.h 
  COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-configuration.xml
  --proxy=configuration-proxy.h
  OUTPUT positioning-constants.h 
  COMMAND xsltproc ${CMAKE_CURRENT_SOURCE_DIR}/../api/enum.xsl ${CMAKE_CURRENT_SOURCE_DIR}/../api/genivi-positioning-constants.xml > positioning-constants.h
)

link_directories(${DBUS_CPP_LIBRARY_DIRS})

add_executable(enhanced-position-client
    enhanced-position-proxy.h
    enhanced-position-client.h
    enhanced-position-client.cpp
    position-feedback-proxy.h
    configuration-proxy.h
    positioning-constants.h
)

set(LIBRARIES 
    ${DBUS_CPP_LIBRARIES}
)

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/..")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

target_link_libraries(enhanced-position-client ${LIBRARIES})

message(STATUS "DBUS_CPP_LIBRARIES: " ${DBUS_CPP_LIBRARIES})



