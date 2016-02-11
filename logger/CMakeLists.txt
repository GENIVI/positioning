###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: Logger
#
# Author: Helmut Schmidt
#
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Update (2015/07/10) : Helmut Schmidt <https://github.com/huirad>,
#		       - first version derived from log-replayer CMakeLists.txt
# @licence end@
###########################################################################

project(logger)
cmake_minimum_required(VERSION 2.6.0)

option(WITH_DLT
    "Enable the build to enable DLT logging " OFF)

option(WITH_TESTS
    "Compile test applications" OFF)

option(WITH_GPSD
    "Use GPSD as source of GPS data" OFF)

option(WITH_NMEA
    "Use NMEA as source of GPS data" OFF)
    
option(WITH_SENSORS
    "Use real sensors connected to the target device" OFF)
    
option(WITH_REPLAYER
    "Use REPLAYER as source of GPS data" ON)

add_subdirectory(src)
message(STATUS "---------------------------------------------------------")

if(WITH_TESTS)
    add_subdirectory(test)
    message(STATUS "---------------------------------------------------------")
endif()
