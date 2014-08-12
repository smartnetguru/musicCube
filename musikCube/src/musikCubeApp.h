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

///////////////////////////////////////////////////

#include "resource.h"

#include "Globals.h"
#include "MainFrameMessages.h"

#include <musikCore.h>

///////////////////////////////////////////////////

#define _CRT_SECURE_NO_DEPRECATE 
#define _SCL_SECURE_NO_DEPRECATE

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

class musikCubeApp : public CWinApp
{
public:

    static CString RelToAbs(CString& path);
    static CString AbsToRel(CString& path);

    // construct and destruct
    musikCubeApp();
    virtual ~musikCubeApp();

    // overrides
    virtual BOOL InitInstance();
    virtual int ExitInstance();

    // message maps
    afx_msg void OnAppAbout();
    static CString GetUserDir();
    static CString GetWorkingDir(bool include_filename = false);

    // here for lack of a better place
    void GetCrossfader(musik::Core::CrossfadeSettings* fader);

    // macros
    DECLARE_MESSAGE_MAP()

private:

    void InitCore();
    void DeinitCore();
    void ReloadLibrary();

    static BOOL shared_IsRunning;
    static HWND shared_MainHwnd;
    ULONG_PTR m_gdiplusToken;
    void Play(const CString& fn);
};

extern musikCubeApp theApp;

///////////////////////////////////////////////////
// TODO: move this somewhere not retarded
///////////////////////////////////////////////////

enum
{
    PBSTRTYPE_NOWPLAYING_TITLE = 0,
    PBSTRTYPE_NOWPLAYING_ALBUM_ARTIST,
    PBSTRTYPE_TASKTRAY,
    PBSTRTYPE_BALLOONTITLE,
    PBSTRTYPE_BALLOONARTISTALBUM
};

CString GetPlaybackStr(int type);

///////////////////////////////////////////////////

} } // namespace musik::Cube