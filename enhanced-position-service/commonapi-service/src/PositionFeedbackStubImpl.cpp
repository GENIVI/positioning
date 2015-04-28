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

#include "PositionFeedbackStubImpl.h"
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

void PositionFeedbackStubImpl::GetVersion(EnhancedPositionServiceTypes::Version& version) {
    LOG_INFO_MSG(gCtx,"GetVersion");

    version.maj = VER_MAJOR;
    version.min = VER_MINOR;
    version.mic = VER_MICRO;
    version.date = std::string(VER_DATE);
}

void PositionFeedbackStubImpl::SetPositionFeedback(std::vector<PositionFeedback::PositionFeedbackInfo> feedback, uint64_t timestamp, EnhancedPositionServiceTypes::PositionFeedbackType feedbackType) {
    LOG_WARNING_MSG(gCtx,"Method not supported yet");
}

void PositionFeedbackStubImpl::run()
{
    LOG_INFO_MSG(gCtx,"Starting PositionFeedback dispatcher...");
}

void PositionFeedbackStubImpl::shutdown()
{
    LOG_INFO_MSG(gCtx,"Shutting down PositionFeedback dispatcher...");
}




