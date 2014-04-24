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
#include "demoifadaptor.h"

#include "ConfigurationImpl.h"
#include "confifadaptor.h"

// e.g. testing with dbus-launch
#define USE_DBUS_SESSION_BUS 1

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ServerImpl *server = new ServerImpl();
    new DemoIfAdaptor(server);

    ConfigurationImpl *confServer = new ConfigurationImpl();
    new ConfigurationIfAdaptor(confServer);

#if USE_DBUS_SESSION_BUS
    QDBusConnection connection = QDBusConnection::sessionBus();
#else
    QDBusConnection connection = QDBusConnection::systemBus();
#endif

    server->setConnection(connection);

    bool ret = false;
    ret |= connection.registerService("org.genivi.positioning.EnhancedPosition");
    ret |= connection.registerObject("/position", server);
    ret |= connection.registerObject("/config", confServer);

    if (!ret) {
        return EXIT_FAILURE;
    }

    qDebug() << "loop ...";
    return a.exec();
}
