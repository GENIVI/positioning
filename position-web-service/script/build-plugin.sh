#!/bin/bash

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: PositionWebService
#
# Author: Marco Residori
#
# Copyright (C) 2014, XS Embedded GmbH
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

# SRC_DIR: source directory
SRC_DIR=../src/plugin
# PLUGIN_ID: plugin identifier
PLUGIN_ID=PositionWebService
# PLUGIN_DIR: the directory that the plugin will be installed
PLUGIN_DIR=~/.mozilla/plugins
# FB_VER: firebreath version
FB_VER=1.7

usage() { 
	echo "Usage: ./build-plugin.sh [mode]"
	echo
	echo "Mode:"
	echo "  prepmake 	Clone firebreath from git if it has not been downloaded yet"
	echo "		Run prepmake.sh in the firebreath directory to prepare building the plugin project"
	echo "  make 		Build the plugin project"
	echo "  install 	Install the binary file (np$PLUGIN_ID.so) to the browser's plugin directory"
	echo " 		($PLUGIN_DIR)"
	echo "  clean 	Clean the plugin project"
	echo "  distclean 	Remove the fbbuild directory and the binary file; after distclean,"
	echo " 		prepmake has to be run again"
	echo "  new 		Clean and make the plugin project"
	echo "  -h or --help 	Print help (this message)"
	echo
	exit 1
}
error_prepmake() {
	echo "Run prepmake first"
	echo "Usage: ./build-plugin.sh prepmake"
	echo
	exit 1
}
error_make() {
	echo "Run make first"
	echo "Usage: ./build-plugin.sh make"
	echo
	exit 1
}
check_prepmake() {
	if [ ! -d "$SRC_DIR/firebreath-dev" ]; then
		error_prepmake
	elif [ ! -d "$SRC_DIR/fbbuild" ]; then
		error_prepmake
	fi
}
check_make() {
	if [ ! -f "$SRC_DIR/fbbuild/bin/$PLUGIN_ID/np$PLUGIN_ID.so" ]; then
		error_make
	fi
}

if [ $# -ne 1 ]; then
	usage
else
	if [ $1 = -h ]; then
		usage
	elif [ $1 = --help ]; then
		usage
	elif [ $1 = prepmake ]; then
		if [ ! -d "$SRC_DIR/firebreath-dev" ]; then
			git clone https://github.com/firebreath/FireBreath.git -b firebreath-$FB_VER $SRC_DIR/firebreath-dev
		fi
		$SRC_DIR/firebreath-dev/prepmake.sh $SRC_DIR/fbprojects $SRC_DIR/fbbuild
	elif [ $1 = make ]; then
		check_prepmake
		cd $SRC_DIR/fbbuild/projects/$PLUGIN_ID
		make
	elif [ $1 = install ]; then
		check_make
		if [ ! -d "$PLUGIN_DIR" ]; then
			mkdir $PLUGIN_DIR
		fi
		cp -rvf $SRC_DIR/fbbuild/bin/$PLUGIN_ID/np$PLUGIN_ID.so $PLUGIN_DIR 
	elif [ $1 = clean ]; then
		check_prepmake
		cd $SRC_DIR/fbbuild/projects/$PLUGIN_ID
		make clean
	elif [ $1 = distclean ]; then
		rm -rvf $SRC_DIR/fbbuild
		rm -rvf $PLUGIN_DIR/np$PLUGIN_ID.so
	elif [ $1 = new ]; then
		check_prepmake
		cd $SRC_DIR/fbbuild/projects/$PLUGIN_ID
		make clean
		make
	else
		usage
	fi
fi
