/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup EnhancedPositionService
*
* \copyright Copyright (C) BMW Car IT GmbH 2011, 2012
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/


#include "genivi-version.h"

#define DUMMY_VERSION_MAJOR (0)
#define DUMMY_VERSION_MINOR (0)
#define DUMMY_VERSION_MICRO (0)
#define DUMMY_VERSION_DATE "2011-07-25"

GeniviVersion::GeniviVersion()
 : _major(DUMMY_VERSION_MAJOR),
   _minor(DUMMY_VERSION_MINOR),
   _micro(DUMMY_VERSION_MICRO)
{
    // qDebug() << "ktor: " << _major << _minor << _micro;
}

GeniviVersion::GeniviVersion(uint major, uint minor, uint micro)
: _major(major),
  _minor(minor),
  _micro(micro)
{
    // qDebug() << "ktor: ..." << _major << _minor << _micro;
}

GeniviVersion::~GeniviVersion()
{
    // TODO
}

void GeniviVersion::setVersion(uint major, uint minor, uint micro)
{
    _major = major;
    _minor = minor;
    _micro = micro;
}

uint GeniviVersion::getMajorVersion() const
{
    return _major;
}

uint GeniviVersion::getMinorVersion() const
{
    return _minor;
}

uint GeniviVersion::getMicroVersion() const
{
    return _micro;
}

// Marshall the MyStructure data into a D-BUS argument
QDBusArgument &operator<<(QDBusArgument &argument, const GeniviVersion &mystruct)
{
    argument.beginStructure();
    argument << mystruct.getMajorVersion()
            << mystruct.getMinorVersion()
            << mystruct.getMicroVersion();
            //<< "unknown";
    argument.endStructure();
    return argument;
}

// Retrieve the MyStructure data from the D-BUS argument
const QDBusArgument &operator>>(const QDBusArgument &argument, GeniviVersion &mystruct)
{
    uint major;
    uint minor;
    uint micro;
    //QString date;

    argument.beginStructure();
    argument >> major >> minor >> micro; /* date */
    argument.endStructure();

    mystruct.setVersion(major, minor, micro);

    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const MapIntVariant &myMap)
{
    QMap<int, QVariant>::const_iterator i;
    argument.beginMap( QVariant::Int, qMetaTypeId<QDBusVariant>() );
    for (i = myMap.constBegin(); i != myMap.constEnd(); ++i) {
        argument.beginMapEntry();
        argument << i.key() << QDBusVariant(i.value());
        argument.endMapEntry();
    }
    argument.endMap();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument, MapIntVariant &myMap)
{
    argument.beginMap();
    myMap.clear();
    while ( !argument.atEnd() ) {
        // argument
        int key;
        QVariant value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        myMap.insert(key, value);
    }
    argument.endMap();
    return argument;
}

QDBusArgument &operator<<(QDBusArgument &argument, const MapUShortVariant &myMap)
{
    QMap<ushort, QVariant>::const_iterator i;
    argument.beginMap(qMetaTypeId<ushort>(), qMetaTypeId<QDBusVariant>());

    for (i = myMap.constBegin(); i != myMap.constEnd(); ++i) {
        argument.beginMapEntry();
        argument << i.key() << QDBusVariant(i.value());
        argument.endMapEntry();
    }
    argument.endMap();
    return argument;
}

const QDBusArgument &operator>>(const QDBusArgument &argument,
        MapUShortVariant &myMap)
{
    argument.beginMap();
    myMap.clear();
    while ( !argument.atEnd() ) {
        // argument
        ushort key;
        QVariant value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();
        myMap.insert(key, value);
    }
    argument.endMap();
    return argument;
}
