#!/bin/bash
#
# execute from position-daemon direcotry
#

#introspection="../position-api/EnhancedPosition/EnhancedPosition.xml"

# minimal set (hand edited)
introspection="genivi-positioning-enhancedposition-qt.xml"
interface="org.genivi.positioning.EnhancedPosition"

#####
echo
echo "precheck with xmllint"
xmllint $introspection

if [ $? != 0 ]; then
	echo "xmllint failed ... check your xml"
	exit 1
fi

mkdir -p client
mkdir -p server
#####
echo
echo "generate client"
cd client
pwd
qdbusxml2cpp -v -c DemoIf -p demoif.h:demoif.cpp \
    -i genivi-dbus-types.h \
    ../$introspection $interface

cd ..

#####
echo
echo "generate server"
cd server
pwd
qdbusxml2cpp -v -c DemoIfAdaptor -a demoifadaptor.h:demoifadaptor.cpp \
    -i genivi-dbus-types.h \
    -i serverimpl.h -l ServerImpl \
    ../$introspection $interface

cd ..
#####

echo
echo "done"
