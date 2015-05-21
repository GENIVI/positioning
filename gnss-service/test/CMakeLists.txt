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
#		- introduce debug flag to disable verbosity
# @licence end@
###########################################################################

message(STATUS "GNSS-SERVICE-CLIENT")
message( STATUS "WITH_DLT = ${WITH_DLT}")
message( STATUS "WITH_GPSD = ${WITH_GPSD}")
message( STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_DEBUG = ${WITH_DEBUG}")

include_directories("${PROJECT_SOURCE_DIR}/api")
include_directories("${PROJECT_SOURCE_DIR}/src")

find_package(PkgConfig)

set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/gnss-service-client.c)

add_executable(gnss-service-client ${SRCS})

if(WITH_GPSD)
    set(LIBRARIES gnss-service-use-gpsd gps)  
elseif(WITH_REPLAYER)
    set(LIBRARIES gnss-service-use-replayer) 
else()
    message(STATUS "Invalid cmake options!")
endif()

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

if(WITH_DEBUG)
    add_definitions("-DDEBUG_ENABLED=1")
endif()

target_link_libraries(gnss-service-client ${LIBRARIES})

install(TARGETS gnss-service-client DESTINATION bin)
