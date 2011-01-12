/**********************************************************\ 
 Original Author: Georg Fritzsche
 
 Created:    Oct 23, 2010
 License:    Dual license model; choose one of two:
 New BSD License
 http://www.opensource.org/licenses/bsd-license.php
 - or -
 GNU Lesser General Public License, version 2.1
 http://www.gnu.org/licenses/lgpl-2.1.html
 
 Copyright 2010 Georg Fritzsche, Firebreath development team
 \**********************************************************/

#pragma once 
#ifndef H_FB_FACTORYBASE
#define H_FB_FACTORYBASE

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>
#include "APITypes.h"
#include "FBPointers.h"
#include "logging.h"

#include "PluginWindowForwardDecl.h"

namespace FB
{
    FB_FORWARD_PTR(FactoryBase);
    FB_FORWARD_PTR(PluginCore);
    FB_FORWARD_PTR(BrowserHost);
    FB_FORWARD_PTR(JSAPI);
    /// @brief  Defines an alias representing a boost::shared_ptr<FactoryBase>
    typedef boost::shared_ptr<FactoryBase> FactoryBasePtr;

    /// @brief  Defines an alias representing a boost::shared_ptr<PluginCore>
    typedef boost::shared_ptr<PluginCore> PluginCorePtr;
    
    namespace Npapi { 
        FB_FORWARD_PTR(NpapiPlugin);
        FB_FORWARD_PTR(NpapiBrowserHost);
    }
}
    
/// @brief Get an instance of the factory class. This function *must* be defined in your plugin project.
FB::FactoryBasePtr getFactoryInstance();
struct IDispatchEx;
typedef long HRESULT;

namespace FB
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /// @class  FactoryBase
    ///
    /// @brief  This is the base factory for a plugin.  Every plugin *must* implement this class
    ///         and override at least the createPlugin() method.  To futher customize your plugin,
    ///         you can override other methods to replace the PluginWindow or the NpapiPlugin class.
    ///         
    /// @code
    ///      // Example implementation:
    ///      class PluginFactory : public FB::FactoryBase
    ///      {
    ///      public:
    ///          FB::PluginCorePtr createPlugin(const std::string& mimetype)
    ///          {
    ///              return boost::make_shared<MyPluginObject>();
    ///          }
    ///          
    ///          void globalPluginInitialize()
    ///          {
    ///              MyPluginObject::StaticInitialize();
    ///          }
    ///          
    ///          void globalPluginDeinitialize()
    ///          {
    ///              MyPluginObject::StaticDeinitialize();
    ///          }
    ///      };
    ///
    ///      FB::FactoryBasePtr getFactoryInstance()
    ///      {
    ///          static boost::shared_ptr<PluginFactory> factory = boost::make_shared<PluginFactory>();
    ///          return factory;
    ///      }
    /// @endcode
    /// 
    /// @since 1.3 RC2
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class FactoryBase : public boost::noncopyable
    {
    public:
        FactoryBase();
        virtual ~FactoryBase();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual FB::PluginCorePtr FactoryBase::createPlugin(const std::string& mimetype) = 0
        ///
        /// @brief  Creates a FB::PluginCore derived user plugin object.  This must be implemented
        ///         for all plugin projects.
        ///
        /// @code
        ///      // Example implementation
        ///      FB::PluginCorePtr createPlugin(const std::string& mimetype)
        ///      {
        ///          return boost::make_shared<MyPluginObject>();
        ///      }
        /// @endcode
        /// 
        /// @param mimetype As of 1.3 this is not wired in, in future versions this will be the mimetype
        ///                 that was requested by the object tag that this plugin is being loaded for.  This
        ///                 will allow you to optionally provide multiple plugins in one file.
        /// 
        /// @return FB::PluginCorePtr pointer to the FB::PluginCore class
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::PluginCorePtr           createPlugin(const std::string& mimetype) = 0;

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void FactoryBase::globalPluginInitialize()
        /// @brief  Global plugin initialization
        /// 
        /// This function will be called when the active plugin count goes from 0 to 1.  There is no 
        /// guarantee that it will only be called once in the lifetime of a plugin module, however it will
        /// never be called again until after globalPluginDeinitialize() is called.
        /// 
        /// @code
        ///      void globalPluginInitialize()
        ///      {
        ///          MyPluginObject::StaticInitialize();
        ///      }
        /// @endcode
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void                        globalPluginInitialize();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual void FactoryBase::globalPluginDeinitialize()
        /// @brief  Global plugin deinitialization
        ///
        /// This function will be called when the last plugin is destroyed, taking the active plugin count
        /// from 1 to 0.  There is no guarantee that it will only be called once in the lifetime of a plugin
        /// module because another plugin could be created after this is destroyed and before the module
        /// is unloaded.  However it will not be called again until after globalPluginInitialize() is called.
        /// 
        /// @code
        ///      void globalPluginDeinitialize()
        ///      {
        ///          MyPluginObject::StaticDeinitialize();
        ///      }
        /// @endcode
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual void                        globalPluginDeinitialize();

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn std::string FactoryBase::getPluginName()
        ///
        /// @brief  Returns the name of the plugin.  To change the name of your plugin edit
        ///         PluginConfig.cmake
        ///
        /// @return The plugin name. 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string                         getPluginName();
        std::string                         getPluginName(const std::string& mimetype);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn std::string FactoryBase::getPluginDescription()
        ///
        /// @brief  Returns the description of the plugin.  To change the description of your plugin edit
        ///         PluginConfig.cmake
        ///
        /// @return The plugin description 
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        std::string                         getPluginDescription();
        std::string                         getPluginDescription(const std::string& mimetype);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn FB::Npapi::NpapiPluginPtr FactoryBase::createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host, const std::string& mimetype)
        ///
        /// @brief  Creates a npapi plugin.  The default implementation creates a NpapiPluginWin, NpapiPluginX11, or
        ///         NpapiPluginMac depending on the platform.  Only in very very rare cases will you need to override
        ///         this method.
        ///
        /// @param  host    The BrowserHost object for the browser containing this plugin
        ///
        /// @return An NpapiPluginPtr containing an NpapiPlugin derived plugin handler
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual FB::Npapi::NpapiPluginPtr   createNpapiPlugin(const FB::Npapi::NpapiBrowserHostPtr& host, const std::string& mimetype);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn void getLoggingMethods(FB::Log::LogMethodList& outMethods)
        ///
        /// @brief  Called by the logger to discover which log methods should be used
        ///
        /// @param  outMethods    a LogMethodList instance to hold the result
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        void getLoggingMethods(FB::Log::LogMethodList& outMethods);
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn  FB::Log::LogLevel getLogLevel()
        ///
        /// @brief  Called by the logger to discover what loglevel to use
        ///
        /// @param  outMethods    a LogMethodList instance to hold the result
        /// @since 1.4a3
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        FB::Log::LogLevel getLogLevel();

