#!/bin/bash

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: Positioning poC
# Author: Marco Residori
#
# Copyright (C) 2013, XS Embedded GmbH
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

TOP_SRC_DIR=$PWD
TOP_BIN_DIR=$PWD/build

GNSS_SERVICE_BIN_DIR=$TOP_BIN_DIR/gnss-service
SENSORS_SERVICE_BIN_DIR=$TOP_BIN_DIR/sensors-service
ENHANCED_POSITION_SERVICE_DBUS_BIN_DIR=$TOP_BIN_DIR/enhanced-position-service/dbus
ENHANCED_POSITION_SERVICE_FRANCA_BIN_DIR=$TOP_BIN_DIR/enhanced-position-service/franca
LOG_REPLAYER_BIN_DIR=$TOP_BIN_DIR/log-replayer
LOG_REPLAYER_LOGS_DIR=$TOP_BIN_DIR/log-replayer/logs

usage() {
    echo "Usage: ./run-test.sh [service]"
    echo
    echo "service:"
    echo "  gnss            Test GNSSService"
    echo "  sns             Test SensorsService"
    echo "  enhpos          Test EnhancedPositionService"
    echo "  repl            Test LogReplayer"
    echo "  kill            Kill all test applications"
    echo "  help            Print Help"
    echo
}

testGnssService()
{
    $GNSS_SERVICE_BIN_DIR/test/gnss-service-client &
    sleep 3
    echo 'Starting log replayer...'
    $LOG_REPLAYER_BIN_DIR/src/log-replayer $LOG_REPLAYER_LOGS_DIR/20100411_Geko_Regensburg_short.log > /dev/null 2>&1 &
    sleep 10
    echo 'Stopping test...'
    killall log-replayer
    killall gnss-service-client
}

testSensorsService()
{
    $SENSORS_SERVICE_BIN_DIR/test/sensors-service-client &
    sleep 3
    echo 'Starting log replayer...'
    $LOG_REPLAYER_BIN_DIR/src/log-replayer $LOG_REPLAYER_LOGS_DIR/20100411_Geko_Regensburg_short.log > /dev/null 2>&1 &
    sleep 10
    echo 'Stopping test...'
    killall log-replayer
    killall sensors-service-client
}

testEnhancedPositionService()
{
    $ENHANCED_POSITION_SERVICE_DBUS_BIN_DIR/src/enhanced-position-service > /dev/null 2>&1 &
    sleep 3
    $ENHANCED_POSITION_SERVICE_DBUS_BIN_DIR/test/enhanced-position-client &
    sleep 3
    echo 'Starting log replayer...'
    $LOG_REPLAYER_BIN_DIR/src/log-replayer $LOG_REPLAYER_LOGS_DIR/geneve-cologny.log > /dev/null  2>&1  &
    sleep 20
    echo 'Stopping test...'
    killall enhanced-position-client 
    killall enhanced-position-service
    killall log-replayer
}

testEnhancedPositionService2()
{
    echo 'Starting enhanced position service...'
    $LOG_REPLAYER_BIN_DIR/src/log-replayer $LOG_REPLAYER_LOGS_DIR/geneve-cologny.log > /dev/null  2>&1  &
    echo 'Starting log replayer...'
    $ENHANCED_POSITION_SERVICE_DBUS_BIN_DIR/src/enhanced-position-service &
    sleep 1000
    echo 'Stopping test...'
    killall enhanced-position-service
    killall log-replayer
}

testLogReplayer()
{
    echo 'Starting log replayer...'
    $LOG_REPLAYER_BIN_DIR/src/log-replayer $LOG_REPLAYER_LOGS_DIR/geneve-cologny.log  > /dev/null 2>&1 &
    #$LOG_REPLAYER_LOGS_DIR/src/log-replayer $LOG_REPLAYER_LOGS_DIR/20100411_Geko_Regensburg_short.log > /dev/null 2>&1 &
    sleep 1
    echo 'Starting test application...'
    $LOG_REPLAYER_BIN_DIR/test/test-log-replayer 9930 &
    sleep 10
    echo 'Stopping test...'
    killall log-replayer
    killall test-log-replayer
}

killAllTests()
{
    killall gnss-service-client
    killall sensors-service-client
    killall enhanced-position-client 
    killall enhanced-position-service
    killall test-log-replayer
    killall log-replayer
}

if [ $# -eq 1 ]; then
   if [ $1 = help ]; then
       usage
   elif [ $1 = gnss ]; then
       testGnssService
    elif [ $1 = sns ]; then
       testSensorsService
    elif [ $1 = enhpos ]; then
       testEnhancedPositionService
    elif [ $1 = enhpos2 ]; then
       testEnhancedPositionService2
    elif [ $1 = repl ]; then
       testLogReplayer
    elif [ $1 = kill ]; then
       killAllTests
    else
       usage
    fi
else
    usage
fi
