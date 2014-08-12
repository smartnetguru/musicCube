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

#include "stdafx.h"

#include "musikCubeApp.h"
#include "EqualizerView.h"

#include "3rdparty/memdc.h"

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// EqualizerViewContainer
///////////////////////////////////////////////////

int WM_CLOSEEQUALIZERPRESETS = RegisterWindowMessage(_T("CLOSEEQUALIZERPRESETS"));

IMPLEMENT_DYNAMIC(EqualizerViewContainer, DockableWindow)

BEGIN_MESSAGE_MAP(EqualizerViewContainer, DockableWindow)
    // mfc maps
    ON_WM_CREATE()
    ON_WM_SIZE()

    // custom messages
    ON_REGISTERED_MESSAGE(WM_CLOSEEQUALIZERPRESETS, OnClosePresets)

    // menu commands
    ON_COMMAND(ID_EQUALIZER_PRESETS, OnEqualizerPresets)
    ON_COMMAND(ID_EQUALIZER_STATE_18BAND, OnEqualizerState18band)
    ON_COMMAND(ID_EQUALIZER_STATE_6BAND, OnEqualizerState6band)
    ON_COMMAND(ID_EQUALIZER_SET_AS_DEFAULT, OnEqualizerSetAsDefault)
    ON_COMMAND(ID_EQUALIZER_RESETDEFAULT, OnEqualizerResetdefault)
    ON_COMMAND(ID_EQUALIZER_USE_GLOBAL, OnEqualizerAutomaticallySave)
    ON_COMMAND(ID_EQUALIZER_CLOSE_VIEW, OnEqualizerCloseView)
END_MESSAGE_MAP()

/*ctor*/ EqualizerViewContainer::EqualizerViewContainer()
    : DockableWindow()
{
    m_wndChild = new EqualizerView();
    m_Presets = NULL;
}

/*dtor*/ EqualizerViewContainer::~EqualizerViewContainer()
{
    OnClosePresets(NULL, NULL);
    delete m_wndChild;
}

int EqualizerViewContainer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (DockableWindow::OnCreate(lpCreateStruct) == -1)
        return -1;

    // dock bar
    SetBarStyle(GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    EnableDocking(CBRS_ALIGN_ANY);

    // child
     musik::Core::SetEQPreampLevel(Globals::Preferences->GetEqualizerMultiplier());
    if (!m_wndChild->Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, CRect(0, 0, 0, 0), this, 123))
        return -1;

    if (!m_Font.CreateStockObject(DEFAULT_GUI_FONT))
        return -1;

    m_wndChild->SetFont(&m_Font);

    return 0;
}

void EqualizerViewContainer::OnOptions()
{
    CPoint pos;
    ::GetCursorPos(&pos);

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_EQUALIZER_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // update ui does not work becuase the
    // menu does not belong to a CView or
    // CFrameWnd inherited class, so this
    // updates the menu items...
    popup_menu->EnableMenuItem(ID_EQUALIZER_PRESETS, (m_Presets == 0) ? MF_ENABLED : MF_GRAYED | MF_DISABLED);
    popup_menu->CheckMenuItem(ID_EQUALIZER_USE_GLOBAL, (Globals::Preferences->EqualizerUseGlobal() == true) ? MF_CHECKED : MF_UNCHECKED);
    popup_menu->CheckMenuItem(ID_EQUALIZER_ENABLED2, (Globals::Preferences->IsEqualizerEnabled() == true) ? MF_CHECKED : MF_UNCHECKED);

    int band_state = GetCtrl()->GetBandState();
    if (band_state == MUSIK_EQUALIZER_CTRL_18BANDS)
        popup_menu->CheckMenuItem(ID_EQUALIZER_STATE_18BAND, MF_CHECKED);
    else if (band_state == MUSIK_EQUALIZER_CTRL_6BANDS)
        popup_menu->CheckMenuItem(ID_EQUALIZER_STATE_6BAND, MF_CHECKED);
    else
        ASSERT(0);

    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
} 

