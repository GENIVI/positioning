#! /bin/bash
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
  else
    echo "Error generating pdf file"
fi



