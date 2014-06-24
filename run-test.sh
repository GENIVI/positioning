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
    build/gnss-service/test/gnss-service-client &
    sleep 3
    echo 'starting log replayer...'
    build/log-replayer/src/log-replayer log-replayer/logs/20100411_Geko_Regensburg_short.log > /dev/null 2>&1 &
    sleep 10
    echo 'stopping test...'
    killall log-replayer
    killall gnss-service-client
}

testSensorsService()
{
    build/sensors-service/test/sensors-service-client &
    sleep 3
    echo 'starting log replayer...'
    build/log-replayer/src/log-replayer log-replayer/logs/20100411_Geko_Regensburg_short.log > /dev/null 2>&1 &
    sleep 10
    echo 'stopping test...'
    killall log-replayer
    killall sensors-service-client
}

testEnhancedPositionService()
{
    build/enhanced-position-service/src/enhanced-position-service > /dev/null 2>&1 &
    sleep 3
    build/enhanced-position-service/test/enhanced-position-client &
    sleep 3
    echo 'starting log replayer...'
    build/log-replayer/src/log-replayer log-replayer/logs/geneve-cologny.log > /dev/null  2>&1  &
    sleep 10
    echo 'stopping test...'
    killall enhanced-position-client 
    killall enhanced-position-service
    killall log-replayer
}

testLogReplayer()
{
    build/log-replayer/test/test-log-replayer 9930 &
    sleep 3
    echo 'starting log replayer...'
    build/log-replayer/src/log-replayer log-replayer/logs/geneve-cologny.log  > /dev/null 2>&1
    #build/log-replayer/src/log-replayer log-replayer/logs/20100411_Geko_Regensburg_short.log > /dev/null 2>&1 &
    sleep 10
    echo 'stopping test...'
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