void EqualizerViewContainer::OnEqualizerPresets()
{
    if (!m_Presets)
    {
        m_Presets = new EqualizerPresetsDialog(this);
        m_Presets->Create(IDD_EQUALIZER_SETS, this);
        m_Presets->ShowWindow(SW_SHOW);
    }    
}

LRESULT EqualizerViewContainer::OnClosePresets(WPARAM wParam, LPARAM lParam)
{
    if (m_Presets)
    {
        delete m_Presets;
        m_Presets = NULL;
    }

    return 0L;
}

void EqualizerViewContainer::OnEqualizerState18band()
{
    GetCtrl()->SetBandState(MUSIK_EQUALIZER_CTRL_18BANDS);
    GetCtrl()->LayoutNewState();
    GetCtrl()->DisplayChanged();

    Globals::Preferences->SetEqualizerBandState(MUSIK_EQUALIZER_CTRL_18BANDS);
}

void EqualizerViewContainer::OnEqualizerState6band()
{
    GetCtrl()->SetBandState(MUSIK_EQUALIZER_CTRL_6BANDS);
    GetCtrl()->LayoutNewState();
    GetCtrl()->DisplayChanged();

    Globals::Preferences->SetEqualizerBandState(MUSIK_EQUALIZER_CTRL_6BANDS);
}

void EqualizerViewContainer::OnEqualizerSetAsDefault()
{
    GetCtrl()->SetAsDefault();
}

void EqualizerViewContainer::OnEqualizerResetdefault()
{
    musik::Core::EqualizerSettings settings;
    GetCtrl()->ResetDefault();
}

void EqualizerViewContainer::OnEqualizerAutomaticallySave()
{
    if (Globals::Preferences->EqualizerUseGlobal())
        Globals::Preferences->SetEqualizerUseGlobal(false);
    else
        Globals::Preferences->SetEqualizerUseGlobal(true);

    Globals::Player->UseGlobalEQSettings(Globals::Preferences->EqualizerUseGlobal());
}

void EqualizerViewContainer::OnEqualizerCloseView()
{
    CFrameWnd* parent = (CFrameWnd*)GetParent();
    parent->ShowControlBar(this, FALSE, FALSE);
}

///////////////////////////////////////////////////
// EqualizerSlider
///////////////////////////////////////////////////

EqualizerSlider::EqualizerSlider()
: SliderView()
{
    m_RealtimeUpdate = false;
}

void EqualizerSlider::OnPosChanged()
{
}

///////////////////////////////////////////////////
// EqualizerView
///////////////////////////////////////////////////

int WM_BANDCHANGE = RegisterWindowMessage(_T("TRACKDRAGFINISH"));

IMPLEMENT_DYNAMIC(EqualizerView, CWnd)

