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
# @licence end@
###########################################################################

project(position-daemon)
cmake_minimum_required(VERSION 2.6.0)

message(STATUS "ENHANCED-POSITION-SERVICE")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_TESTS = ${WITH_TESTS}")

include_directories("${PROJECT_SOURCE_DIR}/../../common")

find_package(PkgConfig)

link_directories(
    ${QT_LIBRARY_DIR}
    ${gnss-service_LIBRARY_DIRS}
    ${sensors-service_LIBRARY_DIRS})

set(server_HEADERS
	demoifadaptor.h
	serverimpl.h
	confifadaptor.h
	ConfigurationImpl.h
)
qt4_wrap_cpp(server_HEADERS_MOC ${server_HEADERS})

add_executable(position-daemon
	main.cpp
	demoifadaptor.cpp
	serverimpl.cpp
	PositionFilter.cpp
	confifadaptor.cpp
	ConfigurationImpl.cpp
	genivi-version.cpp
	${server_HEADERS_MOC}
)

set(LIBRARIES 
    ${QT_QTDBUS_LIBRARY}
    #${QT_LIBRARIES}
    QtCore QtDBus QtXml # should not be required, failed automatic library detaction
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

target_link_libraries(position-daemon ${LIBRARIES})

message(STATUS "QT_LIBRARIES: " ${QT_LIBRARIES})
message(STATUS "QT_LIBRARY_DIR: " ${QT_LIBRARY_DIR})
message(STATUS "QT_DBUS_LIBRARY: " ${QT_QTDBUS_LIBRARY})
message(STATUS "GNSS_SERVICE_LIBRARIES: " ${gnss-service_LIBRARIES})
message(STATUS "SENSORS_SERVICE_LIBRARIES: " ${sensors-service_LIBRARIES})


