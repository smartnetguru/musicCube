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

#include "stdafx.h"

#include "musikCubeApp.h"
#include "TransportTimeView.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// TransportTimeView
///////////////////////////////////////////////////

int WM_TRACKCHANGE = RegisterWindowMessage(_T("TRACKCHANGE"));
int WM_TRACKDRAGBEGIN = RegisterWindowMessage(_T("TRACKDRAGBEGIN"));
int WM_TRACKDRAGFINISH = RegisterWindowMessage(_T("TRACKDRAGFINISH"));

#define MUSIK_SEEK_TIMER WM_USER + 1

IMPLEMENT_DYNAMIC(TransportTimeView, CWnd)

BEGIN_MESSAGE_MAP(TransportTimeView, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_TIMER()
    ON_REGISTERED_MESSAGE(WM_TRACKCHANGE, OnTrackChange)
    ON_REGISTERED_MESSAGE(WM_TRACKDRAGBEGIN, OnTrackBegin)
    ON_REGISTERED_MESSAGE(WM_TRACKDRAGFINISH, OnTrackFinish)
    ON_STN_CLICKED(IDC_STATIC_TIMEPLAYED, OnClicked)
END_MESSAGE_MAP()

/*ctor*/ TransportTimeView::TransportTimeView()
{
    m_TimeCtrl = NULL;
    m_CurTime = NULL;
    m_TotalTime = NULL;

    m_CurChars = -1;
    m_TotalChars = -1;

    m_TimeDrag = false;
    m_CountDown = false;

    m_Size = CSize(200, 16);
}

/*dtor*/ TransportTimeView::~TransportTimeView()
{
    if (m_TimeCtrl)
        delete m_TimeCtrl;

    if (m_CurTime)
        delete m_CurTime;
    
    if (m_TotalTime)
        delete m_TotalTime;
}

int TransportTimeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_TimeCtrl = new SliderView();
    if (!m_TimeCtrl->Create(TBS_HORZ | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 123))
        return false;
    
    m_TimeCtrl->SetRange(0, 100);
    m_TimeCtrl->SetLockIfNotPlaying();

    m_CurTime = new AutoSizingStatic();
    if (!m_CurTime->Create(NULL, WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_STATIC_TIMEPLAYED))
        return false;
    m_CurTime->SetDynFont(12, 0, 0);
    m_CurTime->SetDynText(_T("0:00"), false, false);
    
    m_TotalTime = new AutoSizingStatic();
    if (!m_TotalTime->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this))
        return false;
    m_TotalTime->SetDynFont(12, 0, 0);
    m_TotalTime->SetDynText(_T("0:00"), false, false);
    
    m_TimerID = SetTimer(MUSIK_SEEK_TIMER, 500, NULL);

    return 0;
}

void TransportTimeView::OnSize(UINT nType, int cx, int cy)
{
    RescaleInfo(cx);
}

void TransportTimeView::RescaleInfo(int cx)
{
    CRect rcClient;
    GetClientRect(&rcClient);
    
    if (cx == -1)
        cx = rcClient.Width();

    // stuff we'll use
    CRect rcStatic;
    m_CurTime->GetClientRect(&rcStatic);
    CPoint ptCurr;
    CSize szCurr, szUpdate;
    CString sCurTime, sTotalTime;
    size_t text_width = 0;
    size_t track_start = 0;

    // figure out how big to make
    // the windows...
    m_CurTime->GetWindowText(sCurTime);
    m_TotalTime->GetWindowText(sTotalTime);

    if (sCurTime.GetLength() < sTotalTime.GetLength())
        szUpdate = m_TotalTime->GetTextSize(sTotalTime);
    else
        szUpdate = m_CurTime->GetTextSize(sCurTime);

    // current time
    ptCurr.x = 0;
    ptCurr.y = (rcClient.Height() - rcStatic.Height()) / 2;
    m_CurTime->MoveWindow(CRect(ptCurr, szUpdate));

    text_width += szUpdate.cx;
    track_start = text_width;

    // total time
    ptCurr.x = rcClient.Width() - szUpdate.cx;
    m_TotalTime->MoveWindow(CRect(ptCurr, szUpdate));

    text_width += szUpdate.cx;

    // seeker
    ptCurr.x = track_start;
    ptCurr.y = 0;
    szCurr.cx = rcClient.Width() - text_width;
    szCurr.cy = 16;
    m_TimeCtrl->MoveWindow(CRect(ptCurr, szCurr));
}