BEGIN_MESSAGE_MAP(EqualizerView, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_REGISTERED_MESSAGE(WM_BANDCHANGE, OnBandChange)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void EqualizerView::SendEQToPlayer()
{
    musik::Core::EqualizerSettings& settings = Globals::Player->GetEQSettings();
    BandsToEQSettings(&settings);

    if (Globals::Preferences->EqualizerUseGlobal())
        Globals::MainLibrary->UpdateDefaultEqualizer(settings);

    Globals::Player->UpdateEQSettings(true, false);
}

EqualizerView::EqualizerView()
{
    for (int i = 0; i < 19; i++)
        m_LeftBands[i].m_Parent = this;

    m_BandState = MUSIK_EQUALIZER_CTRL_18BANDS;
    m_FullRange = 96;
}

EqualizerView::~EqualizerView()
{
}

void EqualizerView::OnPaint()
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

BOOL EqualizerView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

void EqualizerView::OnAlwaysOnTop()
{
    ::SetWindowPos(m_Tips.GetSafeHwnd(), 
        HWND_TOPMOST, 
        0, 
        0, 
        0, 
        0, 
        NULL);
}

int EqualizerView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // 
    // small font
    //
    CDC* pDC = GetDC();
    int ppi = pDC->GetDeviceCaps(LOGPIXELSX);
    int pointsize = MulDiv(65, 96, ppi);
    m_Font.CreatePointFont(pointsize, _T(""));
    ReleaseDC(pDC);

    CString sTemp;
    m_Tips.Create(this, TTS_ALWAYSTIP);

    //
    // bands
    //
    for (size_t i = 0; i < 18; i++)
    {
        m_LeftBands[i].Create(WS_CHILD | WS_VISIBLE | TBS_VERT, CRect(0, 0, 0, 0), this, 123);
        m_LeftBands[i].SetRange(0, m_FullRange);
        m_LeftBands[i].SetPos(m_FullRange / 2);

        m_Labels[i].Create(
            CString(IntToString(musik::Core::GetEQFreqs()[i]).c_str()),
            WS_CHILD | WS_VISIBLE | SS_CENTER,
            CRect(0, 0, 0, 0),
            this,
            123);
        m_Labels[i].SetFont(&m_Font);

        sTemp.Format(_T("left: %d hz"), musik::Core::GetEQFreqs()[i]);
        m_Tips.AddTool(&m_LeftBands[i], sTemp);
    }    

    //
    // preamp
    //
    m_LeftBands[18].Create(WS_CHILD | WS_VISIBLE | TBS_VERT, CRect(0, 0, 0, 0), this, 123);
    m_LeftBands[18].SetRange(0, m_FullRange);
    m_LeftBands[18].SetPos(Globals::Preferences->GetEqualizerMultiplier());

    m_Labels[18].Create(
        _T("pre"),
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        CRect(0, 0, 0, 0),
        this,
        123);
    m_Labels[18].SetFont(&m_Font);

    // makes sure the tooltips are always displayed, even
    // if the main window is specified as always on top
    ::SetWindowPos(m_Tips.GetSafeHwnd(), 
        HWND_TOPMOST, 
        0, 
        0, 
        0, 
        0, 
        NULL);

    LoadDefault();

    m_BandState = Globals::Preferences->GetEqualizerBandState();
    LayoutNewState();
    SetBandsFrom(Globals::Player->GetEQSettings());

    return 0;
}

void EqualizerView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    int width_remaining = ((cx - (16 * (GetBandState() + 1))) / (GetBandState() + 1));

    CRect curr;
    int space;
    if (GetBandState() == MUSIK_EQUALIZER_CTRL_18BANDS)
    {
        // preamp
        int i = 18;
        space = (width_remaining / 2);
        curr.top = 12;
        curr.bottom = cy;
        curr.left = space;
        curr.right = curr.left + 16;
        m_LeftBands[18].MoveWindow(curr);
        curr.left = curr.left - 8;
        curr.top = 2;
        curr.bottom = 12;
        curr.right = curr.right + 6;
        m_Labels[18].MoveWindow(curr);

        int j = 0;
        for (int i = 0; i < 18; i++)
        {
            j++;
            space = (width_remaining / 2) + (j * width_remaining);

            curr.top = 12;
            curr.bottom = cy;
            curr.left = (j * 16) + space;
            curr.right = curr.left + 16;

            m_LeftBands[i].MoveWindow(curr);

            curr.left = curr.left - 8;
            curr.top = 2;
            curr.bottom = 12;
            curr.right = curr.right + 6;
            m_Labels[i].MoveWindow(curr);
        }
    }

    else if (GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS)
    {
        int band_at;
        for (size_t i = 0; i < 7; i++)
        {
            switch(i)
            {
            case 0:
                band_at = 18;
                break;
            case 1:
                band_at = 0;
                break;
            case 2:
                band_at = 3;
                break;
            case 3:
                band_at = 7;
                break;
            case 4: 
                band_at = 10;
                break;
            case 5:
                band_at = 14;
                break;
            case 6:
                band_at = 17;
                break;
            }

            space = (width_remaining / 2) + (i * width_remaining);

            curr.top = 12;
            curr.bottom = cy;
            curr.left = ((i) * 16) + space;
            curr.right = curr.left + 16;

            m_LeftBands[band_at].MoveWindow(curr);

            curr.left = curr.left - 8;
            curr.top = 2;
            curr.bottom = 12;
            curr.right = curr.right + 6;
            m_Labels[band_at].MoveWindow(curr);
        }
    }
}

