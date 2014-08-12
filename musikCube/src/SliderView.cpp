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
#include <cmath>

#include "SliderView.h"
#include "musikCubeApp.h"
#include "GraphicsUtility.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(SliderView, CSliderCtrl)

BEGIN_MESSAGE_MAP(SliderView, CSliderCtrl)
    ON_WM_ERASEBKGND()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
    ON_WM_CREATE()
    ON_WM_SETFOCUS()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_PAINT()
END_MESSAGE_MAP()

/*ctor*/ SliderView::SliderView()
{
    m_LeftDown = false;
    m_IsCapturing = false;
    m_LockIfNotPlaying = false;
    m_RealtimeUpdate = true;
    m_DrawThumb = true;
    m_LastPos = -1;
    m_Cursor.x = -1;
    m_Cursor.y = -1;
}

/*dtor*/ SliderView::~SliderView()
{
}

void SliderView::OnPaint()
{
    CPaintDC dc(this);

    CRect rect;
    GetClientRect(&rect);

    CMemDC memDC(&dc, &rect);

    DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);

    memDC.ExcludeClipRect(m_ChannelRect);
    memDC.FillSolidRect(
        rect, 
        Globals::Preferences->GetBtnFaceColor()
   );
}

BOOL SliderView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

void SliderView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMCUSTOMDRAW lpcd = (LPNMCUSTOMDRAW)pNMHDR;

    // CDDS_PREPAINT is at the beginning of the paint cycle. 
    // this will fire first, so we tell it we want
    // all item drawing information...
    if (lpcd->dwDrawStage == CDDS_PREPAINT)
    {
        // Request prepaint notifications for each item.
        *pResult = CDRF_NOTIFYITEMDRAW;         
        return;
    }

    // CDDS_ITEMPREPAINT is sent when an item is about
    // to be drawn..
    if (lpcd->dwDrawStage == CDDS_ITEMPREPAINT)
    {    
        CDC *pDC = CDC::FromHandle(lpcd->hdc);
        CRect rect(lpcd->rc);

        // we don't want to draw any ticks,
        // so just skip the default action
        // and return...
        if (lpcd->dwItemSpec == TBCD_TICS)
        {
            ReleaseDC(pDC);

            *pResult = CDRF_SKIPDEFAULT;
            return;
        }

        // drawing the thumb
        else if (lpcd->dwItemSpec == TBCD_THUMB)
        {
            if (m_DrawThumb)
            {
                CMemDC pMemDC(pDC, &rect);

                pMemDC.FillSolidRect(
                    rect,
                    Globals::Preferences->GetBtnHilightColor());

                pMemDC.Draw3dRect(
                    rect,
                    Globals::Preferences->GetBtnShadowColor(),
                    Globals::Preferences->GetBtnShadowColor());
            }
        }

        // drawing the channel, the space
        // below the thumb
        else if (lpcd->dwItemSpec == TBCD_CHANNEL)
        {
            CRect rcThumb;
            GetThumbRect(rcThumb);

            if (rcThumb.Height() > rcThumb.Width())
            {
                rect.top -= 2;
                rect.bottom += 2;
            }
            else
            {
                rect.left -= 2;
                rect.right += 2;
            }

            DrawChannel(pDC, rect, (rcThumb.Height() > rcThumb.Width()));
        }

        else
        {
            int n = 10;
            ++n;
        }

        *pResult = CDRF_SKIPDEFAULT;
        return;
    }

    *pResult = CDRF_SKIPDEFAULT;
}

