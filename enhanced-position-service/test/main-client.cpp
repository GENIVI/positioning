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
#include "ConfigurationClient.h"
#include "genivi-version.h"

#include "signalhelper.h"


int checkVersion(const QDBusConnection& conn, DemoIf* client)
{
    if (client == NULL) {
        qWarning("client == NULL: can not speak with server, proxy not initialized.");
        return -1;
    }

    if (!client->isValid()) {
        qWarning("client is not valid: can not check for version.");
        return -1;
    }

    // request api version
    QDBusReply<GeniviVersion> replyResult;
    replyResult = client->GetVersion();

    if (replyResult.isValid()) {
        GeniviVersion version = replyResult.value();

        qDebug("API version: %d.%d.%d", version.getMajorVersion(),
               version.getMinorVersion(), version.getMicroVersion());
               // versionDate.toUtf8().data());
    }
    else {
        qDebug("reply is not valid?:");
        qDebug() << replyResult.error();
    }

    return 0;
}

void getConfiguration(ConfigurationIf* client)
{
    if (client == NULL) {
        qWarning("getConfiguration: client is null.");
        return;
    }

    QDBusPendingReply<QVariantMap> reply = client->GetProperties();

    if (reply.isError()) {
        qWarning() << reply.error().name() << "; " << reply.error().message();
        return;
    }

    QVariantMap props = reply.value();
    qDebug() << "Configuration properties: " << props;
}

int main(int argc, char **argv)
{
    QCoreApplication a(argc, argv);

    registerCommTypes();

    // TestClient implements the 'DBus signal' callbacks
    TestClient testClient;
    ConfigurationClient confClient;

    // terminates main loop if SIGTERM appears
    SignalHelper sigTermHelper;

    // do a first call to check connection to service.
    checkVersion(testClient.getConnection(), testClient.getClient());
    getConfiguration(confClient.getClient());

    // loop
    int rv = a.exec();

    return rv;
}
