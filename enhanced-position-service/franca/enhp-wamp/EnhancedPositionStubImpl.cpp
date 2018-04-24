/**
* @licence app begin@
* SPDX-License-Identifier: MPL-2.0
*
* \copyright Copyright (C) 2018, PSA Groupe
*
* \file EnhancedPositionStubImpl.cpp
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

#include "EnhancedPositionStubImpl.hpp"

namespace v5 {
namespace org {
namespace genivi {
namespace enhancedpositionservice {

EnhancedPositionStubImpl::EnhancedPositionStubImpl() : EnhancedPositionStubDefault(){
    std::cout << "EnhancedPositionStubImpl constructor called" << std::endl;
//    m_PositionInfo[EnhancedPositionServiceTypes::PositionInfoKey::LATITUDE]=47.31608607;
//    m_PositionInfo[EnhancedPositionServiceTypes::PositionInfoKey::LONGITUDE]=6.23329262;
//    m_PositionInfo[EnhancedPositionServiceTypes::PositionInfoKey::ALTITUDE]=337;
}

EnhancedPositionStubImpl::~EnhancedPositionStubImpl(){

}

void EnhancedPositionStubImpl::GetPositionInfo(const std::shared_ptr<CommonAPI::ClientId> _client, EnhancedPositionServiceTypes::Bitmask _valuesToReturn, GetPositionInfoReply_t _reply){
    EnhancedPositionServiceTypes::Timestamp _timestamp;
    EnhancedPositionServiceTypes::PositionInfo _data;

    _reply(_timestamp,_data);
}

void EnhancedPositionStubImpl::GetSatelliteInfo(const std::shared_ptr<CommonAPI::ClientId> _client, GetSatelliteInfoReply_t _reply){
    EnhancedPositionServiceTypes::Timestamp _timestamp;
    EnhancedPositionServiceTypes::SatelliteInfo _satelliteInfo;
    _reply(_timestamp,_satelliteInfo);
}

void EnhancedPositionStubImpl::GetTime(const std::shared_ptr<CommonAPI::ClientId> _client, GetTimeReply_t _reply){
    EnhancedPositionServiceTypes::Timestamp _timestamp;
    EnhancedPositionServiceTypes::TimeInfo _time;
    _reply(_timestamp,_time);
}

static const CommonAPI::Version version(0, 0);

const CommonAPI::Version& EnhancedPositionStubImpl::getInterfaceVersion(std::shared_ptr<CommonAPI::ClientId> clientId) {
    return version;

}

} // namespace enhancedpositionservice
} // namespace genivi
} // namespace org
} // namespace v5
