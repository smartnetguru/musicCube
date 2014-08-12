///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright © 2002-2006, Casey Langen
//
// Sources and Binaries of: musikCore musikCube musikBox
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice,
//      this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright 
//      notice, this list of conditions and the following disclaimer in the 
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the author nor the names of other contributors may 
//      be used to endorse or promote products derived from this software 
//      without specific prior written permission. 
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
//
///////////////////////////////////////////////////

#pragma once

#include <3rdparty/iniFile.h>

#include <musikCore.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// MainFramePlugin
///////////////////////////////////////////////////

struct MainFramePlugin
{
public: // typedefs
    
    typedef void    (*SETPLUGINLIBRARY)(musik::Core::Library*);
    typedef void    (*INITIALIZE)(musik::Core::Library*, musik::Core::Player*, HWND);
    typedef void    (*UNLOAD)();
    typedef HMODULE (*GETPLUGININSTANCE)();
    typedef void    (*SETPLUGININSTANCE)(HMODULE);
    typedef void    (*CONFIGURE)();
    typedef void    (*ABOUT)();
    typedef void    (*EXECUTE)();
    typedef void    (*STOP)();
    typedef bool    (*CANCONFIGURE)();
    typedef bool    (*CANABOUT)();
    typedef bool    (*CANEXECUTE)();
    typedef bool    (*CANSTOP)();
    typedef void    (*SETVISIBLEPLAYLIST)(musik::Core::Playlist*);
    typedef void    (*ONSOURCES)();
    typedef bool    (*INSOURCES)();
    typedef bool    (*WANTSPREFS)();
    typedef void    (*SETPREFS)(CIniFile*);
    typedef musik::Core::Functor&       (*GETPLUGINFUNCTOR)();
    typedef const musik::Core::String&  (*GETPLUGINNAME)();
    typedef const musik::Core::String&  (*GETPLUGINDESCRIPTION)();
    typedef int        (*GETPLUGINREVISION)();

    int GetRevision(){ return m_Revision; }
    void SetRevision(int rev){ m_Revision = rev; }

public: // implementation

    CONFIGURE Configure;
    ABOUT About;
    SETPLUGININSTANCE SetPluginInstance;
    GETPLUGININSTANCE GetPluginInstance;
    INITIALIZE Initialize;
    SETPLUGINLIBRARY SetPluginLibrary;
    EXECUTE Execute;
    GETPLUGINNAME GetPluginName;
    GETPLUGINDESCRIPTION GetPluginDescription;
    STOP Stop;
    UNLOAD Unload;
    GETPLUGINFUNCTOR GetPluginFunctor;
    CANCONFIGURE CanConfigure;
    CANABOUT CanAbout;
    CANEXECUTE CanExecute;
    CANSTOP CanStop;
    SETVISIBLEPLAYLIST SetVisiblePlaylist;
    ONSOURCES OnSources;
    INSOURCES InSources;
    WANTSPREFS WantsPrefs;
    SETPREFS SetPrefs;
    GETPLUGINREVISION GetPluginRevision;

    int m_Revision;
};

///////////////////////////////////////////////////

} } // namespace musik::Cube