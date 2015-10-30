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
# @licence end@
###########################################################################

killall enhanced-position-client > /dev/null  2>&1
killall enhanced-position-service > /dev/null  2>&1 
killall log-replayer > /dev/null  2>&1  

cd ../..

echo 'Starting LogReplayer...'
build/log-replayer/src/log-replayer build/log-replayer/logs/geneve-cologny.log > /dev/null  2>&1  &

sleep 1

echo 'Starting EnhancedPositionService...'
build/enhanced-position-service/dbus/src/enhanced-position-service > /dev/null 2>&1 &

sleep 2

cd position-web-service/html
firefox ./pos.html
