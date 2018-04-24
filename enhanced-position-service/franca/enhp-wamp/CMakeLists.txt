###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: enhp-wamp
#
# Author: Philippe Colliot
#
# Copyright (C) 2018, PSA GROUPE
#
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################
project(enhp-wamp)
cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME_SERVICE ${PROJECT_NAME}Service)
set(PROJECT_NAME_LIBRARY ${PROJECT_NAME}-wamp)

message(STATUS ${PROJECT_NAME})

option(WAMP_GENERATION
       "Generate Wamp files" ON)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-Wall -O0 -std=c++0x -D_GLIBCXX_USE_NANOSLEEP -pthread")

set(FRANCA_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../api/franca")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

# Packages
find_package(CommonAPI REQUIRED CONFIG NO_CMAKE_PACKAGE_REGISTRY)
find_package(CommonAPI-WAMP REQUIRED CONFIG NO_CMAKE_PACKAGE_REGISTRY)

message(STATUS "CommonAPI_CONSIDERED_CONFIGS: ${CommonAPI_CONSIDERED_CONFIGS}")
message(STATUS "COMMONAPI_INCLUDE_DIRS: ${COMMONAPI_INCLUDE_DIRS}")
message(STATUS "CommonAPI-WAMP_CONSIDERED_CONFIGS: ${CommonAPI-WAMP_CONSIDERED_CONFIGS}")
message(STATUS "COMMONAPI_WAMP_INCLUDE_DIRS: ${COMMONAPI_WAMP_INCLUDE_DIRS}")

include(FindPkgConfig)

# Add the Franca files (that will generate the CommonAPI stuff)
set(COMMONAPI_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/src-gen")
add_subdirectory(${FRANCA_DIR}/enhancedpositionservice "${CMAKE_CURRENT_BINARY_DIR}/franca")

# Path to the generated files
set(API_VERSION_ENHP v5)
set(PRJ_SRC_GEN_ENHP_PATH ${COMMONAPI_GEN_DIR}/${API_VERSION_ENHP}/org/genivi/enhancedpositionservice)

# Source Files
FILE(GLOB PRJ_WAMP_LIB_SRCS
    ${PRJ_SRC_GEN_ENHP_PATH}/*Wamp*.cpp
    )
FILE(GLOB PRJ_STUB_GEN_SRCS
    ${PRJ_SRC_GEN_ENHP_PATH}/*StubDefault.cpp
    )
FILE(GLOB PRJ_STUB_IMPL_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/*Stub*.cpp
    )
FILE(GLOB PRJ_CURRENT_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/*Service.cpp
    )

set(PRJ_SERVICE_SRCS ${PRJ_CURRENT_SRCS} ${PRJ_STUB_GEN_SRCS} ${PRJ_STUB_IMPL_SRCS})

include_directories(
    ${COMMONAPI_GEN_DIR}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_WAMP_INCLUDE_DIRS}
    ${WAMP_INCLUDE_DIRS}
)

link_directories(
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_WAMP_LIBDIR}
    ${WAMP_LIBDIR}
)

set(LINK_LIBRARIES -Wl,--no-as-needed -Wl,--as-needed CommonAPI)

# Build service
add_executable(${PROJECT_NAME_SERVICE} ${PRJ_SERVICE_SRCS})
target_link_libraries(${PROJECT_NAME_SERVICE} ${LINK_LIBRARIES})

# Build gluecode.so
add_library (${PROJECT_NAME_LIBRARY} SHARED ${PRJ_WAMP_LIB_SRCS})
target_link_libraries(${PROJECT_NAME_LIBRARY} CommonAPI-WAMP ${WAMP_LIBRARIES})

install(TARGETS ${PROJECT_NAME_SERVICE} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
install(TARGETS ${PROJECT_NAME_LIBRARY} DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
