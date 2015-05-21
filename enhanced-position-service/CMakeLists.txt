###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: Positioning Top-level Makefile
#
# Author: Jonathan Maw
#
# Copyright (C) 2015, Codethink Ltd
#
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Update (2015/05/11) : Jonathan Maw <jonathan.maw@codethink.co.uk>
#               - First write
# @licence end@
###########################################################################
cmake_minimum_required(VERSION 2.6.0)

if(WITH_DBUS_INTERFACE)
	add_subdirectory(dbus)
endif(WITH_DBUS_INTERFACE)

if(WITH_FRANCA_INTERFACE)
	add_subdirectory(franca)
endif(WITH_FRANCA_INTERFACE)
