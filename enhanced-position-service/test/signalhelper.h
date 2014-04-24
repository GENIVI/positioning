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

/*
 * See documentation at:
 *   http://doc.qt.nokia.com/4.8/unix-signals.html
 */

#ifndef MYCLIENT_H_
#define MYCLIENT_H_

#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>

#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

static int sighupFd[2];
static int sigtermFd[2];

class SignalHelper : public QObject {
Q_OBJECT

public:
	SignalHelper(QObject *parent = 0, const char *name = 0);
	~SignalHelper();

	// Unix signal handlers.
	static void hupSignalHandler(int unused);
	static void termSignalHandler(int unused);

public slots:
	// Qt signal handlers.
	void handleSigHup();
	void handleSigTerm();

private:
	QSocketNotifier *snHup;
	QSocketNotifier *snTerm;
};

#endif /* MYCLIENT_H_ */
