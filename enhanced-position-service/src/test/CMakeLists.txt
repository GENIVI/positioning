###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
#
# Author: Thomas Bader
#
# Copyright (C) 2012, BMW Car IT GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

project(enhpos-test)
cmake_minimum_required(VERSION 2.6.0)

message(STATUS)
message(STATUS "---------------------------------------------------------")

# gtest
add_subdirectory(externals)
include_directories(${gtest_SOURCE_DIR}/include)
link_directories(${gtest_BINARY_DIR})

# tests for D-Bus Enhanced Position Service POC:
# This client uses the code of the test-client to speak with the
# position-daemon.
add_executable(test-enhpos
    ${gtest_SOURCE_DIR}/src/gtest_main.cc
    EnhPosTest.cpp
    ConfigurationTest.cpp
)
target_link_libraries(test-enhpos
    ClientEnhpos QtCore QtDBus QtXml
    gtest
)

# unit tests for position-daemon internals
add_executable(test-daemon-internals
# tests
    ${gtest_SOURCE_DIR}/src/gtest_main.cc
    PositionFilterTest.cpp
# files under test
    ../server/PositionFilter.cpp
)
target_link_libraries(test-daemon-internals
    # ClientEnhpos QtDBus QtXml
    gtest
    QtCore
)

message(STATUS "---------------------------------------------------------")
message(STATUS)