void EqualizerView::LayoutNewState()
{
    CRect rcClient;
    GetClientRect(&rcClient);
    OnSize(NULL, rcClient.Width(), rcClient.Height());

    if (GetBandState() == MUSIK_EQUALIZER_CTRL_18BANDS)
    {
        SetBandsFrom(Globals::Player->GetEQSettings());    // we may have interpolated, so reload
        for (size_t i = 0; i < 18; i++)
        {
            m_LeftBands[i].ShowWindow(SW_SHOW);
            m_Labels[i].ShowWindow(SW_SHOW);
        }
    }

    else if (GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS)
    {
        m_LeftBands[0].ShowWindow(SW_SHOW);
        m_LeftBands[1].ShowWindow(SW_HIDE);
        m_LeftBands[2].ShowWindow(SW_HIDE);
        m_LeftBands[3].ShowWindow(SW_SHOW);
        m_LeftBands[4].ShowWindow(SW_HIDE);
        m_LeftBands[5].ShowWindow(SW_HIDE);
        m_LeftBands[6].ShowWindow(SW_HIDE);
        m_LeftBands[7].ShowWindow(SW_SHOW);
        m_LeftBands[8].ShowWindow(SW_HIDE);
        m_LeftBands[9].ShowWindow(SW_HIDE);
        m_LeftBands[10].ShowWindow(SW_SHOW);
        m_LeftBands[11].ShowWindow(SW_HIDE);
        m_LeftBands[12].ShowWindow(SW_HIDE);
        m_LeftBands[13].ShowWindow(SW_HIDE);
        m_LeftBands[14].ShowWindow(SW_SHOW);
        m_LeftBands[15].ShowWindow(SW_HIDE);
        m_LeftBands[16].ShowWindow(SW_HIDE);
        m_LeftBands[17].ShowWindow(SW_SHOW);

        m_Labels[0].ShowWindow(SW_SHOW);
        m_Labels[1].ShowWindow(SW_HIDE);
        m_Labels[2].ShowWindow(SW_HIDE);
        m_Labels[3].ShowWindow(SW_SHOW);
        m_Labels[4].ShowWindow(SW_HIDE);
        m_Labels[5].ShowWindow(SW_HIDE);
        m_Labels[6].ShowWindow(SW_HIDE);
        m_Labels[7].ShowWindow(SW_SHOW);
        m_Labels[8].ShowWindow(SW_HIDE);
        m_Labels[9].ShowWindow(SW_HIDE);
        m_Labels[10].ShowWindow(SW_SHOW);
        m_Labels[11].ShowWindow(SW_HIDE);
        m_Labels[12].ShowWindow(SW_HIDE);
        m_Labels[13].ShowWindow(SW_HIDE);
        m_Labels[14].ShowWindow(SW_SHOW);
        m_Labels[15].ShowWindow(SW_HIDE);
        m_Labels[16].ShowWindow(SW_HIDE);
        m_Labels[17].ShowWindow(SW_SHOW);
    }

    m_LeftBands[18].ShowWindow(SW_SHOW);
    m_Labels[18].ShowWindow(SW_SHOW);

    for (size_t i = 0; i < 19; i++)
    {
        m_LeftBands[i].RedrawWindow();
        m_Labels[i].RedrawWindow();
    }

    Invalidate();
}

void EqualizerView::SetBandState(int state)
{
    m_BandState = state;
}

void EqualizerView::LoadCurrSong()
{
    if (Globals::Player->IsPlaying() && Globals::Player->IsEqualizerEnabled())
        SetBandsFrom(Globals::Player->GetEQSettings());
}

void EqualizerView::LoadDefault()
{
    if (Globals::Player->IsEqualizerEnabled())
    {
        Globals::MainLibrary->GetDefaultEqualizer(Globals::Player->GetEQSettings());
        Globals::Player->UpdateEQSettings();
        SetBandsFrom(Globals::Player->GetEQSettings());
    }
}

