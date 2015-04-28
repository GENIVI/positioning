/**************************************************************************
* @licence app begin@
*
* SPDX-License-Identifier: MPL-2.0
*
* \ingroup EnhancedPositionService
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

#ifndef ENHANCEDPOSITIONSTUBIMPL_H_
#define ENHANCEDPOSITIONSTUBIMPL_H_

#include <CommonAPI/CommonAPI.h>
#include <org/genivi/EnhancedPositionService/EnhancedPositionStubDefault.h>
#include "gnss-init.h"
#include "gnss.h"

using namespace org::genivi::EnhancedPositionService;

class EnhancedPositionStubImpl: public EnhancedPositionStubDefault {

public:
    EnhancedPositionStubImpl();
    virtual ~EnhancedPositionStubImpl();
    void GetVersion(EnhancedPositionServiceTypes::Version& version);
    void GetPositionInfo(EnhancedPositionServiceTypes::Bitmask valuesToReturn, EnhancedPositionServiceTypes::Timestamp& timestamp, EnhancedPositionServiceTypes::PositionInfo& data);
    void run();
    void shutdown();
private:
    bool checkMajorVersion(int expectedMajor);
    static void cbPosition(const TGNSSPosition position[], uint16_t numElements);
    static void cbSatelliteDetail(const TGNSSSatelliteDetail satelliteDetail[], uint16_t numElements);
    static void sigPositionUpdate(const TGNSSPosition position[], uint16_t numElements);
    static EnhancedPositionStubImpl* mpSelf;
};

#endif /* ENHANCEDPOSITIONSTUBIMPL_H_ */


