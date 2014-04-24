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

#ifndef TESTCLIENT_H
#define TESTCLIENT_H

#include "demoif.h"

#include <QtDBus/QDBusServiceWatcher>

/*
 * signaturs needs to match demoifadaptor.h
 */
class TestClient : public QObject
{
Q_OBJECT
public:
    explicit TestClient(QObject *parent = 0);
    ~TestClient();
    QDBusConnection& getConnection();
    DemoIf* getClient() { return m_client; }

private Q_SLOTS:
    // calls when the relevant service appears/disappears.
    void serviceRegistered(const QString &serviceName);
    void serviceUnregistered(const QString &servicName);

public Q_SLOTS:
    void AccuracyUpdate(const QList<ushort> &changedValues);
    void PositionUpdate(const QList<ushort> &changedValues);
    void SatelliteInfoUpdate(const QList<ushort> &changedValues);
    void StatusUpdate(const QList<ushort> &changedValues);

private Q_SLOTS:
    void requestData();
    void requestPosition();
    void requestStatus();
    void requestAccuracy();
    void requestSatelliteInfo();

private:
    bool activateWatcher();
    bool connectToProxy();

// member:
    DemoIf* m_client;
    QDBusConnection m_connection;
    QDBusServiceWatcher m_watcher;
};

#endif /* TESTCLIENT_H */