void EqualizerView::SetBandsFrom(const musik::Core::EqualizerSettings& settings)
{
    int left = 0;
    for (size_t i = 0; i < 18; i++)
        m_LeftBands[i].SetPos(settings.m_Left[i]);

    Invalidate();
}

void EqualizerView::BandsToEQSettings(musik::Core::EqualizerSettings* settings)
{
    if (!settings)
        return;

    float left_chan[18];

    for (size_t i = 0; i < 18; i++)
        left_chan[i] = m_LeftBands[i].GetPos();

    // must interpolate
    if (GetBandState() == MUSIK_EQUALIZER_CTRL_6BANDS)
    {
        float chan_set_l[6];

        chan_set_l[0] = (float)m_LeftBands[0].GetPos();
        chan_set_l[1] = (float)m_LeftBands[3].GetPos();
        chan_set_l[2] = (float)m_LeftBands[7].GetPos();
        chan_set_l[3] = (float)m_LeftBands[10].GetPos();
        chan_set_l[4] = (float)m_LeftBands[14].GetPos();
        chan_set_l[5] = (float)m_LeftBands[17].GetPos();
        
        // interpolate the 12 missing bands
        left_chan[0]    = chan_set_l[0];
        left_chan[1]    = left_chan[0]  + ((chan_set_l[1] - chan_set_l[0]) / 4.0f);
        left_chan[2]    = left_chan[1]  + ((chan_set_l[1] - chan_set_l[0]) / 4.0f);
        left_chan[3]    = chan_set_l[1];
        left_chan[4]    = left_chan[3]  + ((chan_set_l[2] - chan_set_l[1]) / 6.0f);
        left_chan[5]    = left_chan[4]  + ((chan_set_l[2] - chan_set_l[1]) / 6.0f);
        left_chan[6]    = left_chan[5]  + ((chan_set_l[2] - chan_set_l[1]) / 6.0f);
        left_chan[7]    = chan_set_l[2];
        left_chan[8]    = left_chan[7]  + ((chan_set_l[3] - chan_set_l[2]) / 4.0f);
        left_chan[9]    = left_chan[8]  + ((chan_set_l[3] - chan_set_l[2]) / 4.0f);
        left_chan[10]    = chan_set_l[3];
        left_chan[11]    = left_chan[10] + ((chan_set_l[4] - chan_set_l[3]) / 6.0f);
        left_chan[12]    = left_chan[11] + ((chan_set_l[4] - chan_set_l[3]) / 6.0f);
        left_chan[13]    = left_chan[12] + ((chan_set_l[4] - chan_set_l[3]) / 6.0f);
        left_chan[14]    = chan_set_l[4];
        left_chan[15]    = left_chan[14] + ((chan_set_l[5] - chan_set_l[4]) / 4.0f);
        left_chan[16]    = left_chan[15] + ((chan_set_l[5] - chan_set_l[4]) / 4.0f);
        left_chan[17]    = chan_set_l[5];
    }

    settings->Set(musik::Core::EqualizerBandLeft, left_chan);
    settings->Set(musik::Core::EqualizerBandRight, left_chan);

    musik::Core::SetEQPreampLevel(m_LeftBands[18].GetPos());
    Globals::Preferences->SetEqualizerMultiplier(m_LeftBands[18].GetPos());
}

LRESULT EqualizerView::OnBandChange(WPARAM wParam, LPARAM lParam)
{
    SendEQToPlayer();
    return 0L;
}

void EqualizerView::SetAsDefault()
{
    musik::Core::EqualizerSettings curr_eq;
    BandsToEQSettings(&curr_eq);
    Globals::MainLibrary->UpdateDefaultEqualizer(curr_eq);
}

void EqualizerView::DisplayChanged()
{
    if (Globals::Player->IsEqualizerEnabled())
        SetBandsFrom(Globals::Player->GetEQSettings());
}

void EqualizerView::ResetDefault()
{
    // get default
    musik::Core::EqualizerSettings reset;
    SetBandsFrom(reset);

    // send to player
    SendEQToPlayer();
}

BOOL EqualizerView::PreTranslateMessage(MSG* pMsg)
{
    m_Tips.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////

} } // namespace musik::Cube