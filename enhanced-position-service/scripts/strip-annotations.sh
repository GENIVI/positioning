#!/bin/bash

in_file=genivi-positioning-enhancedposition-qt.xml
out_file=../api/genivi-positioning-enhancedposition.xml

# use all lines but the <annotation .../> tags.
grep -v "annotation" $in_file > $out_file

