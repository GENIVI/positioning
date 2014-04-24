#!/bin/bash

doxygen Doxyfile generated/latex

pushd generated/latex
make
popd

if [ -f generated/latex/refman.pdf ];
  then 
    cp generated/latex/refman.pdf ./apidoc/EnhancedPositionServiceAPI-part1.pdf
    cd apidoc
    ./generatedoc.sh
    evince ../EnhancedPositionServiceAPI.pdf &
  else
    echo "Error generating pdf file"
fi
