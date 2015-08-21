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

TOP_DIR=../../..

BIN_DIR=$TOP_DIR/build/log-replayer/src
LOGS_DIR=$TOP_DIR/log-replayer/logs
BIN=log-replayer
LOG=geneve-cologny.log

CMD="$BIN_DIR/$BIN $LOGS_DIR/$LOG > /dev/null 2>&1"

usage() {
    echo "Usage: "
    echo "  run-logreplayer.sh [option]"
    echo "  -d           daemonize"
    echo "  -k           kill"
    echo "  -h           help"
    echo
}

if [ $# -ge 1 ]; then
    if [ $1 = "-d" ]; then
        echo "Starting log-replayer..."
        eval "$CMD" &
    elif [ $1 = "-k" ]; then
        killall log-replayer  
    elif [ $1 = "-h" ]; then
        usage  
    fi 
else
    echo "Starting log-replayer..."
    eval "$CMD"
fi



