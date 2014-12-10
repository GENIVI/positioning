###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: enhanced-position-service
#
# Author: Philippe Colliot
#
# Copyright (C) 2014, PCA Peugeot Citroën
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################
project(enhanced-position-service)
cmake_minimum_required(VERSION 2.8)

message(STATUS "generation of DBus files for enhanced-position-service")

find_program(XSLTPROC xsltproc REQUIRED)
find_program(DBUSXML2CPP dbusxx-xml2cpp REQUIRED)

set(cmake_create_dir cmake -E make_directory)

set(API_DIR ${CMAKE_CURRENT_SOURCE_DIR})

execute_process(
	WORKING_DIRECTORY ${API_DIR}
        COMMAND ${cmake_create_dir} include
)

set(TARGET_GENERATED_API_DIR ${API_DIR}/include)

file (GLOB XML_TO_DBUS_PROXY 
	${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-configuration.xml
	${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-enhancedposition.xml
	${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-positionfeedback.xml
)
foreach (RAW_FILE ${XML_TO_DBUS_PROXY})
	get_filename_component(FILE ${RAW_FILE} NAME_WE)
	if(NOT EXISTS ${TARGET_GENERATED_API_DIR}/${FILE}_proxy.h)
		execute_process(
			COMMAND ${DBUSXML2CPP} ${RAW_FILE} --proxy=${TARGET_GENERATED_API_DIR}/${FILE}_proxy.h  
		)
	else()
		if(${RAW_FILE} IS_NEWER_THAN ${TARGET_GENERATED_API_DIR}/${FILE}_proxy.h)
			execute_process(
				COMMAND ${DBUSXML2CPP} ${RAW_FILE} --proxy=${TARGET_GENERATED_API_DIR}/${FILE}_proxy.h  
			)
		endif()
	endif()
endforeach()

file (GLOB XML_TO_DBUS_ADAPTOR
	${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-configuration.xml
	${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-enhancedposition.xml
	${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-positionfeedback.xml
)
foreach (RAW_FILE ${XML_TO_DBUS_ADAPTOR})
	get_filename_component(FILE ${RAW_FILE} NAME_WE)
	if(NOT EXISTS ${TARGET_GENERATED_API_DIR}/${FILE}_adaptor.h)
		execute_process(
			COMMAND ${DBUSXML2CPP} ${RAW_FILE} --adaptor=${TARGET_GENERATED_API_DIR}/${FILE}_adaptor.h  
		)
	else()
		if(${RAW_FILE} IS_NEWER_THAN ${TARGET_GENERATED_API_DIR}/${FILE}_adaptor.h)
			execute_process(
				COMMAND ${DBUSXML2CPP} ${RAW_FILE} --adaptor=${TARGET_GENERATED_API_DIR}/${FILE}_adaptor.h  
			)
		endif()
	endif()
endforeach()

file (GLOB XML_TO_DBUS_CONSTANT
	${CMAKE_CURRENT_SOURCE_DIR}/genivi-positioning-constants.xml
)
foreach (RAW_FILE ${XML_TO_DBUS_CONSTANT})
	get_filename_component(FILE ${RAW_FILE} NAME_WE)
	if(NOT EXISTS ${TARGET_GENERATED_API_DIR}/${FILE}.h)
		execute_process(
			OUTPUT_FILE ${TARGET_GENERATED_API_DIR}/${FILE}.h
			COMMAND ${XSLTPROC} ${CMAKE_CURRENT_SOURCE_DIR}/enum.xsl ${RAW_FILE}        
		)
	else()
		if(${RAW_FILE} IS_NEWER_THAN ${TARGET_GENERATED_API_DIR}/${FILE}.h)
			execute_process(
				OUTPUT_FILE ${TARGET_GENERATED_API_DIR}/${FILE}.h
				COMMAND ${XSLTPROC} ${CMAKE_CURRENT_SOURCE_DIR}/enum.xsl ${RAW_FILE}        
			)
		endif()
	endif()
endforeach()
