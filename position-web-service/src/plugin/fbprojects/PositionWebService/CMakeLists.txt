###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: PositionWebService
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

cmake_minimum_required (VERSION 2.6)
set (CMAKE_BACKWARDS_COMPATIBILITY 2.6)

Project(${PLUGIN_NAME})

find_package(PkgConfig REQUIRED)

pkg_check_modules(DBUS_CPP dbus-c++-1)

include_directories(${DBUS_CPP_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${DBUS_CPP_INCLUDE_DIRS})

set(ENHANCED_POS_SERVICE_API ${CMAKE_CURRENT_SOURCE_DIR}/../../../../../enhanced-position-service/dbus/api)

message(STATUS "ENHANCED_POS_SERVICE_API: " ${ENHANCED_POS_SERVICE_API})

message(STATUS "BINARY DIR: " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "BINARY DIR: " ${DBUS_CPP_INCLUDE_DIRS})

add_custom_target (
  interfaceGenerator ALL
  COMMAND echo "code generation"
  OUTPUT  enhanced-position-proxy.h 
  COMMAND dbusxx-xml2cpp ${ENHANCED_POS_SERVICE_API}/genivi-positioning-enhancedposition.xml 
  --proxy=enhanced-position-proxy.h
  OUTPUT  position-feedback-proxy.h 
  COMMAND dbusxx-xml2cpp ${ENHANCED_POS_SERVICE_API}/genivi-positioning-positionfeedback.xml 
  --proxy=position-feedback-proxy.h
  OUTPUT  configuration-proxy.h 
  COMMAND dbusxx-xml2cpp ${ENHANCED_POS_SERVICE_API}/genivi-positioning-configuration.xml
  --proxy=configuration-proxy.h
  OUTPUT positioning-constants.h 
  COMMAND xsltproc ${ENHANCED_POS_SERVICE_API}/enum.xsl ${ENHANCED_POS_SERVICE_API}/genivi-positioning-constants.xml > positioning-constants.h
)

add_dependencies(NpapiCore interfaceGenerator)
set_property(SOURCE EnhancedPositionDispatcher.cpp APPEND PROPERTY DEPEND interfaceGenerator)

file (GLOB GENERAL RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    *.cpp
    *.h
    *.cmake
    *.html
    )

include_directories(${PLUGIN_INCLUDE_DIRS})
pkg_check_modules(dbus REQUIRED dbus-1)
include_directories(${dbus_INCLUDE_DIRS} ${DBUS_CPP_LIBRARY_DIRS})

link_directories(${DBUS_CPP_LIBRARY_DIRS})

# Generated files are stored in ${GENERATED} by the project configuration
set_source_files_properties(
    ${GENERATED}
    PROPERTIES
        GENERATED 1
    )

source_group(Generated FILES
    ${GENERATED}
    )


set( SOURCES
    ${GENERAL}
    ${GENERATED}
    )


include_platform()
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${DBUS_CPP_LIBRARIES} ${dbus_LIBRARIES})











