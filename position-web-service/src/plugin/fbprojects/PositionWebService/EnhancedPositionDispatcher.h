/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup PositionWebService
* \author Marco Residori <marco.residori@xse.de>
*
* \copyright Copyright (C) 2014, XS Embedded GmbH
* 
* \license
* This Source Code Form is subject to the terms of the
* Mozilla Public License, v. 2.0. If a copy of the MPL was not distributed with
* this file, You can obtain one at http://mozilla.org/MPL/2.0/.
*
* @licence end@
**************************************************************************/
#ifndef __ENHANCED_POSITION_DISPATCHER_H
#define __ENHANCED_POSITION_DISPATCHER_H

#include "EnhancedPositionClient.h"

class PositionWebServiceAPI;

class EnhancedPositionDispatcher
{
public:

  EnhancedPositionDispatcher(PositionWebServiceAPI& positionWebServiceAPI);
  ~EnhancedPositionDispatcher();

  void dispatch();
  void stop();
  EnhancedPositionClient * getClient();

private:
  EnhancedPositionClient * mpEnhancedPositionClient;
};

#endif//__ENHANCED_POSITION_DISPATCHER_H
