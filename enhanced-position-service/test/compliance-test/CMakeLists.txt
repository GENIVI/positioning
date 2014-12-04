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

message(STATUS "ENHANCED-POSITION-SERVICE-COMPLIANCE-TEST")

INCLUDE(UsePkgConfig)

include_directories("${PROJECT_SOURCE_DIR}/../common")

find_package(PkgConfig)
pkg_check_modules(DBUS REQUIRED dbus-1)

link_directories ( ${DBUS_LIBDIR} ${DBUS_LIBRARY_DIRS}  )
include_directories ( ${DBUS_INCLUDEDIR} ${DBUS_INCLUDE_DIRS} )

set(SRCS ${CMAKE_SOURCE_DIR}/test/compliance-test/enhanced-position-service-compliance-test.c)

add_executable(enhanced-position-service-compliance-test ${SRCS})

set(LIBRARIES ${DBUS_LIBRARIES} )  

target_link_libraries(enhanced-position-service-compliance-test ${LIBRARIES})

