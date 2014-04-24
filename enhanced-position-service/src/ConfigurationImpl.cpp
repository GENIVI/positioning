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

#include "ConfigurationImpl.h"

ConfigurationImpl::ConfigurationImpl(QObject *parent)
{

}

ConfigurationImpl::~ConfigurationImpl()
{

}

GeniviVersion ConfigurationImpl::GetVersion()
{
    GeniviVersion v;
    v.setVersion(1,0,0);
    return v;
}

QVariantMap ConfigurationImpl::GetProperties()
{
    QVariantMap props;

    props.insert("UpdateInterval",  quint32(1000));
    props.insert("SatelliteSystem", quint16(0x02)); // 0x01 = INVALID, 0x02 = GPS

    return props;
}

void ConfigurationImpl::SetProperty(const QString &name, const QDBusVariant &value)
{
    qDebug("SetProperty: %s", name.toUtf8().constData());
    return;
}
