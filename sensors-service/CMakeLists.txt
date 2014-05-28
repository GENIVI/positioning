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

project(sensors-service)
cmake_minimum_required(VERSION 2.6.0)

if(${CMAKE_INSTALL_RPATH})
    set(CMAKE_INSTALL_RPATH ${LIBRARY_INSTALL_PATH}:${CMAKE_INSTALL_RPATH})
else(${CMAKE_INSTALL_RPATH})
    set(CMAKE_INSTALL_RPATH ${LIBRARY_INSTALL_PATH})
endif(${CMAKE_INSTALL_RPATH})

option(WITH_DLT
    "Enable DLT logging" OFF)

option(WITH_REPLAYER
    "Use REPLAYER as source of sensors data" ON)

option(WITH_IPHONE
    "Use IPHONE as source of sensors data" OFF)

option(WITH_TESTS
    "Compile test applications" OFF)

message(STATUS)
message(STATUS "---------------------------------------------------------")

add_subdirectory(src)
message(STATUS "---------------------------------------------------------")

if(WITH_TESTS)
add_subdirectory(test)
message(STATUS "---------------------------------------------------------")
endif()

message(STATUS)
