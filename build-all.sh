#!/bin/bash

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: Positioning PoC
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
#		- introduce debug flag to enable verbosity
#		- generate the API of enhanced-position-service into the API folder
# @licence end@
###########################################################################

#--------------------------------------------------------------------------
# Compiler Flags
#--------------------------------------------------------------------------
# modify the following flags as needed:
GNSS_SERVICE_FLAGS='-DWITH_DLT=OFF -DWITH_GPSD=OFF -DWITH_REPLAYER=ON -DWITH_TESTS=ON -DWITH_DEBUG=ON'
SENSORS_SERVICE_FLAGS='-DWITH_DLT=OFF -DWITH_REPLAYER=ON -DWITH_IPHONE=OFF -DWITH_TESTS=ON -DWITH_DEBUG=ON'
ENHANCED_POSITION_SERVICE_FLAGS='-DWITH_DLT=OFF -DWITH_GPSD=OFF -DWITH_REPLAYER=ON -DWITH_IPHONE=OFF -DWITH_TESTS=ON -DWITH_DEBUG=ON'
LOG_REPLAYER_FLAGS='-DWITH_DLT=OFF -DWITH_TESTS=ON -DWITH_DEBUG=ON'
#--------------------------------------------------------------------------

TOP_SRC_DIR=$PWD
TOP_BIN_DIR=$PWD/build

GNSS_SERVICE_SRC_DIR=$TOP_SRC_DIR/gnss-service
SENSORS_SERVICE_SRC_DIR=$TOP_SRC_DIR/sensors-service
ENHANCED_POSITION_SERVICE_SRC_DIR=$TOP_SRC_DIR/enhanced-position-service
ENHANCED_POSITION_SERVICE_API_DIR=$ENHANCED_POSITION_SERVICE_SRC_DIR/api
LOG_REPLAYER_SRC_DIR=$TOP_SRC_DIR/log-replayer

GNSS_SERVICE_BIN_DIR=$TOP_BIN_DIR/gnss-service
SENSORS_SERVICE_BIN_DIR=$TOP_BIN_DIR/sensors-service
ENHANCED_POSITION_SERVICE_BIN_DIR=$TOP_BIN_DIR/enhanced-position-service
LOG_REPLAYER_BIN_DIR=$TOP_BIN_DIR/log-replayer

usage() {
    echo "Usage: ./build-all.sh Build all services"
    echo "   or: ./build-all.sh [command] [service]"
    echo
    echo "command:"
    echo "  make            Build"
    echo "  clean           Clean"
    echo "  help            Print Help"
    echo
    echo "service:"
    echo "  gnss            Test GNSSService"
    echo "  sns             Test SensorsService"
    echo "  enhpos          Test EnhancedPositionService"
    echo "  repl            Test LogReplayer"
    echo
}

buildGnssService() {
    echo ''
    echo 'Building GNSSService ->' $GNSS_SERVICE_SRC_DIR
    mkdir -p $GNSS_SERVICE_BIN_DIR
    cd $GNSS_SERVICE_BIN_DIR 
    cmake $GNSS_SERVICE_FLAGS $GNSS_SERVICE_SRC_DIR && make 
}

buildSensorsService() {
    echo ''
    echo 'Building SensorsService ->' $SENSORS_SERVICE_SRC_DIR
    mkdir -p $SENSORS_SERVICE_BIN_DIR
    cd $SENSORS_SERVICE_BIN_DIR 
    cmake $SENSORS_SERVICE_FLAGS $SENSORS_SERVICE_SRC_DIR && make
}

buildEnhancedPositionService() {
    echo ''
    echo 'Building EnhancedPositionService ->' $SENSORS_SERVICE_SRC_DIR
    mkdir -p $ENHANCED_POSITION_SERVICE_BIN_DIR
    cd $ENHANCED_POSITION_SERVICE_BIN_DIR 
    cmake $ENHANCED_POSITION_SERVICE_FLAGS $ENHANCED_POSITION_SERVICE_SRC_DIR && make
}

buildLogReplayer() {
    echo ''
    echo 'Building LogReplayer ->' $LOG_REPLAYER_SRC_DIR
    mkdir -p $LOG_REPLAYER_BIN_DIR
    cd $LOG_REPLAYER_BIN_DIR 
    cmake $LOG_REPLAYER_FLAGS $LOG_REPLAYER_SRC_DIR && make 
}

buildAll() {
    buildGnssService
    buildSensorsService
    buildEnhancedPositionService
    buildLogReplayer 
}

cleanAll() {
    rm -rf $TOP_BIN_DIR
}

if [ $# -ge 1 ]; then
    if [ $1 = help ]; then
        usage
    elif [ $1 = make ]; then
        if [ $# -eq 2 ]; then
            if [ "$2" = "gnss" ]; then
                buildGnssService
            elif [ "$2" = "sns" ]; then
                buildSensorsService
            elif [ "$2" = "enhpos" ]; then
                buildEnhancedPositionService
            elif [ "$2" = "repl" ]; then
                buildLogReplayer
            fi
        else
            buildAll
        fi   
    elif [ $1 = clean ]; then
        if [ $# -eq 2 ]; then
            if [ "$2" = "gnss" ]; then
                rm -rf $GNSS_SERVICE_BIN_DIR
            elif [ "$2" = "sns" ]; then
                rm -rf $SENSORS_SERVICE_BIN_DIR 
            elif [ "$2" = "enhpos" ]; then
                rm -rf $ENHANCED_POSITION_SERVICE_BIN_DIR
            elif [ "$2" = "repl" ]; then
                rm -rf $LOG_REPLAYER_BIN_DIR
            fi
        else
            rm -rf $TOP_BIN_DIR
        fi
    else
        usage
    fi
elif [ $# -eq 0 ]; then
    #cleanAll
    buildAll
else
    usage
fi


