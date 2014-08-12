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

#include "musikCubeApp.h"
#include <musikCore.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// PlaylistInfoWorker
///////////////////////////////////////////////////

class PlaylistInfoView;

class PlaylistInfoViewWorker : public musik::Core::Task
{
public:

    void run();
    PlaylistInfoView* m_Parent;
};

///////////////////////////////////////////////////
// PlaylistInfoView
///////////////////////////////////////////////////

#define VIZ_WIDTH           128
#define VIZ_HEIGHT          19
#define MUSIK_VIZ_TIMER     WM_USER + 2

enum PLAYLIST_INFO_VIZ_STYLE
{
    PLAYLIST_INFO_VIZ_STYLE_NONE = 0,
    PLAYLIST_INFO_VIZ_STYLE_EQ,
};

class PlaylistView;

class PlaylistInfoView : public CWnd
{
    // friend
    friend class PlaylistInfoViewWorker;

public:

    // construct and destruct
    PlaylistInfoView(PlaylistView* parent);
    virtual ~PlaylistInfoView();

    // called to update the info string
    void UpdateInfo();

protected:

    // pointer to the list
    PlaylistView* m_ListCtrl;

    // draws the eq bars in the bg
    void DrawEQ(HDC hdc);

    HBITMAP m_hBGBitmap;
    COLORREF *m_pBGBitmapBits;

    // actual string to be printed
    musik::Core::String    m_strInfo;

    // message maps
    afx_msg void OnPaint();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        
    // default font
    CFont m_Font;

    // worker thread
    void UpdateStr(bool is_calc = false);
    PlaylistInfoViewWorker* m_InfoWorker;

    // macros
    DECLARE_DYNAMIC(PlaylistInfoView)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

} } // namespace musik::Cube