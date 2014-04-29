###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: Replayer
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

project(log-replayer)
cmake_minimum_required(VERSION 2.6.0)

option(WITH_DLT
    "Enable the build to enable DLT logging " OFF)

option(WITH_TESTS
    "Compile test applications" OFF)
 
option(WITH_MESSAGES
    "Enable the build to print messages" OFF)
    
message(STATUS)
message(STATUS "---------------------------------------------------------")

add_subdirectory(src)
add_subdirectory(logs)

if(WITH_TESTS)
add_subdirectory(test)
endif()

message(STATUS "---------------------------------------------------------")
message(STATUS)
