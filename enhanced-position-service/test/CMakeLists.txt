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

project(position-daemon)
cmake_minimum_required(VERSION 2.6.0)

link_directories(${QT_LIBRARY_DIR})

message(STATUS "ENHANCED-POSITION-SERVICE TEST-APPLICATION")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")

set(client_HEADERS
	demoif.h
	testclient.h
	confif.h
	ConfigurationClient.h
	signalhelper.h
	../src/genivi-version.h
)

qt4_wrap_cpp(client_HEADERS_MOC ${client_HEADERS})

add_library(ClientEnhpos STATIC
	demoif.cpp
	testclient.cpp
	confif.cpp
	ConfigurationClient.cpp
	../src/genivi-version.cpp
	signalhelper.cpp
    ${client_HEADERS_MOC}
)

add_executable(test-client
	main-client.cpp
)

target_link_libraries(test-client
    ${QT_QTDBUS_LIBRARY} # TODO fix this
    #${QT_LIBRARIES}
    QtCore QtDBus QtXml   # TODO remove this if fixed
    ClientEnhpos
)
