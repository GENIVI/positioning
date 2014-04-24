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

#ifndef CONFIGURATIONCLIENT_H
#define CONFIGURATIONCLIENT_H

#include "confif.h"

class ConfigurationClient : public QObject
{
Q_OBJECT
public:
    explicit ConfigurationClient(QObject *parent = 0);
    ~ConfigurationClient();

    QDBusConnection& getConnection() { return m_connection; }
    ConfigurationIf* getClient() { return m_client; }

public Q_SLOTS:
    void PropertyChanged(const QString &name, const QDBusVariant &value);

private:
    bool connectToProxy();
    static const QString getServiceName() {
        return QString("org.genivi.positioning.EnhancedPosition");
    }

    QDBusConnection m_connection;
    ConfigurationIf* m_client;
};

#endif
