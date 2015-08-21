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

option(COMMONAPI_TOOL_GENERATOR
       "Absolute path to CommonAPI code generator" "")
option(COMMONAPI_DBUS_TOOL_GENERATOR
       "Absolute path to CommonAPI-DBus code generator" "")
option(COMMONAPI_SOMEIP_TOOL_GENERATOR
       "Absolute path to CommonAPI-SomeIP code generator" "")

set(COMMONAPI_FIDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(COMMONAPI_GEN_DIR "${CMAKE_BINARY_DIR}/enhanced-position-service/franca/api")

file(GLOB FIDL_FILES "${COMMONAPI_FIDL_DIR}/*.fidl")

# generate CommonAPI generic code from fidl files
if (COMMONAPI_TOOL_GENERATOR)
    execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR}
                COMMAND ${COMMONAPI_TOOL_GENERATOR} -sk -d ${COMMONAPI_GEN_DIR} ${FIDL_FILES})
    message("COMMONAPI_TOOL_GENERATOR=" ${COMMONAPI_TOOL_GENERATOR})
else ()
    message (FATAL_ERROR "Set the variable -DCOMMONAPI_TOOL_GENERATOR=<path>")
endif ()

# generate DBus specific code from fidl files
if (WITH_FRANCA_DBUS_INTERFACE)

    if (COMMONAPI_DBUS_TOOL_GENERATOR)
        execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR}
                COMMAND ${COMMONAPI_DBUS_TOOL_GENERATOR} -d ${COMMONAPI_GEN_DIR} ${FIDL_FILES})
        message("COMMONAPI_DBUS_TOOL_GENERATOR=" ${COMMONAPI_DBUS_TOOL_GENERATOR})
    else ()
        message (FATAL_ERROR "Set the variable -DCOMMONAPI_DBUS_TOOL_GENERATOR=<path>")
    endif ()

endif ()

# generate SomeIP specific code from fidl files
if (WITH_FRANCA_SOMEIP_INTERFACE)

    file(GLOB SOMEIP_FDEPL_FILES "${COMMONAPI_FIDL_DIR}/someip/*.fdepl")
    message("SOMEIP_FDEPL_FILES: ${SOMEIP_FDEPL_FILES}")

    if (COMMONAPI_SOMEIP_TOOL_GENERATOR)
        execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR} 
                COMMAND ${COMMONAPI_SOMEIP_TOOL_GENERATOR} -d ${COMMONAPI_GEN_DIR} ${SOMEIP_FDEPL_FILES})
        message("COMMONAPI_SOMEIP_TOOL_GENERATOR=" ${COMMONAPI_SOMEIP_TOOL_GENERATOR})
    else ()
        message (FATAL_ERROR "Set the variable -DCOMMONAPI_SOMEIP_TOOL_GENERATOR=<path>")
    endif ()

endif ()

message("FIDL_FILES: ${FIDL_FILES}")
message("COMMONAPI_FIDL_DIR" ${COMMONAPI_FIDL_DIR})
message("COMMONAPI_GEN_DIR" ${COMMONAPI_GEN_DIR})
message("CMAKE_BINARY_DIR" ${CMAKE_BINARY_DIR})
message("CMAKE_CURRENT_SOURCE_DIR" ${CMAKE_CURRENT_SOURCE_DIR})








