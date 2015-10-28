###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: LogReplayer
#
# Author: Marco Residori
#
# Copyright (C) 2014, XS Embedded GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

set(DLT_INCLUDE_DIRS /usr/include/dlt)
set(DLT_LIBRARIES dlt)
set(DLT_LIBRARY_DIRS /usr/lib)

find_package(PkgConfig)
pkg_check_modules(DLT REQUIRED automotive-dlt)

if(${DLT_FOUND})
    #message(STATUS "found and use automotive-dlt: version ${DLT_VERSION}")
else()
    message("missing DLT - check with 'pkg-config automotive-dlt --cflags-only-I'")
endif()
