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

project(sensors-service)
cmake_minimum_required(VERSION 2.6.0)

SET(CMAKE_INSTALL_RPATH "")
#SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

option(WITH_DLT
    "Enable DLT logging" OFF)

option(WITH_REPLAYER
    "Use REPLAYER as source of sensors data" ON)

option(WITH_IPHONE
    "Use IPHONE as source of sensors data" OFF)

option(WITH_SENSORS
    "Use real sensors connected to the target device" OFF)

option(WITH_TESTS
    "Compile test applications" OFF)

add_subdirectory(src)
message(STATUS "---------------------------------------------------------")

if(WITH_TESTS)
    add_subdirectory(test)
    message(STATUS "---------------------------------------------------------")
endif()
