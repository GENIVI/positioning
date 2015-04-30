#!/bin/bash

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
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

TOP_DIR=../../../..

LOGS_DIR=$TOP_DIR/log-replayer/logs
LOGREPLAYER_DIR=$TOP_DIR/build/log-replayer/src
ENHPOS_DIR=$TOP_DIR/build/enhanced-position-service/franca/src

echo "Test started"

$LOGREPLAYER_DIR/log-replayer $LOGS_DIR/geneve-cologny.log > /dev/null 2>&1 &

$ENHPOS_DIR/EnhancedPositionService &

sleep 1

$ENHPOS_DIR/EnhancedPositionClient &

sleep 10

echo "Test finished"

killall EnhancedPositionClient
killall EnhancedPositionService
killall log-replayer