void SliderView::DrawChannel(CDC* pDC, const CRect& rect, BOOL bHoriz)
{

    CRect rcChannel = rect;

    if (bHoriz)
    {
        rcChannel.top += 1;
        rcChannel.bottom += 1;
        rcChannel.InflateRect(0, 1, 0, 1);
    }
    else
    {
        rcChannel.left += 1;
        rcChannel.right += 1;
        rcChannel.InflateRect(1, 0, 1, 0);
    }

    CMemDC pMemDC(pDC, &rcChannel);

    pMemDC.FillSolidRect(
        rcChannel,
        GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 10));
        
    pMemDC.Draw3dRect(rcChannel, Globals::Preferences->GetBtnShadowColor(), Globals::Preferences->GetBtnShadowColor());

    int nMax = GetRangeMax();
    int nPos = GetPos();
    float fPercent = (float)GetPos() / (float)GetRangeMax();

    if (bHoriz)
    {
        if (nPos > 0)
        {
            float fWidth = (float)(rcChannel.Width() - 4) * fPercent;
            int nWidth = fPercent >= 50.0 ? floor(fWidth) : ceil(fWidth);

            pMemDC->FillSolidRect(
                CRect(
                    rcChannel.left + 2,
                    rcChannel.top + 2,
                    rcChannel.left + nWidth,
                    rcChannel.bottom - 2),
                Globals::Preferences->GetActiveCaptionColor() 
           );
        }
    }
    else
    {
        if (nPos < nMax)
        {
            float fHeight = (float)(rcChannel.Height() - 4) * fPercent;
            int nHeight = fPercent >= 50.0 ? floor(fHeight) : ceil(fHeight);

            pMemDC->FillSolidRect(
                CRect(
                    rcChannel.left + 2,
                    rcChannel.top + nHeight,
                    rcChannel.right - 2,
                    rcChannel.bottom - 2),
                Globals::Preferences->GetActiveCaptionColor() 
           );
        }
    }

    m_ChannelRect = rcChannel;

    Invalidate(false);
}

int SliderView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CSliderCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void SliderView::OnSetFocus(CWnd* pOldWnd)
{
    // this is kludge but it gets the job done...
    // when the tracker is selected, a bounding
    // rectangle with marching ants is drawn
    // around it. to fix it, we set the focus
    // to the control's parent...
    if (GetParent())
        GetParent()->SetFocus();
}

void SliderView::OnMouseMove(UINT nFlags, CPoint point)
{
    if (Globals::Player && m_LockIfNotPlaying && !Globals::Player->IsPlaying())
        return;

    if (nFlags & MK_LBUTTON && m_IsCapturing)
        SetPosFromMouse(m_RealtimeUpdate);
}

void SliderView::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (Globals::Player && m_LockIfNotPlaying && !Globals::Player->IsPlaying())
        return;

    if (!m_IsCapturing)
    {
        m_LeftDown = true;
        SetCapture();
        SetCursor(NULL);

        m_IsCapturing = true;
    }

    GetCursorPos(&m_Cursor);
    SetPosFromMouse(m_RealtimeUpdate);

    OnBeginDrag();
}

void SliderView::OnLButtonUp(UINT nFlags, CPoint point)
{
    if (Globals::Player && m_LockIfNotPlaying && !Globals::Player->IsPlaying())
        return;

    if (m_LeftDown)
    {
        CPoint posCurr;
        GetCursorPos(&posCurr);
        ScreenToClient(&posCurr);

        CRect rcClient;
        GetClientRect(&rcClient);

        // vertical
        if (GetStyle() & TBS_VERT)
        {
            if (posCurr.y >= rcClient.Height())
            {
                ClientToScreen(&rcClient);
                m_Cursor.y = rcClient.bottom;
                SetCursorPos(m_Cursor.x, m_Cursor.y);
            }
            else if (posCurr.y <= 0)
            {
                ClientToScreen(&rcClient);
                m_Cursor.y = rcClient.top;    
                SetCursorPos(m_Cursor.x, m_Cursor.y);
            }
            else
            {
                ClientToScreen(&posCurr);
                SetCursorPos(m_Cursor.x, posCurr.y);
            }
        }

        // horizontal
        else
        {
            if (posCurr.x >= rcClient.Width())
            {
                ClientToScreen(&rcClient);
                m_Cursor.x = rcClient.right;
                SetCursorPos(m_Cursor.x, m_Cursor.y);
            }
            else if (posCurr.x <= 0)
            {
                ClientToScreen(&rcClient);
                m_Cursor.x = rcClient.left;    
                SetCursorPos(m_Cursor.x, m_Cursor.y);
            }
            else
            {
                ClientToScreen(&posCurr);
                SetCursorPos(posCurr.x, m_Cursor.y);
            }
        }

        ReleaseCapture();
        SetCursor(LoadCursor(NULL, IDC_ARROW));
    }

    if (!m_RealtimeUpdate)
        OnPosChanged();

    OnFinishDrag();
    m_LeftDown = false;
    m_IsCapturing = false;
}

