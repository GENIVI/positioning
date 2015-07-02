/**********************************************************\

  Auto-generated PositionWebServiceAPI.h

\**********************************************************/
#include <string>
#include <sstream>
#include <boost/weak_ptr.hpp>
#include "JSAPIAuto.h"
#include "BrowserHost.h"
#include "PositionWebService.h"
#include "EnhancedPositionDispatcher.h"

#ifndef H_PositionWebServiceAPI
#define H_PositionWebServiceAPI

class PositionWebServiceAPI : public FB::JSAPIAuto
{
public:
    ////////////////////////////////////////////////////////////////////////////
    /// @fn PositionWebServiceAPI::PositionWebServiceAPI(const posPtr& plugin, const FB::BrowserHostPtr host)
    ///
    /// @brief  Constructor for your JSAPI object.
    ///         You should register your methods, properties, and events
    ///         that should be accessible to Javascript from here.
    ///
    /// @see FB::JSAPIAuto::registerMethod
    /// @see FB::JSAPIAuto::registerProperty
    /// @see FB::JSAPIAuto::registerEvent
    ////////////////////////////////////////////////////////////////////////////
    PositionWebServiceAPI(const PositionWebServicePtr& plugin, const FB::BrowserHostPtr& host) :
        m_plugin(plugin), m_host(host)
    {
        printf("enter PositionWebServiceAPI()\n");
        
        // Read-only property
        registerProperty("version",make_property(this,&PositionWebServiceAPI::get_version));

        registerMethod("GetPosition",make_method(this,&PositionWebServiceAPI::GetPosition));

        mpEnhancedPositionDispatcher = new EnhancedPositionDispatcher(*this);
        
        if(mpEnhancedPositionDispatcher)
        {
          mpEnhancedPositionDispatcher->dispatch();
        }

        printf("exit PositionWebServiceAPI()\n");
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// @fn PositionWebServiceAPI::~PositionWebServiceAPI()
    ///
    /// @brief  Destructor.  Remember that this object will not be released until
    ///         the browser is done with it; this will almost definitely be after
    ///         the plugin is released.
    ///////////////////////////////////////////////////////////////////////////////
    virtual ~PositionWebServiceAPI() {};

    PositionWebServicePtr getPlugin();

    // Read-only property ${PROPERTY.ident}
    std::string get_version();

    // Method echo
    FB::variant echo(const FB::variant& msg);

    FB::VariantMap GetPosition();
 
    // Event helpers
    FB_JSAPI_EVENT(PositionUpdate,1, (FB::VariantList));

private:
    PositionWebServiceWeakPtr m_plugin;
    FB::BrowserHostPtr m_host;

    FB::VariantMap m_position;

    EnhancedPositionDispatcher* mpEnhancedPositionDispatcher;

};

#endif // H_PositionWebServiceAPI

