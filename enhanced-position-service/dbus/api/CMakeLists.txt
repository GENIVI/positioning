###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
#
# Author: Marco Residori
#
# Copyright (C) 2015, XS Embedded GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

cmake_minimum_required(VERSION 2.6)
message(STATUS "Generate stubs and proxies of enhanced-position-service")

find_program(XSLTPROC xsltproc REQUIRED)
find_program(DBUSXML2CPP dbusxx-xml2cpp REQUIRED)

set(GEN_DIR "${CMAKE_BINARY_DIR}/enhanced-position-service/dbus/api")
file(MAKE_DIRECTORY ${GEN_DIR})

message(STATUS "CMAKE_CURRENT_SOURCE_DIR=" ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE_BINARY_DIR=" "${CMAKE_BINARY_DIR}")

execute_process(
	WORKING_DIRECTORY ${GEN_DIR}
    COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-enhancedposition.xml --adaptor=enhanced-position-adaptor.h
)

execute_process(
	WORKING_DIRECTORY ${GEN_DIR}
    COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-enhancedposition.xml --proxy=enhanced-position-proxy.h
)

execute_process(
	WORKING_DIRECTORY ${GEN_DIR}
    COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-positionfeedback.xml --adaptor=position-feedback-adaptor.h
)

execute_process(
	WORKING_DIRECTORY ${GEN_DIR}
    COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-positionfeedback.xml --proxy=position-feedback-proxy.h
)

execute_process(
	WORKING_DIRECTORY ${GEN_DIR}
    COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-configuration.xml --adaptor=configuration-adaptor.h
)

execute_process(
	WORKING_DIRECTORY ${GEN_DIR}
    COMMAND dbusxx-xml2cpp ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-configuration.xml --proxy=configuration-proxy.h
)

execute_process(
	WORKING_DIRECTORY ${GEN_DIR}
    COMMAND xsltproc ${CMAKE_CURRENT_SOURCE_DIR}/enum.xsl ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-constants.xml
    OUTPUT_FILE positioning-constants.h
)

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-enhancedposition.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-positionfeedback.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-configuration.xml
        ${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-constants.xml
        DESTINATION share/dbus-1/interfaces
)
