###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: franca enhanced position service generation
#
# Author: Philippe Colliot
#
# Copyright (C) 2018, PSA Groupe
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

project(franca-enhancedpositionservice)

get_directory_property(hasParent PARENT_DIRECTORY)

if(hasParent)
    set(COMMONAPI_GEN_DIR "${COMMONAPI_GEN_DIR}" PARENT_SCOPE)
else()
    set(COMMONAPI_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/src-gen")
endif()

option(COMMONAPI_TOOL_GENERATOR
       "Absolute path to CommonAPI code generator" "")
option(COMMONAPI_DBUS_TOOL_GENERATOR
       "Absolute path to CommonAPI-DBus code generator" "")
option(COMMONAPI_WAMP_TOOL_GENERATOR
       "Absolute path to CommonAPI-Wamp code generator" "")
option(WITH_WAMP_GENERATION
       "Generate Wamp files" OFF)

set(COMMONAPI_FIDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(PkgConfig REQUIRED)

include(FindPkgConfig)

# generate code from fidl
if(NOT COMMONAPI_GEN_DIR)
     message (FATAL_ERROR "Set the variable -DCOMMONAPI_GEN_DIR=<path>")
endif()

file(GLOB FIDL_FILES "${COMMONAPI_FIDL_DIR}/*.fidl")

if (COMMONAPI_TOOL_GENERATOR)
     execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR}
                COMMAND ${COMMONAPI_TOOL_GENERATOR} -sk -d ${COMMONAPI_GEN_DIR} ${FIDL_FILES})
else ()
    message (FATAL_ERROR "Set the variable -DCOMMONAPI_TOOL_GENERATOR=<tool>")
endif ()

if (WITH_WAMP_GENERATION)
    if (COMMONAPI_WAMP_TOOL_GENERATOR)
        foreach(RAW_FILE ${FIDL_FILES})
        execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR}
                    COMMAND java -jar ${COMMONAPI_WAMP_TOOL_GENERATOR} -f ${RAW_FILE} -d ${COMMONAPI_GEN_DIR} )
        endforeach()
    else ()
        message (FATAL_ERROR "Set the variable -DCOMMONAPI_WAMP_TOOL_GENERATOR=<tool>")
    endif ()
else()
    if (COMMONAPI_DBUS_TOOL_GENERATOR)
        execute_process(WORKING_DIRECTORY ${COMMONAPI_FIDL_DIR}
                    COMMAND ${COMMONAPI_DBUS_TOOL_GENERATOR} -d ${COMMONAPI_GEN_DIR} ${FIDL_FILES})
    else ()
        message (FATAL_ERROR "Set the variable -DCOMMONAPI_DBUS_TOOL_GENERATOR=<tool>")
    endif ()
endif()

