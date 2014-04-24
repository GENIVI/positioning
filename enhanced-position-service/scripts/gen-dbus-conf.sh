#!/bin/bash
#
# execute from position-daemon direcotry
#

#introspection="../position-api/EnhancedPosition/EnhancedPosition.xml"

# minimal set (hand edited)
introspection="genivi-positioning-configuration-qt.xml"
interface="org.genivi.positioning.Configuration"

#####
echo
echo "precheck with xmllint"
xmllint $introspection

if [ $? != 0 ]; then
	echo "xmllint failed ... check your xml"
	exit 1
fi

mkdir -p server
mkdir -p client

#####
echo
echo "generate client"
cd client
pwd
qdbusxml2cpp -v -c ConfigurationIf -p confif.h:confif.cpp \
    -i genivi-dbus-types.h \
    ../$introspection $interface

cd ..

#####
echo
echo "generate server"
cd server
pwd
qdbusxml2cpp -v -c ConfigurationIfAdaptor -a confifadaptor.h:confifadaptor.cpp \
    -i genivi-dbus-types.h \
    -i ConfigurationImpl.h -l ConfigurationImpl \
    ../$introspection $interface

cd ..
#####

echo
echo "done"