void SliderView::SetPosFromMouse(bool call_changed)
{
    CPoint pos;
    GetCursorPos(&pos);
    ScreenToClient(&pos);

    CRect rcClient;
    GetClientRect(&rcClient);

    // vertical
    if (GetStyle() & TBS_VERT)
    {
        if (pos.y >= rcClient.bottom && GetPos() != GetRangeMax())
        {
            SetPos(GetRangeMax());
            m_LastPos = GetRangeMax();

            if (call_changed)
                OnPosChanged();
        }

        else if (pos.y <= rcClient.top && GetPos() != GetRangeMin())
        {
            SetPos(GetRangeMin());
            m_LastPos = GetRangeMin();

            if (call_changed)
                OnPosChanged();
        }
        
        else
        {
            int nLoc = rcClient.Height() - pos.y;
            float fRatio = (float)nLoc / rcClient.Height();
            float nPos = (float)fRatio * (float)GetRangeMax();

            int nFinal = GetRangeMax() - (int)nPos;

            if (nFinal != m_LastPos)
            {
                SetPos(nFinal);
                m_LastPos = nFinal;

                if (call_changed)
                    OnPosChanged();

            }
        }
    }

    // horizontal
    else
    {
        if (pos.x >= rcClient.right && GetPos() != GetRangeMax())
        {
            SetPos(GetRangeMax());
            m_LastPos = GetRangeMax();

            if (call_changed)
                OnPosChanged();
        }

        else if (pos.x <= rcClient.left && GetPos() != GetRangeMin())
        {
            SetPos(GetRangeMin());
            m_LastPos = GetRangeMin();

            if (call_changed)
                OnPosChanged();
        }

        else
        {
            int nLoc = rcClient.Width() - pos.x;
            float fRatio = (float)nLoc / rcClient.Width();
            float nPos = (float)fRatio * (float)GetRangeMax();

            int nFinal = GetRangeMax() - (int)nPos;

            if (nFinal != m_LastPos)
            {
                SetPos(nFinal);
                m_LastPos = nFinal;

                if (call_changed)
                    OnPosChanged();
            }
        }
    }
}

void SliderView::OnPosChanged()
{
    if (GetParent())
    {
        int WM_TRACKCHANGE = RegisterWindowMessage(_T("TRACKCHANGE"));
        GetParent()->SendMessage(WM_TRACKCHANGE, (WPARAM)this, NULL);
    }
}

void SliderView::OnFinishDrag()
{
    if (GetParent())
    {
        int WM_TRACKDRAGFINISH = RegisterWindowMessage(_T("TRACKDRAGFINISH"));
        GetParent()->SendMessage(WM_TRACKDRAGFINISH, (WPARAM)this, NULL);
    }
}

void SliderView::OnBeginDrag()
{
    if (GetParent())
    {
        int WM_TRACKDRAGBEGIN = RegisterWindowMessage(_T("TRACKDRAGBEGIN"));
        GetParent()->SendMessage(WM_TRACKDRAGBEGIN, (WPARAM)this, NULL);
    }
}

void SliderView::SetPos(int nPos)
{
    CSliderCtrl::SetPos(nPos);
    m_LastPos = nPos;
}

///////////////////////////////////////////////////

} } // namespace musik::Cue