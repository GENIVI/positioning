#!/bin/bash

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: PositionWebService
#
# Author: Marco Residori
#
# Copyright (C) 2013, XS Embedded GmbH
#
# License:
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# Note: This script is an adaptation of a script used in GENIVI project web-api-vehicle
# Please see http://git.projects.genivi.org/web-api-vehicle.git
#
# @licence end@
###########################################################################

usage() {
	echo "Usage: ./build-all.sh Build all"
	echo "   or: ./build-all.sh [mode]"
	echo
	echo "Mode:"
	echo "  make          Build all projects"
	echo "  install       Install all projects"
	echo "  clean         Clean all projects"
	echo "  distclean     Clean all projects"
	echo "  new           Clean and make all projects"
	echo "  -h or --help  Print help (this message)"
	echo
}

if [ $# -eq 1 ]; then
	if [ $1 = -h ]; then
		usage
	elif [ $1 = --help ]; then
		usage
	elif [ $1 = make ]; then
		./build-plugin.sh prepmake
		./build-plugin.sh make
	elif [ $1 = install ]; then
		./build-plugin.sh install
	elif [ $1 = clean ]; then
		./build-plugin.sh clean
	elif [ $1 = distclean ]; then
		./build-plugin.sh distclean
	elif [ $1 = new ]; then
		./build-plugin.sh new
	else
		usage
	fi
elif [ $# -eq 0 ]; then
	./build-plugin.sh prepmake 
	./build-plugin.sh make
	./build-plugin.sh install
else
	usage
fi
