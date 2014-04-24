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

#include "testclient.h"

#include "agentimpl.h"
#include "genivi-navigationcore-enum.h"

TestClient::TestClient(QObject *parent)
  : m_watcher(this),
    m_client(NULL),
    m_connection(QDBusConnection::sessionBus())
{
    qDebug("TestClient started\n");

    if (!connectToProxy()) {
        qWarning() << "failed to connect to server";
    }

    // listen on bus: signal when EnhancedPositon server is started.
    activateWatcher();
}

TestClient::~TestClient()
{
    m_watcher.removeWatchedService(DemoIf::staticInterfaceName());

    if (m_client != NULL)
        delete(m_client);
}

// signal callback
void TestClient::AccuracyUpdate(const QList<ushort> &changedValues)
{
    qDebug("'%s' is called", __FUNCTION__);
    emit requestAccuracy();
}

// signal callback
void TestClient::PositionUpdate(const QList<ushort> &changedValues)
{
    qDebug("'%s' is called (%d elements)", __FUNCTION__, changedValues.count());
    emit requestData();
}

// signal callback
void TestClient::SatelliteInfoUpdate(const QList<ushort> &changedValues)
{
    qDebug("'%s' is called", __FUNCTION__);
    emit requestSatelliteInfo();
}

// signal callback
void TestClient::StatusUpdate(const QList<ushort> &changedValues)
{
    qDebug("'%s' is called", __FUNCTION__);
    emit requestStatus();
}

// TODO use this to enable a robust client logic which registers to a new
// appearing position daemon.
void TestClient::serviceRegistered(const QString &serviceName)
{
    qDebug("+++ DBus activities + registered %s  +++",
            serviceName.toUtf8().constData());

    connectToProxy();
    // activateAgent();
}

void TestClient::serviceUnregistered(const QString &serviceName)
{
    qDebug("+++ DBus activities + unegistered %s  +++",
            serviceName.toUtf8().constData());

    if (m_client != NULL) {
        delete(m_client);
        m_client = NULL;
    }
}


void TestClient::requestData()
{
    QList<ushort> request;
    request.append(EnhancedPositionData::ALL);

    QDBusPendingReply<MapUShortVariant> reply = getClient()->GetData(request);

    if (reply.isError()) {
        qWarning() << "GetData failed: "
                   << reply.error().name() << "; "
                   << reply.error().message();
        return;
    }

    MapUShortVariant data = reply.value();
    if (data.value(EnhancedPositionData::INVALID).isValid()) {
        qWarning() << "GetData: returned INVALID key";
        return;
    }

    // qDebug() << "GetData: " << data;
    qDebug() << "GetData: " << "\n" <<
            "lat = " << data.value(EnhancedPositionData::LATITUDE) << "\n"
            "long = " << data.value(EnhancedPositionData::LONGITUDE);
}

void TestClient::requestPosition()
{
    QDBusPendingReply<MapUShortVariant> reply = getClient()->GetPosition();

    if (reply.isError()) {
        qWarning() << "GetPosition failed: "
                   << reply.error().name() << "; "
                   << reply.error().message();
        return;
    }

    MapUShortVariant data = reply.value();
    if (data.value(EnhancedPosition::INVALID).isValid()) {
        qWarning() << "GetPosition: returned INVALID key";
        return;
    }

    qDebug() << "GetPosition: " << data;
}

void TestClient::requestStatus()
{
    QDBusPendingReply<MapUShortVariant> reply = getClient()->GetStatus();

    if (reply.isError()) {
        qWarning() << "GetStatus failed: "
                   << reply.error().name() << "; "
                   << reply.error().message();
        return;
    }

    MapUShortVariant data = reply.value();
    if (data.value(EnhancedPositionStatus::INVALID).isValid()) {
        qWarning() << "GetStatus: returned INVALID key";
        return;
    }

    qDebug() << "GetStatus: " << data;
}

void TestClient::requestAccuracy()
{
    QDBusPendingReply<MapUShortVariant> reply = getClient()->GetAccuracy();

    if (reply.isError()) {
        qWarning() << "GetAccuracy failed: "
                   << reply.error().name() << "; "
                   << reply.error().message();
        return;
    }

    MapUShortVariant data = reply.value();
    if (data.value(Accuracy::INVALID).isValid()) {
        qWarning() << "GetAccuracy: returned INVALID key";
        return;
    }

    qDebug() << "GetAccuracy: " << data;
}

void TestClient::requestSatelliteInfo()
{
    QDBusPendingReply<MapUShortVariant> reply = getClient()->GetSatelliteInfo();

    if (reply.isError()) {
        qWarning() << "GetSatelliteInfo failed: "
                   << reply.error().name() << "; "
                   << reply.error().message();
        return;
    }

    MapUShortVariant data = reply.value();
    if (data.value(SatelliteInfo::INVALID).isValid()) {
        qWarning() << "GetSatelliteInfo: returned INVALID key";
        return;
    }

    qDebug() << "GetSatelliteInfo: " << data;
}

// private
bool TestClient::activateWatcher()
{
    // only interesseted in EnhancedPosition-service
    m_watcher.addWatchedService(DemoIf::staticInterfaceName());

    // call me if: registered or unregistered
    connect(&m_watcher, SIGNAL(serviceRegistered(QString)),
            this, SLOT(serviceRegistered(QString)) );
    connect(&m_watcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(serviceUnregistered(QString)) );

    m_watcher.setConnection(getConnection());
}

/*
 * connect signals to get info about position update!
 */
bool TestClient::connectToProxy()
{
    if (m_client != NULL) {
        qWarning("m_client already created. do not continue here.");
        return false;
    }

    m_client = new DemoIf(DemoIf::staticInterfaceName(), "/position", getConnection(), 0);

    if (!m_client->isValid()) {
        qWarning("failed to connect to service '%s'.",
                DemoIf::staticInterfaceName());
        delete(m_client);
        m_client = NULL;
        return false;
    }

    QObject::connect(m_client, SIGNAL(StatusUpdate(QList<ushort>)),
                     this, SLOT(StatusUpdate(QList<ushort>)));
    QObject::connect(m_client, SIGNAL(PositionUpdate(QList<ushort>)),
                     this, SLOT(PositionUpdate(QList<ushort>)));
    QObject::connect(m_client, SIGNAL(SatelliteInfoUpdate(QList<ushort>)),
                         this, SLOT(SatelliteInfoUpdate(QList<ushort>)));
    QObject::connect(m_client, SIGNAL(AccuracyUpdate(QList<ushort>)),
                         this, SLOT(AccuracyUpdate(QList<ushort>)));

    return true;
}

QDBusConnection& TestClient::getConnection()
{
    return m_connection;
}
