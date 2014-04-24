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

#include "serverimpl.h"
#include "genivi-navigationcore-enum.h"

#include <iostream>

#include "gnss.h"
#include "gnss-simple.h"

#include <QtCore/QDateTime>

static ServerImpl* instance;

ServerImpl::ServerImpl(QObject *parent)
    : m_pConnection(NULL)
    , m_posFilter(*PositionFilter::getInstance())
    , m_watcher(NULL)
{
    qDebug("ServerImpl instantiated.");
    registerCommTypes();

    // use GENIVI PosGnss interface.
    gnssInit();
    gnssSimpleInit();
    gnssSimpleRegisterCourseCallback(&PositionFilter::onCourseUpdate);
    gnssSimpleRegisterPositionCallback(&PositionFilter::onPositionUpdate);

    // FIXME: how to deal with this? create c++ wrapper for callback API?
    instance = this;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(sendUpdate()));
    timer->start(500); // [ms]
}

ServerImpl::~ServerImpl()
{
    gnssSimpleDeregisterPositionCallback(&PositionFilter::onPositionUpdate);
    gnssSimpleDeregisterCourseCallback(&PositionFilter::onCourseUpdate);
    gnssSimpleDestroy();
}


void ServerImpl::setConnection(QDBusConnection& conn)
{
    if (!conn.isConnected())
        qWarning("SetConnection: conn is not connected.");

    m_pConnection = &conn;

    // setup watcher - detects disappearing clients.
    m_watcher.setConnection(conn);
}


MapUShortVariant ServerImpl::GetAccuracy()
{
    MapUShortVariant temp; // FIXME: use real values.
    temp.insert(Accuracy::INVALID, "not implemented");

    qDebug("%s was called.", __FUNCTION__);
    return temp;
}

MapUShortVariant ServerImpl::GetData(const QList<ushort> &valuesToReturn)
{
    MapUShortVariant temp;

    QList<ushort> changeList;
    QList<ushort>::const_iterator i;
    for (i = valuesToReturn.begin(); i != valuesToReturn.end(); ++i) {
        qDebug() << "append now" << (ushort)*i;
        m_posFilter.appendElementTo(temp, *i);
    }

    if (temp.count() <= 0) {
        temp.insert(EnhancedPosition::INVALID, "no valid position available.");
        qDebug("%s: no valid position available.", __FUNCTION__);
    }

    qDebug("%s was called. (%d)", __FUNCTION__, temp.count());
    return temp;
}

MapUShortVariant ServerImpl::GetPosition()
{
    MapUShortVariant temp;

    qDebug("GetPosition: ..");

    QList<ushort> changeList;
    m_posFilter.appendPositionTo(temp);
    m_posFilter.appendCourseTo(temp);
    m_posFilter.appendTimestampTo(temp);

    if (temp.count() <= 0)
        temp.insert(EnhancedPosition::INVALID, "no valid position available.");

    qDebug("%s was called.", __FUNCTION__);
    return temp;
}

MapUShortVariant ServerImpl::GetRotationRate()
{
    MapUShortVariant temp;

    if (temp.count() <= 0)
        temp.insert(RotationRate::INVALID, "no valid rotation rate available.");

    qDebug("%s was called.", __FUNCTION__);
    return temp;
}

MapUShortVariant ServerImpl::GetSatelliteInfo()
{
    MapUShortVariant temp;
    qDebug("%s was called.", __FUNCTION__);

    if (message().service().isEmpty())
        return temp;

    //
    // NOT IMPLEMENTED, but demonstrates error message
    //

    m_pConnection->call(
                message().createErrorReply(
                        "org.genivi.positioning.EnhancedPosition.NotImplemented",
                        "method 'GetSatelliteInfo' is not implemented."),
                QDBus::NoBlock);

    return temp;
}

MapUShortVariant ServerImpl::GetStatus()
{
    MapUShortVariant temp;

    qDebug("%s was called.", __FUNCTION__);
    temp.insert(EnhancedPositionStatus::DR_STATUS, false);

    return temp;
}

MapUShortVariant ServerImpl::GetTime()
{
    MapUShortVariant temp;

    QDateTime now = QDateTime::currentDateTimeUtc();
    QDate date = now.date();
    QTime time = now.time();

    temp.insert(Time::YEAR, date.year());
    temp.insert(Time::MONTH, date.month());
    temp.insert(Time::DAY, date.day());

    temp.insert(Time::HOUR, time.hour());
    temp.insert(Time::MINUTE, time.minute());
    temp.insert(Time::SECOND, time.second());
    temp.insert(Time::MS, time.msec());

    qDebug("%s was called.", __FUNCTION__);
    return temp;
}

GeniviVersion ServerImpl::GetVersion()
{
    GeniviVersion apiVersion(1, 0, 0);
    qDebug("%s was called.", __FUNCTION__);
    return apiVersion;
}

QDBusConnection* ServerImpl::getConnection()
{
    return m_pConnection;
}

// TODO: decide if still part of demo, not call at the moment.
bool ServerImpl::activateWatcherFor(const QString& uniqueId) {
    // only interested in EnhancedPosition-service
    m_watcher.addWatchedService(uniqueId);

    // call me only if agent is unregistered
    connect(&m_watcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(onAgentUnregistered(QString)) );

    return true;
}

void ServerImpl::onAgentUnregistered(const QString &serviceName)
{
    qDebug("+++++ agent disappeared: %s ++++++",
            serviceName.toUtf8().data());

    m_watcher.removeWatchedService(serviceName);
}

/**
 * This method is called by a QTimer to update (later) the Status.
 */
void ServerImpl::changeStatus()
{
    //checkAndPrintDBusContext();
}

void ServerImpl::sendUpdate()
{
    // extract and fill data for signal!
    QList<ushort> updates;
    PositionFilter::getInstance()->runFiltering(updates);

    if (0 == updates.count()) {
        // updates.append(EnhancedPosition::INVALID);
        //qDebug("*** sendUpdate: no changes or invalild content! --> no PositionUpdate signal");
        return;
    }

    qDebug("*** sendUpdate done *** ");
    PositionUpdate(updates);
}

/**
 * Visualizes the important infos from the current DBus context.
 * The information is provided via message() and connection() call.
 */
bool ServerImpl::checkAndPrintDBusContext()
{
    bool rv = false;
#if 0
    if (m_agentPath != NULL) {
        qDebug("m_agentPath: %s", m_agentPath.toUtf8().data());
        rv = true;
    }

    if (m_agentUniqueName != NULL) {
        qDebug("m_agentUniqueName: %s", m_agentUniqueName.toUtf8().data());
        rv = true;
    }

    if (!QDBusContext::calledFromDBus())
        return false;

    if (message().service() != NULL) {
        // show infos we use to identify the agent.
        qDebug("message():\n"
            "  * path      : '%s'\n"
            "  * interface : '%s'\n"
            "  * service   : '%s'\n", message().path().toUtf8().data(),
                message().interface().toUtf8().data(),
                message().service().toUtf8().data());
        rv = true;
    }

    if (connection().sender().baseService() != NULL) {
        qDebug("connection():\n"
            "  * sender    : '%s'\n",
                connection().sender().baseService().toUtf8().data());
        rv |= true;
    }

    if (!rv) {
        qWarning("chack and print of DBus context failed.\n");
    }
#endif

    return rv;
}
