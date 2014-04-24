/**************************************************************************
 * Part of GENIVI Proof of Concept for Positioning
 * Copyright (C) BMW Car IT GmbH 2011, 2012
 **************************************************************************/


#ifndef GENIVI_VERSION_H_
#define GENIVI_VERSION_H_

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMetaType>
#include <QtDBus/QtDBus>

class GeniviVersion {
public:
    GeniviVersion();
    GeniviVersion(uint major, uint minor, uint micro=0);
    virtual ~GeniviVersion();

    void setVersion(uint major, uint minor, uint micro);

    uint getMajorVersion() const;
    uint getMinorVersion() const;
    uint getMicroVersion() const;

private:
    uint _major;
    uint _minor;
    uint _micro;
    //QString date; // TODO fix issue with defect dereference
};

typedef QMap<int,QVariant> MapIntVariant;
typedef QMap<ushort,QVariant> MapUShortVariant;

Q_DECLARE_METATYPE(GeniviVersion);
Q_DECLARE_METATYPE(QList<bool>);
Q_DECLARE_METATYPE(QList<int>);
Q_DECLARE_METATYPE(QList<ushort>);
Q_DECLARE_METATYPE(QList<QString>);
Q_DECLARE_METATYPE(MapUShortVariant);
Q_DECLARE_METATYPE(MapIntVariant);

inline void registerCommTypes() {
    qDebug("registerCommTypes: GeniviVersion");
    qDBusRegisterMetaType<GeniviVersion >();
    qDBusRegisterMetaType<QList<bool> >();
    qDBusRegisterMetaType<QList<int> >();
    qDBusRegisterMetaType<QList<ushort> >();
    qDBusRegisterMetaType<QList<QString> >();
    qDBusRegisterMetaType<MapIntVariant >();
    qDBusRegisterMetaType<MapUShortVariant >();
}


QDBusArgument &operator<<(QDBusArgument &argument, const GeniviVersion &mystruct);
const QDBusArgument &operator>>(const QDBusArgument &argument,
    GeniviVersion &mystruct);

QDBusArgument &operator<<(QDBusArgument &argument, const MapIntVariant &myMap);
const QDBusArgument &operator>>(const QDBusArgument &argument,
        MapIntVariant &myMap);

QDBusArgument &operator<<(QDBusArgument &argument, const MapUShortVariant &myMap);
const QDBusArgument &operator>>(const QDBusArgument &argument,
        MapUShortVariant &myMap);

#endif /* GENIVI_VERSION_H_ */
