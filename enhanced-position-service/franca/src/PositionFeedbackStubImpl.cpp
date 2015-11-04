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

#include "PositionFeedbackStubImpl.hpp"
#include "log.h"

//PositionFeedback-interface version
#define VER_MAJOR 3
#define VER_MINOR 0
#define VER_MICRO 0
#define VER_DATE "05-08-2014"

DLT_IMPORT_CONTEXT(gCtx);

PositionFeedbackStubImpl::PositionFeedbackStubImpl() {
}

PositionFeedbackStubImpl::~PositionFeedbackStubImpl() {
}

void PositionFeedbackStubImpl::GetVersion(const std::shared_ptr<CommonAPI::ClientId> _client, GetVersionReply_t _reply) {
    LOG_INFO_MSG(gCtx,"GetVersion");

    EnhancedPositionServiceTypes::Version PositionFeedbackVersion(VER_MAJOR,VER_MINOR,VER_MICRO,std::string(VER_DATE));

    _reply(PositionFeedbackVersion);
}

void PositionFeedbackStubImpl::SetPositionFeedback(const std::shared_ptr<CommonAPI::ClientId> _client, ::org::genivi::EnhancedPositionService::EnhancedPositionServiceTypes::PositionFeedback _feedback, uint64_t _timestamp, ::org::genivi::EnhancedPositionService::EnhancedPositionServiceTypes::PositionFeedbackType _feedbackType, SetPositionFeedbackReply_t _reply) 
{
    LOG_WARNING_MSG(gCtx,"Method not supported yet");
    _reply(); 
}

void PositionFeedbackStubImpl::run()
{
    LOG_INFO_MSG(gCtx,"Starting PositionFeedback dispatcher...");
}

void PositionFeedbackStubImpl::shutdown()
{
    LOG_INFO_MSG(gCtx,"Shutting down PositionFeedback dispatcher...");
}




