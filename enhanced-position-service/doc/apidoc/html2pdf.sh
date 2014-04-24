#! /bin/bash
./xml2html.sh genivi-positioning-enhancedposition
./xml2html.sh genivi-positioning-configuration
./xml2html.sh genivi-positioning-positionfeedback
xsltproc -o genivi-positioning-constants.xml ../../api/constants.xsl ../../api/genivi-positioning-constants.xml

wkhtmltopdf ./genivi-positioning-enhancedposition.html ./genivi-positioning-configuration.html ./genivi-positioning-positionfeedback.html genivi-positioning-constants.xml EnhancedPositionServiceAPI-part2.pdf

rm *.html
