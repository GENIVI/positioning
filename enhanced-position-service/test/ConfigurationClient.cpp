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

#include "ConfigurationClient.h"

ConfigurationClient::ConfigurationClient(QObject *parent)
  : m_connection(QDBusConnection::sessionBus()),
    m_client(NULL)
{
    qDebug("ConfigurationClient started\n");

    if (!connectToProxy()) {
        qWarning() << "failed to connect to server";
    }
}

ConfigurationClient::~ConfigurationClient()
{
    qDebug("ConfigurationClient: d-tor\n");
}

void ConfigurationClient::PropertyChanged(const QString &name, const QDBusVariant &value)
{
    qDebug("PropertyChanged: %s", name.toUtf8().constData());
}

bool ConfigurationClient::connectToProxy()
{
    if (m_client != NULL) {
        qWarning("m_client already created. do not continue here.");
        return false;
    }

    m_client = new ConfigurationIf(getServiceName(), "/config", getConnection(), 0);

    if (!m_client->isValid()) {
        qWarning("failed to connect to service '%s'.",
                getServiceName().toUtf8().constData());
        delete(m_client);
        m_client = NULL;
        return false;
    }

#if 0
    QObject::connect(m_client, SIGNAL(StatusUpdate(QList<int>)),
                     this, SLOT(StatusUpdate(QList<int>)));
    QObject::connect(m_client, SIGNAL(PositionUpdate(QList<int>)),
                     this, SLOT(PositionUpdate(QList<int>)));
    QObject::connect(m_client, SIGNAL(SatelliteInfoUpdate(QList<int>)),
                         this, SLOT(SatelliteInfoUpdate(QList<int>)));
    QObject::connect(m_client, SIGNAL(AccuracyUpdate(QList<int>)),
                         this, SLOT(AccuracyUpdate(QList<int>)));
#endif

    return true;
}
