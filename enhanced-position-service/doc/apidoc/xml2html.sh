#! /bin/bash
xsltproc -o $1.html ../../api/introspect.xsl ../../api/$1.xml