#ifdef FB_WIN
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowWin* FactoryBase::createPluginWindowWin(const WindowContextWin&)
        ///
        /// @brief  Creates a PluginWindowWin derived plugin window object
        ///
        /// @param  Platform specific data structure for window context information (including the HWND usually) 
        ///
        /// @return A PluginWindowWin* to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowWin*            createPluginWindowWin(const WindowContextWin&);

        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowlessWin* FactoryBase::createPluginWindowless(const WindowContextWin&)
        ///
        /// @brief  Creates a PluginWindowlessWin derived plugin window object
        ///
        /// @param  Platform specific data structure for window context information (including the HDC usually) 
        ///
        /// @return A PluginWindowlessWin* to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowlessWin*        createPluginWindowless(const WindowContextWindowless&);

        virtual IDispatchEx*                createCOMJSObject(FB::BrowserHostPtr host, FB::JSAPIPtr api);
        virtual HRESULT                     UpdateWindowsRegistry( bool install );
#endif
      
#ifdef FB_MACOSX
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowMacCarbonQD* createPluginWindowCarbonQD(const WindowContextQuickDraw&);
        ///
        /// @brief  Creates a PluginWindow derived plugin window object for Carbon / QuickDraw
        ///
        /// @param  Platform specific data structure for window context information
        ///
        /// @return A pointer to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowMacCarbonQD* createPluginWindowCarbonQD(const WindowContextQuickDraw&);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowMacCarbonCG* createPluginWindowCarbonQD(const WindowContextCoreGraphics&);
        ///
        /// @brief  Creates a PluginWindow derived plugin window object for Carbon / CoreGraphics
        ///
        /// @param  Platform specific data structure for window context information
        ///
        /// @return A pointer to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowMacCarbonCG* createPluginWindowCarbonCG(const WindowContextCoreGraphics&);
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowMacCocoaCG* createPluginWindowCocoaCG();
        ///
        /// @brief  Creates a PluginWindow derived plugin window object for Cocoa / CoreGraphics
        ///
        /// @return A pointer to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowMacCocoaCG*  createPluginWindowCocoaCG ();
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowMacCocoaCA* createPluginWindowCocoaCA();
        ///
        /// @brief  Creates a PluginWindow derived plugin window object for Cocoa / CoreGraphics
        ///
        /// @return A pointer to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowMacCocoaCA*  createPluginWindowCocoaCA ();
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowMacCocoaICA* createPluginWindowCarbonICA();
        ///
        /// @brief  Creates a PluginWindow derived plugin window object for Carbon / QuickDraw
        ///
        /// @param  Platform specific data structure for window context information
        ///
        /// @return A pointer to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowMacCocoaICA* createPluginWindowCocoaICA();
#endif
        
#ifdef FB_X11
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        /// @fn virtual PluginWindowX11* createPluginWindowX11(const WindowContextX11&)
        ///
        /// @brief  Creates a PluginWindowX11 derived plugin window object for X11
        ///
        /// @param  Platform specific data structure for window context information
        ///
        /// @return A pointer to the object that should handle the plugin window
        ////////////////////////////////////////////////////////////////////////////////////////////////////
        virtual PluginWindowX11* createPluginWindowX11(const WindowContextX11&); 
#endif
    };
}

#endif
