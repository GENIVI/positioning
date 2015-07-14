###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
#
# Author: Marco Residori
#
# Copyright (C) 2015, XS Embedded GmbH
# 
# Note:
# The current file is an adaptation of the example file
# http://git.projects.genivi.org/?p=ipc/common-api-tools.git;a=blob;f=CommonAPI-Examples/E01HelloWorld/CMakeLists.txt
#
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

cmake_minimum_required (VERSION 2.8.1)

PROJECT(EnhancedPositionService)

message("Project name: ${PROJECT_NAME}")

option(COMMONAPI_TOOL_GENERATOR
       "Absolute path to CommonAPI code generator" "")
option(COMMONAPI_DBUS_TOOL_GENERATOR
       "Absolute path to CommonAPI-DBus code generator" "")

if (COMMONAPI_TOOL_GENERATOR)
    message (STATUS "COMMONAPI_TOOL_GENERATOR = " ${COMMONAPI_TOOL_GENERATOR})
else ()
    message (FATAL_ERROR "Set the variable -DCOMMONAPI_TOOL_GENERATOR=<path>")
endif ()


if (COMMONAPI_DBUS_TOOL_GENERATOR)
    message (STATUS "COMMONAPI_DBUS_TOOL_GENERATOR = " ${COMMONAPI_DBUS_TOOL_GENERATOR})
else ()
    message (FATAL_ERROR "Set the variable -DCOMMONAPI_DBUS_TOOL_GENERATOR=<path>")
endif ()

#set(FRANCA_DIR "..")
#set(COMMONAPI_TOOL_GENERATOR "${FRANCA_DIR}/generators/common-api-tools/commonapi-generator-linux-x86" )
#set(COMMONAPI_DBUS_TOOL_GENERATOR "${FRANCA_DIR}/generators/common-api-dbus-tools/commonapi-dbus-generator-linux-x86" )

set(COMMONAPI_FIDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(COMMONAPI_GEN_DIR "${CMAKE_BINARY_DIR}/enhanced-position-service/franca/api")

find_package(PkgConfig REQUIRED)

include(FindPkgConfig)

message("CMAKE_BINARY_DIR" ${CMAKE_BINARY_DIR})
message("COMMONAPI_GEN_DIR" ${COMMONAPI_GEN_DIR})
message("COMMONAPI_FIDL_DIR" ${COMMONAPI_FIDL_DIR})
message("CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})

# generate code from fidl
file(GLOB FIDL_FILES "${COMMONAPI_FIDL_DIR}/*.fidl")
message("FIDL_FILES: ${FIDL_FILES}")

execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR}
                COMMAND ${COMMONAPI_DBUS_TOOL_GENERATOR} -d ${COMMONAPI_GEN_DIR} ${FIDL_FILES})
execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR}
                COMMAND ${COMMONAPI_TOOL_GENERATOR} -sk -d ${COMMONAPI_GEN_DIR} ${FIDL_FILES})


