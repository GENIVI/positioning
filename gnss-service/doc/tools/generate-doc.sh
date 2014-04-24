#! /bin/bash

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: GNSSService
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

OUTPUT_DIR=..

OUTPUT_FILE=GNSSServiceAPI.pdf
FILE1=GNSSServiceAPI-part1.pdf
FILE2=GNSSServiceAPI-part2.pdf

doxygen Doxyfile generated/latex
pushd generated/latex
make
popd

if [ -f generated/latex/refman.pdf ];
  then 
    #copy the file containing the doxygen documentation
    cp generated/latex/refman.pdf $FILE2
    #concatenate pdf files
    pdftk $FILE1 $FILE2 cat output $OUTPUT_DIR/$OUTPUT_FILE
    #show output file
    evince $OUTPUT_DIR/$OUTPUT_FILE &
    rm $FILE2
    rm -rf generated
  else
    echo "Error generating pdf file"
fi



