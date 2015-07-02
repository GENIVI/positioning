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

#include "JSObject.h"
#include "variant_list.h"
#include "DOM/Document.h"
#include "global/config.h"

#include "PositionWebServiceAPI.h"
#include "SharedData.h"
#include "Log.h"

DLT_IMPORT_CONTEXT(gCtx);


///////////////////////////////////////////////////////////////////////////////
/// @fn PositionWebServicePtr PositionWebServiceAPI::getPlugin()
///
/// @brief  Gets a reference to the plugin that was passed in when the object
///         was created.  If the plugin has already been released then this
///         will throw a FB::script_error that will be translated into a
///         javascript exception in the page.
///////////////////////////////////////////////////////////////////////////////
PositionWebServicePtr PositionWebServiceAPI::getPlugin()
{
    PositionWebServicePtr plugin(m_plugin.lock());
    if (!plugin) {
        throw FB::script_error("The plugin is invalid");
    }
    return plugin;
}

FB::VariantMap PositionWebServiceAPI::GetPosition()
{
  LOG_INFO_MSG(gCtx,"GetPosition");
  double val = 0;

  SharedData& PositioningData = SharedData::getInstance();
  
  PositioningData.getLatitude(val);
  m_position["LATITUDE"] = val;

  PositioningData.getLongitude(val);
  m_position["LONGITUDE"] = val;

  PositioningData.getAltitude(val);
  m_position["ALTITUDE"] = val;

  PositioningData.getSpeed(val);
  m_position["SPEED"] = val;

  return m_position;
}

// Read-only property version
std::string PositionWebServiceAPI::get_version()
{
    return FBSTRING_PLUGIN_VERSION;
}


