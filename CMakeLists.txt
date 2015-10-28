###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: Positioning Top-level Makefile
#
# Author: Jonathan Maw
#
# Copyright (C) 2015, Codethink Ltd
#
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Update (2015/05/11) : Jonathan Maw <jonathan.maw@codethink.co.uk>
#               - First write
# Update (2015/08/19) : Marco Residori <marco_residori@mentor.com>
#               - Added option WITH_FRANCA_SOMEIP_INTERFACE
#               - Renamed WITH_FRANCA_INTERFACE to WITH_FRANCA_DBUS_INTERFACE
# Update (2015/10/28) : Helmut Schmidt <Helmut.3.Schmidt@continental-corporation.com>
#               - Add option for logger
# @licence end@
###########################################################################
cmake_minimum_required(VERSION 2.6.0)

option(WITH_ENHANCED_POSITION_SERVICE
       "Build the Enhanced Positioning Service" ON)
option(WITH_GNSS_SERVICE
       "Build the GNSS Service" ON)
option(WITH_LOG_REPLAYER
       "Build the Log Replayer" ON)
option(WITH_LOGGER
       "Build the Logger" ON)       
option(WITH_SENSORS_SERVICE
       "Build the Sensors Service" ON)
option(WITH_FRANCA_DBUS_INTERFACE
       "Build using the Franca DBus interfaces" OFF)
option(WITH_FRANCA_SOMEIP_INTERFACE
       "Build using the Franca SomeIP interfaces" OFF)
option(WITH_DBUS_INTERFACE
       "Build using the D-Bus interfaces" ON)
option(WITH_DEBUG
       "Enable the debug messages" OFF)

message(STATUS "---------------------------------------------------------")

if(WITH_ENHANCED_POSITION_SERVICE)
	add_subdirectory(enhanced-position-service)
endif(WITH_ENHANCED_POSITION_SERVICE)

if(WITH_GNSS_SERVICE)
	add_subdirectory(gnss-service)
endif(WITH_GNSS_SERVICE)

if(WITH_LOG_REPLAYER)
	add_subdirectory(log-replayer)
endif(WITH_LOG_REPLAYER)

if(WITH_LOGGER)
	add_subdirectory(logger)
endif(WITH_LOGGER)

if(WITH_SENSORS_SERVICE)
	add_subdirectory(sensors-service)
endif(WITH_SENSORS_SERVICE)
