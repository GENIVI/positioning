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


#ifndef CONFIGURATIONIMPL_H
#define CONFIGURATIONIMPL_H

#include <QtCore/QObject>
#include <QtDBus/QtDBus>
#include <QtDBus/QDBusContext>

#include "genivi-dbus-types.h"

class ConfigurationImpl : public QObject, public QDBusContext
{
Q_OBJECT
public:
    explicit ConfigurationImpl(QObject *parent = 0);
    ~ConfigurationImpl();

public Q_SLOTS: // METHODS
    QVariantMap GetProperties();
    GeniviVersion GetVersion();
    void SetProperty(const QString &name, const QDBusVariant &value);

Q_SIGNALS: // SIGNALS
    void PropertyChanged(const QString &name, const QDBusVariant &value);
};

#endif /* CONFIGURATIONIMPL_H */
