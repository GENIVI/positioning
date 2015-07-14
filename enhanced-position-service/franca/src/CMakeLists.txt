# Copyright (C) 2014 BMW Group
# Author: Marco Residori (marco_residori@mentor.com)
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

cmake_minimum_required(VERSION 2.8)
find_package(PkgConfig REQUIRED)

# Project settings
set(PROJECT_NAME EnhancedPosition)

project(${PROJECT_NAME})
set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "-Wall -std=c++0x")
set(CMAKE_BUILD_TYPE Debug)

message(STATUS)
message(STATUS "---------------------------------------------------------")
message(STATUS "ENHANCED-POSITION-SERVICE-COMMON-API")
message(STATUS "WITH_DLT = ${WITH_DLT}")
message(STATUS "WITH_GPSD = ${WITH_GPSD}")
message(STATUS "WITH_REPLAYER = ${WITH_REPLAYER}")
message(STATUS "WITH_TESTS = ${WITH_TESTS}")
message(STATUS "WITH_DEBUG = ${WITH_DEBUG}")

find_package(PkgConfig REQUIRED)
find_package(CommonAPI 3.1.2 REQUIRED)
find_package(CommonAPI-DBus 3.1.2 REQUIRED)

# CommonAPI
include(FindPkgConfig)
pkg_check_modules (DBUS "dbus-1 >= 1.4")
pkg_check_modules(COMMONAPI "CommonAPI >= 3.1")
pkg_check_modules (COMMONAPI_DBUS "CommonAPI-DBus >= 3.1")

# Source Files
set(PRJ_SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR})
set(PRJ_SRC_GEN_ROOT ${CMAKE_BINARY_DIR}/enhanced-position-service/franca/api)
set(PRJ_SRC_GEN_PATH ${PRJ_SRC_GEN_ROOT}/org/genivi/EnhancedPositionService)

set(PRJ_NAME_CLIENT ${PROJECT_NAME}Client)
set(PRJ_NAME_SERVICE ${PROJECT_NAME}Service)

FILE(GLOB PRJ_PROXY_GEN_SRCS ${PRJ_SRC_GEN_PATH}/*Proxy.cpp ${PRJ_SRC_GEN_PATH}/*Types.cpp)
FILE(GLOB PRJ_STUB_GEN_SRCS ${PRJ_SRC_GEN_PATH}/*Stub*.cpp ${PRJ_SRC_GEN_PATH}/*Types.cpp)
FILE(GLOB PRJ_STUB_IMPL_SRCS ${PRJ_SRC_PATH}/*Stub*.cpp)

set(PRJ_CLIENT_SRCS ${PRJ_SRC_PATH}/${PRJ_NAME_CLIENT}.cpp ${PRJ_PROXY_GEN_SRCS})
set(PRJ_SERVICE_SRCS ${PRJ_SRC_PATH}/${PRJ_NAME_SERVICE}.cpp ${PRJ_STUB_GEN_SRCS} ${PRJ_STUB_IMPL_SRCS})

message(STATUS "PRJ_SRC_GEN_ROOT = " ${PRJ_SRC_GEN_ROOT})
message(STATUS "PRJ_SRC_PATH = " ${PRJ_SRC_PATH})
message(STATUS "PRJ_SRC_GEN_PATH = " ${PRJ_SRC_GEN_PATH})
message(STATUS "COMMONAPI_INCLUDE_DIRS = " ${COMMONAPI_INCLUDE_DIRS})
message(STATUS "COMMONAPI_DBUS_INCLUDE_DIRS = " ${COMMONAPI_DBUS_INCLUDE_DIRS})
message(STATUS "PRJ_CLIENT_SRCS = " ${PRJ_CLIENT_SRCS})
message(STATUS "PRJ_SERVICE_SRCS = " ${PRJ_SERVICE_SRCS})

include_directories(
    ${PRJ_SRC_GEN_ROOT}
    ${DBUS_INCLUDE_DIRS}
    ${COMMONAPI_INCLUDE_DIRS}
    ${COMMONAPI_DBUS_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${gnss-service_INCLUDE_DIRS}
    ${sensors-service_INCLUDE_DIRS}
)

link_directories(
    ${DBUS_LIBDIR}
    ${gnss-service_LIBRARY_DIRS}
    ${sensors-service_LIBRARY_DIRS}
    ${COMMONAPI_LIBDIR}
    ${COMMONAPI_DBUS_LIBDIR}
)

set(LIBRARIES 
    ${DBUS_LIBRARIES}
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

if(WITH_DEBUG)
    add_definitions("-DDEBUG_ENABLED=1")
endif()

# Build Client
add_executable(${PRJ_NAME_CLIENT} ${PRJ_CLIENT_SRCS})
target_link_libraries(${PRJ_NAME_CLIENT} ${LIBRARIES} CommonAPI CommonAPI-DBus)

# Build service
add_executable(${PRJ_NAME_SERVICE} ${PRJ_SERVICE_SRCS})
target_link_libraries(${PRJ_NAME_SERVICE} ${LIBRARIES} CommonAPI CommonAPI-DBus)

message(STATUS "CMAKE_CURRENT_BINARY_DIR!!!!!!!!!! = " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "PRJ_STUB_IMPL_SRCS!!!!!!!!!! = " ${PRJ_STUB_IMPL_SRCS})
message(STATUS "PRJ_STUB_GEN_SRCS!!!!!!!!!! = " ${PRJ_STUB_GEN_SRCS})
message(STATUS "COMMONAPI_INCLUDE_DIRS!!!!!!!!!! = " ${COMMONAPI_INCLUDE_DIRS})
message(STATUS "COMMONAPI_LIBDIR!!!!!!!!!! = " ${COMMONAPI_LIBDIR})


