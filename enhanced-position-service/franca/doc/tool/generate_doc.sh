#!/bin/sh

###########################################################################
# @licence app begin@
# SPDX-License-Identifier: MPL-2.0
#
# Component Name: EnhancedPositionService
# Author: Marco Residori
#
# Copyright (C) 2017, Mentor Graphics
# 
# License:
# This Source Code Form is subject to the terms of the
# Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
# this file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# @licence end@
###########################################################################

OUTPUT_DIR=..

OUTPUT_FILE=EnhancedPositionService_ComponentSpecification.pdf
FILE1=GENIVI_EnhancedPositionService_ComponentSpecification-part1.pdf
FILE2=GENIVI_EnhancedPositionService_ComponentSpecification-part2.pdf

doxygen Doxyfile

cd latex
make pdf
cd ..

if [ -f latex/refman.pdf ];
  then 
    #copy the file containing the doxygen documentation
    cp latex/refman.pdf $FILE2
    #concatenate pdf files
    pdftk $FILE1 $FILE2 cat output $OUTPUT_DIR/$OUTPUT_FILE
    #show output file
    evince $OUTPUT_DIR/$OUTPUT_FILE &
    rm $FILE2
    rm -rf latex
    rm -rf doxygen_sqlite3.db
  else
    echo "Error generating pdf file"
fi
