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

#include "TransportVolumeView.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

int WM_NOWPLAYING_VOLUME_CHANGED = RegisterWindowMessage(_T("WM_NOWPLAYING_VOLUME_CHANGED"));

BEGIN_MESSAGE_MAP(TransportVolumeSlider, SliderView)
    ON_WM_CREATE()
END_MESSAGE_MAP()

/*ctor*/ TransportVolumeSlider::TransportVolumeSlider()
    : SliderView()
{
}

/*dtor*/ TransportVolumeSlider::~TransportVolumeSlider()
{
}

int TransportVolumeSlider::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (SliderView::OnCreate(lpCreateStruct) == -1)
        return -1;

    SetRange(0, 100);
    UpdateVolume();

    return 0;
}

void TransportVolumeSlider::UpdateVolume()
{
    if (Globals::Player)
        SetPos((int)ceil((((float)Globals::Player->GetMaxVolume() / 255.0) * 100.0)));
}

void TransportVolumeSlider::OnPosChanged()
{
    if (Globals::Player)
    {
        Globals::Player->SetMaxVolume((int)((float)GetPos() * 2.55), true);
    }

    int n = GetPos();

    if (Globals::Preferences)
        Globals::Preferences->SetPlayerVolume((int)((float)GetPos() * 2.55));

    GetParent()->PostMessage(WM_NOWPLAYING_VOLUME_CHANGED);
}

///////////////////////////////////////////////////
// TransportVolumeView
///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(TransportVolumeView, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_REGISTERED_MESSAGE(WM_NOWPLAYING_VOLUME_CHANGED, OnVolumeChanged)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

TransportVolumeView::~TransportVolumeView()
{
    delete m_Caption;
    delete m_VolumeLabel;
    delete m_VolumeCtrl;
    m_VolumeCtrl = NULL;
    m_VolumeLabel = NULL;
}

LRESULT TransportVolumeView::OnVolumeChanged(WPARAM wParam, LPARAM lParam)
{
    wchar_t volume[5];
    wsprintf(volume, _T("%3d"), (int)ceil((float)(((float)Globals::Player->GetMaxVolume() / 255.0) * 100.0)));

    m_VolumeLabel->SetDynText(volume, false, false);

    return 0;
}

int TransportVolumeView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // create slider
    m_VolumeCtrl = new TransportVolumeSlider();
    if (!m_VolumeCtrl->Create(TBS_HORZ | TBS_NOTICKS | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 123))
        return -1;
    m_VolumeCtrl->SetLockIfNotPlaying(false);

    // create label
    m_VolumeLabel = new AutoSizingStatic();
    m_VolumeLabel->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
    m_VolumeLabel->SetDynText(_T("0"));
    m_VolumeLabel->SetDynFont(12, 0, 0);
    //
    mLabelWidth = m_VolumeLabel->GetDynSize().cx;

    // create caption
    m_Caption = new AutoSizingStatic();
    m_Caption->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
    m_Caption->SetDynText(_T("vol:"));
    m_Caption->SetDynFont(12, 0, 0);
    
    OnVolumeChanged(0, 0);
    m_VolumeCtrl->UpdateVolume();

    return 0;
}

void TransportVolumeView::SetLabelWidth(int inWidth)
{
    mLabelWidth = inWidth;

    // fake size event
    CRect clientRect;
    this->GetClientRect(&clientRect);
    this->OnSize(NULL, clientRect.Width(), clientRect.Height());
}

void TransportVolumeView::UpdateVolume()
{
    m_VolumeCtrl->UpdateVolume();
    this->OnVolumeChanged(NULL, NULL);
}

void TransportVolumeView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    CRect clientRect;
    this->GetClientRect(clientRect);

    // caption
    CRect captionRect(
        CPoint(
            0,    // x
            0    // y
       ),
        CSize(
            m_Caption->GetDynSize().cx,                    // width
            m_Caption->GetDynSize().cy                    // height
       )
   );

    m_VolumeCtrl->MoveWindow(captionRect);

    // slider
    CRect sliderRect(
        CPoint(
            captionRect.Width() + 2,    // x
            0                            // y
       ),
        CSize(
            clientRect.Width() - mLabelWidth -    // width
                 captionRect.Width() - 2,        // width
            16                                    // height
       )
   );

    m_VolumeCtrl->MoveWindow(sliderRect);

    // readout
    CRect labelRect(
        CPoint(
            clientRect.right - mLabelWidth,                        // x
            0                                                    // y
       ),
        CSize(
            mLabelWidth,                                        // width
            m_VolumeLabel->GetDynSize().cy                        // height
       )
   );

    m_VolumeLabel->MoveWindow(labelRect);
}

void TransportVolumeView::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    CMemDC memDC(&dc, &rect);

    memDC.FillSolidRect(rect, Globals::Preferences->GetBtnFaceColor());
}

BOOL TransportVolumeView::OnEraseBkgnd(CDC* pDC)
{
    return true;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube