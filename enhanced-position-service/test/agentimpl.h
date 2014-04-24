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

#ifndef AGENTIMPL_H_
#define AGENTIMPL_H_

#include <QtCore/QObject>
#include <QtDBus/QtDBus>

class AgentAdaptor;

class AgentImpl : public QObject
{
Q_OBJECT
public:
    explicit AgentImpl(QObject *parent = 0);
    ~AgentImpl();

public Q_SLOTS: // METHODS
    void StatusUpdate(uint status);
    void PositionUpdate(const QList<bool> &validFields, double latitude, double longitude, double altitude, uint timestamp);
Q_SIGNALS: // SIGNALS

private:
    AgentAdaptor* m_adaptor;

};

#endif /* AGENTIMPL_H_ */
