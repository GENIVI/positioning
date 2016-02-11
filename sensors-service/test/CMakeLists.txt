###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: SensorsService
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

message(STATUS "SENSORS-SERVICE-CLIENT")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_IPHONE = ${WITH_IPHONE}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_DEBUG = ${WITH_DEBUG}")

include_directories("${PROJECT_SOURCE_DIR}/api")
include_directories("${PROJECT_SOURCE_DIR}/src")

find_package(PkgConfig)

if(WITH_IPHONE)
    set(LIBRARIES sensors-service-use-iphone)
elseif(WITH_SENSORS)
    set(LIBRARIES sensors-service-use-sensors)
    #for glibc <2.17, clock_gettime is in librt: http://linux.die.net/man/2/clo$
    #TODO: is there a nice way to detect glibc version in CMake?
    set(LIBRARIES ${LIBRARIES} rt)
elseif(WITH_REPLAYER)
    set(LIBRARIES sensors-service-use-replayer)
else()
    message(STATUS "Invalid cmake options!")
endif()

if(WITH_DLT)
    add_definitions("-DDLT_ENABLED=1")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
    find_package(DLT REQUIRED)
    include_directories( ${DLT_INCLUDE_DIRS} )
    link_directories ( ${DLT_LIBRARY_DIRS} )
    set(LIBRARIES ${LIBRARIES} ${DLT_LIBRARIES})
endif()

if(WITH_DEBUG)
    add_definitions("-DDEBUG_ENABLED=1")
endif()

set(SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sensors-service-client.c)
add_executable(sensors-service-client ${SRCS})
target_link_libraries(sensors-service-client ${LIBRARIES})
install(TARGETS sensors-service-client DESTINATION bin)



