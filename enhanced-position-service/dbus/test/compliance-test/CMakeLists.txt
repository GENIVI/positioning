###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: GNSSService
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
#		- generate the API of enhanced-position-service into the API folder
# @licence end@
###########################################################################

cmake_minimum_required(VERSION 2.6.0)
message(STATUS "ENHANCED-POSITION-SERVICE-COMPLIANCE-TEST")

find_package(PkgConfig)
pkg_check_modules(DBUS REQUIRED dbus-1)

include_directories( ${DBUS_INCLUDEDIR} ${DBUS_INCLUDE_DIRS} )

link_directories ( ${DBUS_LIBDIR} ${DBUS_LIBRARY_DIRS}  )

set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/enhanced-position-service-compliance-test.c)

add_executable(enhanced-position-service-compliance-test ${SRCS})

set(LIBRARIES ${DBUS_LIBRARIES} )  

target_link_libraries(enhanced-position-service-compliance-test ${LIBRARIES})