BOOL TransportTimeView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

void TransportTimeView::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    CMemDC memDC(&dc, &rect);
    
    CRect clip;
    memDC.GetClipBox(&clip);
    memDC.FillSolidRect(clip, Globals::Preferences->GetBtnFaceColor());

    DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

void TransportTimeView::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == MUSIK_SEEK_TIMER)
    {
        if (Globals::Player->IsPlaying() && !Globals::Player->IsPaused() && !m_TimeDrag)
        {
            CString sTimeStr;
            if (!m_CountDown)
                sTimeStr = Globals::Player->GetTimeStr(Globals::Player->GetTimeNow(musik::Core::MUSIK_TIME_MSECS));
            else
            {
                sTimeStr = _T("-");
                sTimeStr += Globals::Player->GetTimeStr(Globals::Player->GetTimeRemain(musik::Core::MUSIK_TIME_MSECS));
            }
            m_TimeCtrl->SetPos(Globals::Player->GetTimeNow(musik::Core::MUSIK_TIME_SECS));
            m_CurTime->SetDynText(sTimeStr, false, false);

            // rescale the window so everything fits
            RescaleInfo(-1);
            m_CurChars = sTimeStr.GetLength();
            m_CurTime->RedrawWindow();
        }
    }
}

void TransportTimeView::OnNewSong()
{
    if (Globals::Player->IsPlaying())
    {
        // reset controls
        CString sTimeStr = Globals::Player->GetTimeStr(Globals::Player->GetDuration(musik::Core::MUSIK_TIME_MSECS));
        m_TotalTime->SetDynText(sTimeStr, false, false);
        if (!m_CountDown)
            m_CurTime->SetDynText(_T("0:00"), true, true);
        else
        {
            sTimeStr = _T("-") + sTimeStr;
            m_CurTime->SetDynText(sTimeStr, true, true);
        }

        m_TimeCtrl->SetPos(0);
        m_TimeCtrl->SetRangeMax(Globals::Player->GetDuration(musik::Core::MUSIK_TIME_SECS));

        // rescale the controls so everything fits
        RescaleInfo(-1);
        m_TotalChars = sTimeStr.GetLength();
        RedrawWindow();

        return;
    }

    else if (!Globals::Player->IsPlaying())
    {
        CString sTimeStr = Globals::Player->GetTimeStr(Globals::Player->GetDuration(musik::Core::MUSIK_TIME_MSECS));
        m_TotalTime->SetDynText(sTimeStr, false, false);

        m_TotalTime->SetDynText(_T("0:00"), false, false);
        m_CurTime->SetDynText(_T("0:00"), false, false);
        m_TimeCtrl->SetPos(0);
        RescaleInfo(-1);
        RedrawWindow();
    }
}

LRESULT TransportTimeView::OnTrackChange(WPARAM wParam, LPARAM lParam)
{
    CString sTimeStr;
    if (!m_CountDown)
        sTimeStr = Globals::Player->GetTimeStr(m_TimeCtrl->GetPos() * 1000);
    else
        sTimeStr = Globals::Player->GetTimeStr(100 - (m_TimeCtrl->GetPos() * 1000));

    m_CurTime->SetDynText(sTimeStr, false, false);

    // avoid unnecessary RescaleInfo() calls
    if (sTimeStr.GetLength() != m_CurChars)
    {
        RescaleInfo(-1);
        m_CurChars = sTimeStr.GetLength();
        m_CurTime->RedrawWindow();
    }    
    
    return 0L;
}

LRESULT TransportTimeView::OnTrackBegin(WPARAM wParam, LPARAM lParam)
{
    m_TimeDrag = true;

    return 0L;
}

LRESULT TransportTimeView::OnTrackFinish(WPARAM wParam, LPARAM lParam)
{
    m_TimeDrag = false;

    if (Globals::Player)
    {
        Globals::Player->SetTimeNow(m_TimeCtrl->GetPos(), musik::Core::MUSIK_TIME_SECS);
    }

    return 0L;
}

void TransportTimeView::OnClicked()
{
    // toggle our timer display state
    m_CountDown = !m_CountDown;

    // force an immediate display update 
    // (otherwise it waits for a second, which is disconcerting)
    OnTimer(MUSIK_SEEK_TIMER);
}

///////////////////////////////////////////////////

} } // namespace musik::Cube