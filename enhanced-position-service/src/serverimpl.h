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

#ifndef SERVERIMPL_H
#define SERVERIMPL_H

#include <QtCore>
#include <QtDBus>

#include "genivi-dbus-types.h"
#include "ConfigurationImpl.h"

#include "gnss-simple.h"
#include "PositionFilter.h"

// FIXME: decide, if still needed.
enum _EnhancedPositionStatus {
    ENH_STATUS_INVALID = 0,
    ENH_STATUS_UNKNOWN = 1,
    ENH_STATUS_DISCONNECTED = 2,
    ENH_STATUS_STARTUP = 3,
    ENH_STATUS_READY = 4,
    ENH_STATUS_CONNECTED = 5
};

/*
 * Signatures needs to match demoifadaptor.h
 */
class ServerImpl : public QObject, public QDBusContext
{
Q_OBJECT
public:
    explicit ServerImpl(QObject *parent = 0);
    ~ServerImpl();
    void setConnection(QDBusConnection& conn);

    QDBusConnection* getConnection();


public: // PROPERTIES
public Q_SLOTS: // METHODS
    MapUShortVariant GetAccuracy();
    MapUShortVariant GetData(const QList<ushort> &valuesToReturn);
    MapUShortVariant GetPosition();
    MapUShortVariant GetRotationRate();
    MapUShortVariant GetSatelliteInfo();
    MapUShortVariant GetStatus();
    MapUShortVariant GetTime();
    GeniviVersion GetVersion();

Q_SIGNALS: // SIGNALS
    void AccuracyUpdate(const QList<ushort> &changedValues);
    void PositionUpdate(const QList<ushort> &changedValues);
    void RotationRateUpdate(const QList<ushort> &changedValues);
    void SatelliteInfoUpdate(const QList<ushort> &changedValues);
    void StatusUpdate(const QList<ushort> &changedValues);

private Q_SLOTS:
    void changeStatus();
    void sendUpdate();
    void onAgentUnregistered(const QString &serviceName);

private:
    bool checkAndPrintDBusContext();
    bool activateWatcherFor(const QString& uniqueId);

    QDBusConnection* m_pConnection;
    QDBusServiceWatcher m_watcher;

    _EnhancedPositionStatus m_status;

    PositionFilter& m_posFilter;
};

#endif
