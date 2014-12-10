#! /bin/bash

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

API_DIR=../../api
OUTPUT_DIR=..
DOC_NAME=GENIVI_EnhancedPositionService_ComponentSpecification
OUTPUT_FILE=$DOC_NAME.pdf

INPUT_FILE1=$DOC_NAME-part1.pdf
INPUT_FILE2=$DOC_NAME-part2.pdf

#convert xml files into html files
xsltproc -o ./genivi-positioning-configuration.html           $API_DIR/introspect.xsl    $API_DIR/genivi-positioning-configuration.xml
xsltproc -o ./genivi-positioning-enhancedposition.html        $API_DIR/introspect.xsl    $API_DIR/genivi-positioning-enhancedposition.xml
xsltproc -o ./genivi-positioning-positionfeedback.html        $API_DIR/introspect.xsl    $API_DIR/genivi-positioning-positionfeedback.xml   
xsltproc -o ./genivi-positioning-constants.html               $API_DIR/constants.xsl     $API_DIR/genivi-positioning-constants.xml

#concatenate html files into a single pdf file
wkhtmltopdf ./genivi-positioning-configuration.html ./genivi-positioning-enhancedposition.html ./genivi-positioning-positionfeedback.html ./genivi-positioning-constants.html $INPUT_FILE2

#remove temporary html files
rm *.html

#concatenate pdf files
pdftk $INPUT_FILE1 $INPUT_FILE2 cat output $OUTPUT_DIR/$OUTPUT_FILE

#remove temporary pdf file
rm $INPUT_FILE2

