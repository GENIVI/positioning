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
# Update (2014/12/02) : Philippe Colliot <philippe.colliot@mpsa.com>,
#				PSA Peugeot Citroen
#		- introduce debug flag to disable verbosity
# @licence end@
###########################################################################

project(log-replayer)
cmake_minimum_required(VERSION 2.6.0)

option(WITH_DLT
    "Enable the build to enable DLT logging " OFF)

option(WITH_TESTS
    "Compile test applications" OFF)

message(STATUS "---------------------------------------------------------")

add_subdirectory(src)
message(STATUS "---------------------------------------------------------")

add_subdirectory(logs)
message(STATUS "---------------------------------------------------------")

if(WITH_TESTS)
    add_subdirectory(test)
    message(STATUS "---------------------------------------------------------")
endif()
