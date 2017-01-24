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

#include <CommonAPI/CommonAPI.hpp>
#include <v5/org/genivi/EnhancedPositionService/EnhancedPositionStubDefault.hpp>
#include "gnss-init.h"
#include "gnss.h"

using namespace v5::org::genivi::EnhancedPositionService;

class EnhancedPositionStubImpl: public EnhancedPositionStubDefault {

public:
    EnhancedPositionStubImpl();
    virtual ~EnhancedPositionStubImpl();


    void GetVersion(const std::shared_ptr<CommonAPI::ClientId> _client, GetVersionReply_t _reply);

    void GetPositionInfo(const std::shared_ptr<CommonAPI::ClientId> _client, ::org::genivi::EnhancedPositionService::EnhancedPositionServiceTypes::Bitmask _valuesToReturn, GetPositionInfoReply_t _reply);
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


