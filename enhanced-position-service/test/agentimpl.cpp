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

#include "agentimpl.h"
#include "agentadaptor.h"


AgentImpl::AgentImpl(QObject *parent) : m_adaptor(NULL)
{
    m_adaptor = new AgentAdaptor(this);
}

AgentImpl::~AgentImpl()
{

}

void AgentImpl::StatusUpdate(uint status)
{
    qDebug("Agent: received 'StatusUpdate' %d.", status);
}

void AgentImpl::PositionUpdate(const QList<bool> &validFields, double latitude, double longitude, double altitude, uint timestamp)
{
    qDebug("Agent: received 'PositionUpdate' ... position %f, %f", latitude, longitude);
}
