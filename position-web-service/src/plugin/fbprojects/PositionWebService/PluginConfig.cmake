#/**********************************************************\ 
#
# Auto-Generated Plugin Configuration file
# for PositionWebService
#
#\**********************************************************/

set(PLUGIN_NAME "PositionWebService")
set(PLUGIN_PREFIX "PosWebSrv")
set(COMPANY_NAME "XSe")

# ActiveX constants:
set(FBTYPELIB_NAME PositionWebServiceLib)
set(FBTYPELIB_DESC "Position Web Service 1.0 Type Library")
set(IFBControl_DESC "Position Web Service Control Interface")
set(FBControl_DESC "Position Web Service Control Class")
set(IFBComJavascriptObject_DESC "PositionWebService IComJavascriptObject Interface")
set(FBComJavascriptObject_DESC "PositionWebService ComJavascriptObject Class")
set(IFBComEventSource_DESC "PositionWebService IFBComEventSource Interface")
set(AXVERSION_NUM "1")

# NOTE: THESE GUIDS *MUST* BE UNIQUE TO YOUR PLUGIN/ACTIVEX CONTROL!  YES, ALL OF THEM!
set(FBTYPELIB_GUID 23f66e5f-0d82-56d5-8b58-e2b37797611c)
set(IFBControl_GUID 05cba700-8b67-58cc-9f3d-289f2d1b1cb6)
set(FBControl_GUID 4c366758-58a5-582d-b72a-7ba16865d985)
set(IFBComJavascriptObject_GUID 77875990-dacb-524c-a56b-a38b0a6765a7)
set(FBComJavascriptObject_GUID 05dc6047-cc4e-5d97-baa3-19d3b61c96f9)
set(IFBComEventSource_GUID 1cfa7a29-5054-5316-998d-e62017828a25)
if ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID 8cee0269-b394-5d88-aade-f6a5f20707ba)
else ( FB_PLATFORM_ARCH_32 )
    set(FBControl_WixUpgradeCode_GUID f78611db-f271-5b79-a8f5-301d5e21fd30)
endif ( FB_PLATFORM_ARCH_32 )

# these are the pieces that are relevant to using it from Javascript
set(ACTIVEX_PROGID "XSe.pos-web-service")
if ( FB_PLATFORM_ARCH_32 )
    set(MOZILLA_PLUGINID "xse.de/position-web-service")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(MOZILLA_PLUGINID "xse.de/position-web-service_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )

# strings
set(FBSTRING_CompanyName "XSe")
set(FBSTRING_PluginDescription "PoC of Position Web Service")
set(FBSTRING_PLUGIN_VERSION "1.0.0.0")
set(FBSTRING_LegalCopyright "Copyright 2014 XSe")
set(FBSTRING_PluginFileName "np${PLUGIN_NAME}")
set(FBSTRING_ProductName "PositionWebService")
set(FBSTRING_FileExtents "")
if ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "PositionWebService")  # No 32bit postfix to maintain backward compatability.
else ( FB_PLATFORM_ARCH_32 )
    set(FBSTRING_PluginName "PositionWebService_${FB_PLATFORM_ARCH_NAME}")
endif ( FB_PLATFORM_ARCH_32 )
set(FBSTRING_MIMEType "application/x-position-web-service")

# Uncomment this next line if you're not planning on your plugin doing
# any drawing:

#set (FB_GUI_DISABLED 1)

# Mac plugin settings. If your plugin does not draw, set these all to 0
set(FBMAC_USE_QUICKDRAW 0)
set(FBMAC_USE_CARBON 1)
set(FBMAC_USE_COCOA 1)
set(FBMAC_USE_COREGRAPHICS 1)
set(FBMAC_USE_COREANIMATION 0)
set(FBMAC_USE_INVALIDATINGCOREANIMATION 0)

# If you want to register per-machine on Windows, uncomment this line
#set (FB_ATLREG_MACHINEWIDE 1)


