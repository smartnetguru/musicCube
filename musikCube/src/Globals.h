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

#include "Preferences.h"
#include "MainFrameFunctor.h"
#include "MainFramePlugin.h"

#include <musikCore.h>

///////////////////////////////////////////////////

using musik::Cube::Preferences;
using musik::Cube::MainFrameFunctor;
using musik::Cube::MainFramePlugin;

///////////////////////////////////////////////////

namespace Globals {

///////////////////////////////////////////////////

typedef std::vector<MainFramePlugin> MainFramePluginList;

extern musik::Core::Player*             Player;
extern musik::Core::Library*            Library;
extern musik::Core::Library*            MainLibrary;
extern musik::Core::Library*            Collected;
extern musik::Cube::Preferences*        Preferences;
extern musik::Cube::MainFrameFunctor*   Functor;
extern bool                             DrawGraphics;
extern MainFramePluginList              Plugins;

extern void SetMainLibrary(musik::Core::Library* library);
extern void LoadMainFramePlugins();
extern void UnloadMainFramePlugins();
extern void SynchronizeDirs(musik::Core::Library* library);

///////////////////////////////////////////////////

}   // namespace Globals