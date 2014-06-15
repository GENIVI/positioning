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
# List of changes:
# 
# 15/6/2014, Philippe Colliot, migrate to Qt5
#
# @licence end@
###########################################################################

cmake_minimum_required(VERSION 2.8.11)

project(test-client)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Tell CMake where are Qt packages
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

# Tell CMake what Qt components are used
find_package(Qt5Core)
find_package(Qt5DBus)
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtDBus_location Qt5::DBus LOCATION)
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5DBus_EXECUTABLE_COMPILE_FLAGS}")

message(STATUS "ENHANCED-POSITION-SERVICE TEST-APPLICATION")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")

set(LIBRARIES 
	${Qt5Core_LIBRARIES}
	${Qt5DBus_LIBRARIES}
)

add_library(ClientEnhpos STATIC
	demoif.cpp
	testclient.cpp
	confif.cpp
	ConfigurationClient.cpp
	../src/genivi-version.cpp
	signalhelper.cpp
)

# Specify the paths in which the linker should search for libraries. 
# The command will apply only to targets created after it is called.
link_directories(${QT_LIBRARY_DIR})

# Tell CMake to create the test-client executable
add_executable(test-client main-client.cpp)

target_link_libraries(test-client
    ClientEnhpos
	${LIBRARIES}
)

