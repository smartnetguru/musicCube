///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright � 2002-2006, Casey Langen
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
#include "AutoSizingStatic.h"
#include "SliderView.h"

#include <musikCore.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

class TransportTimeView : public CWnd
{
public:

    // construct and destruct
    TransportTimeView();
    virtual ~TransportTimeView();

    // misc
    void OnNewSong();

    // size
    void SetSize(const CSize& size);
    int GetTotalTimeSz(){return m_TotalTime->GetDynSize().cx;}

    // message maps
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg LRESULT OnTrackChange(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTrackBegin(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnTrackFinish(WPARAM wParam, LPARAM lParam);
    afx_msg void OnClicked();

protected:

    // our children...
    SliderView* m_TimeCtrl;
    AutoSizingStatic* m_CurTime;
    AutoSizingStatic* m_TotalTime;
    void RescaleInfo(int cx);
    size_t m_CurChars;
    size_t m_TotalChars;

    // size
    CSize m_Size;

    // we seeking playback time?
    bool m_TimeDrag;

    // toggle for normal timer vs. countdown timer
    bool m_CountDown;

    // timer
    UINT_PTR m_TimerID;

    // macros
    DECLARE_DYNAMIC(TransportTimeView)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

} } // namespace musik::Cube