/**
* @licence app begin@
* SPDX-License-Identifier: MPL-2.0
*
* \copyright Copyright (C) 2018, PSA Groupe
*
* \file EnhancedPositionStubImpl.hpp
*
* \brief This file is part of the enhp Wamp proof of concept.
*
* \author Philippe Colliot <philippe.colliot@mpsa.com>
*
* \version 0.1
*
* This Source Code Form is subject to the terms of the
* Mozilla Public License (MPL), v. 2.0.
* If a copy of the MPL was not distributed with this file,
* You can obtain one at http://mozilla.org/MPL/2.0/.
*
* For further information see http://www.genivi.org/.
*
* List of changes:
* <date>, <name>, <description of change>
*
* @licence end@
*/

#ifndef EnhancedPositionStubImpl_H_
#define EnhancedPositionStubImpl_H_

#include "v5/org/genivi/enhancedpositionservice/EnhancedPositionStubDefault.hpp"

namespace v5 {
namespace org {
namespace genivi {
namespace enhancedpositionservice {

class EnhancedPositionStubImpl : public EnhancedPositionStubDefault {
public:
    EnhancedPositionStubImpl();

    virtual ~EnhancedPositionStubImpl();

    virtual const CommonAPI::Version& getInterfaceVersion(std::shared_ptr<CommonAPI::ClientId> clientId);

    virtual void GetPositionInfo(const std::shared_ptr<CommonAPI::ClientId> _client, ::v5::org::genivi::enhancedpositionservice::EnhancedPositionServiceTypes::Bitmask _valuesToReturn, GetPositionInfoReply_t _reply);

    virtual void GetSatelliteInfo(const std::shared_ptr<CommonAPI::ClientId> _client, GetSatelliteInfoReply_t _reply);

    virtual void GetTime(const std::shared_ptr<CommonAPI::ClientId> _client, GetTimeReply_t _reply);

    EnhancedPositionServiceTypes::PositionInfo m_PositionInfo;
    EnhancedPositionServiceTypes::SatelliteInfo m_constSatelliteInfo;
    EnhancedPositionServiceTypes::TimeInfo m_constTimeInfo;
private:

};

} // namespace enhancedpositionservice
} // namespace genivi
} // namespace org
} // namespace v5
#endif /* EnhancedPositionStubImpl */
