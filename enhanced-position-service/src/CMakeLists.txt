###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
#
# Author: Thomas Bader, Marco Residori
#
# Copyright (C) 2012, BMW Car IT GmbH, XS Embedded GmbH
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

project(position-daemon)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Manage packages
find_package(PkgConfig)

# Set Qt stuff
find_package(Qt5Core)
find_package(Qt5DBus)
get_target_property(QtCore_location Qt5::Core LOCATION)
get_target_property(QtDBus_location Qt5::DBus LOCATION)
include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5DBus_INCLUDE_DIRS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Core_EXECUTABLE_COMPILE_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5DBus_EXECUTABLE_COMPILE_FLAGS}")

message(STATUS "ENHANCED-POSITION-SERVICE")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_TESTS = ${WITH_TESTS}")

include_directories("${PROJECT_SOURCE_DIR}/../../common")

set(LIBRARIES 
	${Qt5Core_LIBRARIES}
	${Qt5DBus_LIBRARIES}
    ${gnss-service_LIBRARIES}
    ${sensors-service_LIBRARIES}
)

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/..")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

# Specify the paths in which the linker should search for libraries. 
# The command will apply only to targets created after it is called.
link_directories(
    ${gnss-service_LIBRARY_DIRS}
    ${sensors-service_LIBRARY_DIRS})

# Tell CMake to create the position-daemon executable
add_executable(position-daemon
	main.cpp
	demoifadaptor.cpp
	serverimpl.cpp
	PositionFilter.cpp
	confifadaptor.cpp
	ConfigurationImpl.cpp
	genivi-version.cpp
)

target_link_libraries(position-daemon
	${LIBRARIES})




