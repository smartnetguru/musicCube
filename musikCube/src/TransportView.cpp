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
#include "TransportView.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// TransportViewContainer
///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(TransportViewContainer, DockableWindow)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_PAINT()
END_MESSAGE_MAP()

/*ctor*/ TransportViewContainer::TransportViewContainer(CFrameWnd* parent)
    : DockableWindow()
{
    m_wndChild = new TransportView(parent);
}

/*dtor*/ TransportViewContainer::~TransportViewContainer()
{
    delete m_wndChild;
}

int TransportViewContainer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (DockableWindow::OnCreate(lpCreateStruct) == -1)
        return -1;

    // dock bar
    SetBarStyle(GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    EnableDocking(CBRS_ALIGN_BOTTOM | CBRS_ALIGN_TOP);

    ShowGripper(false);

    // child
    if (!m_wndChild->Create(NULL, NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 123))
        return -1;

    if (!m_Font.CreateStockObject(DEFAULT_GUI_FONT))
        return -1;

    m_wndChild->SetFont(&m_Font);
    
    return 0;
}

///////////////////////////////////////////////////
// TransportView
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(TransportView, CWnd)

BEGIN_MESSAGE_MAP(TransportView, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_SYSCOLORCHANGE()
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_BTN_PREV, OnBtnPrev)
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_BTN_PLAY, OnBtnPlay)
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_BTN_NEXT, OnBtnNext)
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_BTN_STOP, OnBtnStop)
    ON_COMMAND(ID_PLAYRANDOM_ARTIST, OnPlayrandomArtist)
    ON_COMMAND(ID_PLAYRANDOM_ALBUM, OnPlayrandomAlbum)
    ON_COMMAND(ID_PLAYRANDOM_GENRE, OnPlayrandomGenre)
    ON_COMMAND(ID_PLAYRANDOM_YEAR, OnPlayrandomYear)
    ON_COMMAND(ID_QUEUERANDOM_ARTIST, OnQueuerandomArtist)
    ON_COMMAND(ID_QUEUERANDOM_ALBUM, OnQueuerandomAlbum)
    ON_COMMAND(ID_QUEUERANDOM_GENRE, OnQueuerandomGenre)
    ON_COMMAND(ID_QUEUERANDOM_YEAR, OnQueuerandomYear)
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_ENB_CROSSFADER, OnEnbCrossfader)
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_ENB_EQUALIZER, OnEnbEqualizer)
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_ENB_RANDOM, OnEnbRandom)
    ON_BN_CLICKED(MUSIK_TRANSPORTVIEW_ENB_REPEATPLAYLIST, OnEnbRepeat)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/*ctor*/ TransportView::TransportView(CFrameWnd* parent)
{
    m_pPrevOff = m_pPrevOn = NULL;
    m_pPlayOff = m_pPlayOn = NULL;
    m_pPauseOff = m_pPauseOn = NULL;
    m_pStopOff = m_pStopOn = NULL;
    m_pNextOff = m_pNextOn = NULL;
    m_pCfOff = m_pCfOn = NULL;
    m_pEqOff = m_pEqOn = NULL;
    m_pRepeatOff = m_pRepeatOn = NULL;
    m_pRandomOff = m_pRandomOn = NULL;

    m_MainWnd = parent;
}

/*dtor*/ TransportView::~TransportView()
{
    delete m_Volume;
    delete m_Track;

    delete m_Play;
    delete m_Prev;
    delete m_Next;
    delete m_Stop;

    delete m_EnbCrossfader;
    delete m_EnbEqualizer;
    delete m_EnbRandom;
    delete m_EnbRepeatPlaylist;

    UnloadBitmaps();
}

void TransportView::OnSysColorChange()
{
    this->LoadBitmaps();
    this->SetButtonBitmaps();
}

void TransportView::LoadBitmaps()
{
    UnloadBitmaps();

    musikCubeApp* ptrApp = (musikCubeApp*)AfxGetApp();

    std::wstring strType;
    if (Globals::Preferences->UseLightIconTheme())
    {
        strType = _T("light");
    }
    else
    {
        strType = _T("dark");
    }

    m_pPrevOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\prev_") + strType.c_str() + _T("_uf.png"));
    m_pPrevOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\prev_") + strType.c_str() + _T("_f.png"));

    m_pNextOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\next_") + strType.c_str() + _T("_uf.png"));
    m_pNextOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\next_") + strType.c_str() + _T("_f.png"));

    m_pStopOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\stop_") + strType.c_str() + _T("_uf.png"));
    m_pStopOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\stop_") + strType.c_str() + _T("_f.png"));

    m_pCfOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\cf_") + strType.c_str() + _T("_uf.png"));
    m_pCfOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\cf_") + strType.c_str() + _T("_f.png"));

    m_pEqOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\eq_") + strType.c_str() + _T("_uf.png"));
    m_pEqOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\eq_") + strType.c_str() + _T("_f.png"));

    m_pRepeatOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\repeat_") + strType.c_str() + _T("_uf.png"));
    m_pRepeatOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\repeat_") + strType.c_str() + _T("_f.png"));

    m_pRandomOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\random_") + strType.c_str() + _T("_uf.png"));
    m_pRandomOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\random_") + strType.c_str() + _T("_f.png"));

    m_pPlayOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\play_") + strType.c_str() + _T("_uf.png"));
    m_pPlayOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\play_") + strType.c_str() + _T("_f.png"));

    m_pPauseOff = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\pause_") + strType.c_str() + _T("_uf.png"));
    m_pPauseOn = Gdiplus::Bitmap::FromFile(
        ptrApp->GetWorkingDir() + _T("\\img\\pause_") + strType.c_str() + _T("_f.png"));
}

void TransportView::UnloadBitmaps()
{
    delete m_pPrevOff;
    delete m_pPrevOn;
    delete m_pNextOff;
    delete m_pNextOn;
    delete m_pStopOff;
    delete m_pStopOn;
    delete m_pCfOff;
    delete m_pCfOn;
    delete m_pEqOff;
    delete m_pEqOn;
    delete m_pRepeatOff;
    delete m_pRepeatOn;
    delete m_pRandomOff;
    delete m_pRandomOn;
    delete m_pPlayOff;
    delete m_pPlayOn;
    delete m_pPauseOff;
    delete m_pPauseOn;
}

void TransportView::SetButtonBitmaps()
{
    m_EnbCrossfader->SetBitmaps(m_pCfOn, m_pCfOff);
    m_EnbEqualizer->SetBitmaps(m_pEqOn, m_pEqOff);
    m_EnbRandom->SetBitmaps(m_pRandomOn, m_pRandomOff);
    m_EnbRepeatPlaylist->SetBitmaps(m_pRepeatOn, m_pRepeatOff);
    m_Prev->SetBitmaps(m_pPrevOn, m_pPrevOff);
    m_Next->SetBitmaps(m_pNextOn, m_pNextOff);
    m_Stop->SetBitmaps( m_pStopOn, m_pStopOff);

    this->SetPlayingButton();
}

void TransportView::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    CMemDC memDC(&dc, &rect);

    memDC.FillSolidRect(rect, Globals::Preferences->GetBtnFaceColor());
}

int TransportView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rcClient;
    GetClientRect(&rcClient);

    m_Tips.Create(this, TTS_ALWAYSTIP);
    m_Font.CreateStockObject(DEFAULT_GUI_FONT);

    LoadBitmaps();

    m_EnbCrossfader = new FlatToggleButton();
    if (!m_EnbCrossfader->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_ENB_CROSSFADER))
        return -1;
    m_EnbCrossfader->SetFont(&m_Font);
    m_Tips.AddTool(m_EnbCrossfader, _T("CrossfadeSettings"));
    if (!Globals::DrawGraphics)
        m_EnbCrossfader->SetWindowText(_T("Fade"));

    m_EnbEqualizer = new FlatToggleButton();
    if (!m_EnbEqualizer->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_ENB_EQUALIZER))
        return -1;
    m_EnbEqualizer->SetFont(&m_Font);
    m_Tips.AddTool(m_EnbEqualizer, _T("Equalizer"));
    if (!Globals::DrawGraphics)
        m_EnbEqualizer->SetWindowText(_T("EQ"));

    m_EnbRandom = new FlatToggleButton();
    if (!m_EnbRandom->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_ENB_RANDOM))
        return -1;
    m_EnbRandom->SetFont(&m_Font);
    m_Tips.AddTool(m_EnbRandom, _T("Random Playback"));
    if (!Globals::DrawGraphics)
        m_EnbRandom->SetWindowText(_T("Random"));

    m_EnbRepeatPlaylist = new FlatToggleButton();
    if (!m_EnbRepeatPlaylist->Create(_T(""), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_ENB_REPEATPLAYLIST))
        return -1;
    m_EnbRepeatPlaylist->SetFont(&m_Font);
    m_Tips.AddTool(m_EnbRepeatPlaylist, _T("Repeat Playlist"));
    if (!Globals::DrawGraphics)
        m_EnbRepeatPlaylist->SetWindowText(_T("Repeat"));

    m_Info1 = new TransportViewInfoLine();
    if (!m_Info1->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL))
        return -1;
    m_Info1->Set(_T("%cNot Playing"));

    m_Info2 = new TransportViewInfoLine();
    if (!m_Info2->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL))
        return -1;
    m_Info2->Set(_T("%cNot Playing"));

    m_Volume = new TransportVolumeView();
    m_Volume->Create(NULL, NULL, WS_CHILD | WS_VISIBLE, rcClient, this, 123, NULL);
    m_Tips.AddTool(m_Volume, _T("volume"));

    m_Track = new TransportTimeView();
    if (!m_Track->Create(NULL, NULL, WS_CLIPCHILDREN | WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 123))
        return -1;

    m_Prev = new FlatToggleButton();
    if (!m_Prev->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_BTN_PREV))
        return -1;
    m_Prev->SetFont(&m_Font);
    m_Tips.AddTool(m_Prev, _T("Previous Song"));
    if (!Globals::DrawGraphics)
        m_Prev->SetWindowText(_T("Prev"));

    m_Play = new FlatToggleButton();
    if (!m_Play->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_BTN_PLAY))
        return -1;
    m_Play->SetFont(&m_Font);
    m_Tips.AddTool(m_Play, _T("Play / Pause"));
    if (!Globals::DrawGraphics)
        m_Play->SetWindowText(_T("Play"));

    m_Next = new FlatToggleButton();
    if (!m_Next->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_BTN_NEXT))
        return -1;
    m_Next->SetFont(&m_Font);
    m_Tips.AddTool(m_Next, _T("Next Song"));
    if (!Globals::DrawGraphics)
        m_Next->SetWindowText(_T("Next"));

    m_Stop = new FlatToggleButton();
    if (!m_Stop->Create(_T(""), WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, MUSIK_TRANSPORTVIEW_BTN_STOP))
        return -1;
    m_Stop->SetFont(&m_Font);
    m_Tips.AddTool(m_Stop, _T("Stop"));
    if (!Globals::DrawGraphics)
        m_Stop->SetWindowText(_T("Stop"));

    SetButtonBitmaps();
    UpdateCheckboxes();

    m_Volume->SetLabelWidth(this->m_Track->GetTotalTimeSz());

    return 0;
}

void TransportView::UpdateInfo(bool refresh)
{
    SetRedraw(FALSE);
    m_Info1->Set(musik::Cube::GetPlaybackStr(PBSTRTYPE_NOWPLAYING_TITLE));
    m_Info1->UpdateInfo();

    m_Info2->Set(musik::Cube::GetPlaybackStr(PBSTRTYPE_NOWPLAYING_ALBUM_ARTIST));
    m_Info2->UpdateInfo();

    SetRedraw();

    SetPlayingButton();    // set button to playing, paused, or resume. logic inside.

    RescaleInfo();
    m_Info2->RedrawWindow();
    m_Info1->RedrawWindow();
    RedrawWindow();

    m_Volume->SetLabelWidth(this->m_Track->GetTotalTimeSz());
}

void TransportView::UpdateVolume()
{
    m_Volume->UpdateVolume();
}

void TransportView::SetPlayingButton()
{
    if (Globals::Player->IsPlaying())
    {
        // playing, not paused
        if (!Globals::Player->IsPaused())
        {
            if (!Globals::DrawGraphics)
            {
                m_Play->SetWindowText(_T("Pause"));
            }
            else
            {
                m_Play->SetBitmaps(m_pPauseOn, m_pPauseOff);
            }
        }
        // playing, but paused
        else    
        {
            if (!Globals::DrawGraphics)
            {
                m_Play->SetWindowText(_T("Resume"));
            }
            else
            {
                m_Play->SetBitmaps(m_pPlayOn, m_pPlayOff);
            }
        }
    }
    // not playing (stopped)
    else
    {
        if (!Globals::DrawGraphics)
        {
            m_Play->SetWindowText(_T("Play"));
        }
        else
        {
            m_Play->SetBitmaps(m_pPlayOn, m_pPlayOff);
        }
    }


    m_Play->RedrawWindow();
}

void TransportView::OnSize(UINT nType, int cx, int cy)
{
    RescaleInfo();
}

void TransportView::OnDestroy()
{
    CWnd::OnDestroy();

    delete m_Info1;
    delete m_Info2;
}

CSize TransportViewContainer::CalcDynamicLayout(int nLength, DWORD dwMode)
{
    CSize sz = DockableWindow::CalcDynamicLayout(nLength, dwMode);
    
    if (sz.cy < 64)
        sz.cy = 64;

    return sz;
}

void TransportView::RescaleInfo()
{
    CSize szSize;
    CRect rcClient;
    CRect lpRect;

    int x_offset = 4;

    int btn_x, btn_y;
    if (Globals::DrawGraphics)
    {
        btn_x = 32;
        btn_y = 24;
    }
    else
    {
        btn_x = 48;
        btn_y = 24;
    }
    int btn_spc = 2;

    GetClientRect(&rcClient);    
    int btn_sum = (MUSIK_TRANSPORTVIEW_TOGGLE_CNT * btn_x) + (MUSIK_TRANSPORTVIEW_TOGGLE_CNT * btn_spc);

    int track_top = rcClient.bottom - 18;

    //
    // track
    //
    CRect trackRect = CRect(
        rcClient.left + btn_sum + x_offset,    // left
        track_top,                            // top
        rcClient.right,                        // right
        rcClient.bottom - 2);                // bottom

    m_Track->MoveWindow(trackRect);

    //
    // volume
    //

    CRect volumeRect = CRect(
        CPoint(
            rcClient.right - 128,            // left
            (track_top / 2) - (16 / 2) + 2
            //rcClient.bottom - 18 - 12 - 4    // top
       ),
        CSize(
            128,
            16
       )
   );

    m_Volume->MoveWindow(volumeRect);
    
    //
    // toggles 
    //

    int btn_start = rcClient.Width() - btn_sum - 16;

    m_EnbCrossfader->MoveWindow(CRect(
        CPoint(rcClient.left + (0 * btn_spc) + (0 * btn_x), 4),
        CSize(btn_x, btn_y)));

    m_EnbEqualizer->MoveWindow(CRect(
        CPoint(rcClient.left + (1 * btn_spc) + (1 * btn_x), 4),
        CSize(btn_x, btn_y)));

    m_EnbRepeatPlaylist->MoveWindow(CRect(
        CPoint(rcClient.left + (2 * btn_spc) + (2 * btn_x), 4),
        CSize(btn_x, btn_y)));

    m_EnbRandom->MoveWindow(CRect(
        CPoint(rcClient.left + (3 * btn_spc) + (3 * btn_x), 4),
        CSize(btn_x, btn_y)));

    //
    // playback buttons
    //

    btn_sum = (MUSIK_TRANSPORTVIEW_BTN_CNT * btn_x) + (MUSIK_TRANSPORTVIEW_BTN_CNT * btn_spc);
    btn_start = rcClient.Width() - btn_sum - 16;

    m_Prev->MoveWindow(CRect(
        CPoint(rcClient.left + (0 * btn_spc) + (0 * btn_x), rcClient.bottom - btn_y - 2),
        CSize(btn_x, btn_y)));

    m_Play->MoveWindow(CRect(
        CPoint(rcClient.left + (1 * btn_spc) + (1 * btn_x), rcClient.bottom - btn_y - 2),
        CSize(btn_x, btn_y)));

    m_Stop->MoveWindow(CRect(
        CPoint(rcClient.left + (2 * btn_spc) + (2 * btn_x), rcClient.bottom - btn_y - 2),
        CSize(btn_x, btn_y)));

    m_Next->MoveWindow(CRect(
        CPoint(rcClient.left + (3 * btn_spc) + (3 * btn_x), rcClient.bottom - btn_y - 2),
        CSize(btn_x, btn_y)));

    //
    // info lines
    //

    int info_start = btn_sum + 8;

    if (Globals::Player->IsPlaying())
    {
        // may be hidden
        m_Info2->ShowWindow(SW_SHOWNORMAL);

        szSize = m_Info1->GetSize();
        szSize.cx = rcClient.Width() - btn_sum - 8 - volumeRect.Width() - 8;

        // [title]
        lpRect = CRect(
            CPoint(btn_sum + x_offset, 4),
            szSize);

        m_Info1->MoveWindow(lpRect);

        // by [artist] from the album [album]

        int ht = m_Info2->GetHeight();
        int remain = rcClient.Height() - (m_Info2->GetHeight() + 16);

        szSize = m_Info2->GetSize();
        szSize.cx = rcClient.Width() - btn_sum - 8 - volumeRect.Width() - 8;

        lpRect = CRect(
            CPoint(
            btn_sum + x_offset, 
            (remain / 2) + 
            (ht / 2) + 2),
            szSize);

        m_Info2->MoveWindow(lpRect);
    }
    else
    {
        m_Info2->ShowWindow(SW_HIDE);

        int ht = m_Info1->GetHeight();

        // [not playing]
        lpRect = CRect(
            CPoint(btn_sum + x_offset, (track_top / 2) - (ht / 2)),
            m_Info2->GetSize());

        m_Info1->MoveWindow(lpRect);
    }
}

BOOL TransportView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

void TransportView::OnBtnPlay()
{
    // if it's not playing, play a song
    if (!Globals::Player->IsPlaying())
    {
        int PLAYER_PLAYSEL = RegisterWindowMessage(_T("PLAYER_PLAYSEL"));
        m_MainWnd->SendMessage(PLAYER_PLAYSEL);
    }

    else if (Globals::Player->IsPlaying())
    {
        // if it is playing, pause...
        if (!Globals::Player->IsPaused())
            Globals::Player->Pause();

        // is paused, resume
        else
            Globals::Player->Resume();
    }
}

void TransportView::OnBtnPrev()
{
    Globals::Player->Prev();
    GetParent()->SetFocus();
}

void TransportView::OnBtnNext()
{
    Globals::Player->Next();
}

void TransportView::PlayRandom(int field)
{
    Globals::Library->GetRandom(
        field,
        *Globals::Player->GetPlaylist(),
        true);

    int WM_NOWPLAYINGUPDATED = RegisterWindowMessage(_T("NOWPLAYINGUPDATED"));
    m_MainWnd->SendMessage(WM_NOWPLAYINGUPDATED);

    Globals::Player->Play(0, 0, musik::Core::MUSIK_PLAYER_SONG_USER_ADVANCE, true);    
}    

void TransportView::QueueRandom(int field)
{
    Globals::Library->GetRandom(
        field,
        *Globals::Player->GetPlaylist(),
        false);

    int WM_NOWPLAYINGUPDATED = RegisterWindowMessage(_T("NOWPLAYINGUPDATED"));
    m_MainWnd->SendMessage(WM_NOWPLAYINGUPDATED);
}

void TransportView::OnPlayrandomArtist()
{
    PlayRandom(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST);
}

void TransportView::OnPlayrandomAlbum()
{
    PlayRandom(musik::Core::MUSIK_LIBRARY_TYPE_ALBUM);
}

void TransportView::OnPlayrandomGenre()
{
    PlayRandom(musik::Core::MUSIK_LIBRARY_TYPE_GENRE);
}

void TransportView::OnPlayrandomYear()
{
    PlayRandom(musik::Core::MUSIK_LIBRARY_TYPE_YEAR);
}

void TransportView::OnQueuerandomArtist()
{
    QueueRandom(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST);
}

void TransportView::OnQueuerandomAlbum()
{
    QueueRandom(musik::Core::MUSIK_LIBRARY_TYPE_ALBUM);
}

void TransportView::OnQueuerandomGenre()
{
    QueueRandom(musik::Core::MUSIK_LIBRARY_TYPE_GENRE);
}

void TransportView::OnQueuerandomYear()
{
    QueueRandom(musik::Core::MUSIK_LIBRARY_TYPE_YEAR);
}

void TransportView::OnBtnStop()
{
    if (Globals::Player->IsPlaying())
        Globals::Player->Stop();
}

void TransportView::OnEnbCrossfader()
{
    if (m_EnbCrossfader->GetCheck())
    {
        musik::Core::CrossfadeSettings fader;
        musikCubeApp* ptrApp = (musikCubeApp*)AfxGetApp();
        ptrApp->GetCrossfader(&fader);

        Globals::Preferences->SetCrossfaderEnabled(true);
        Globals::Player->EnableCrossfader(true);
        Globals::Player->SetCrossfader(fader);
    }
    else
    {
        Globals::Preferences->SetCrossfaderEnabled(false);
        Globals::Player->EnableCrossfader(false);
    }
}

void TransportView::OnEnbEqualizer()
{
    if (Globals::Preferences->IsEqualizerEnabled())
    {
        Globals::Preferences->SetEqualizerEnabled(false);
        Globals::Player->EnableEqualizer(false, Globals::Preferences->EqualizerUseGlobal());
        return;
    }
    else
    {
        Globals::Preferences->SetEqualizerEnabled(true);
        Globals::Player->EnableEqualizer(true, Globals::Preferences->EqualizerUseGlobal());
        musik::Core::SetEQPreampLevel(Globals::Preferences->GetEqualizerMultiplier());
        Globals::Player->UpdateEQSettings();
    }
}

void TransportView::OnEnbRandom()
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM)
        Globals::Player->ModifyPlaymode(NULL, musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM);
    else
        Globals::Player->ModifyPlaymode(musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM, NULL);

    Globals::Preferences->SetPlayerPlaymode(Globals::Player->GetPlaymode());
}

void TransportView::OnEnbRepeat()
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST)
        Globals::Player->ModifyPlaymode(NULL, musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST);
    else
        Globals::Player->ModifyPlaymode(musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST, NULL);

    Globals::Preferences->SetPlayerPlaymode(Globals::Player->GetPlaymode());
}

void TransportView::UpdateCheckboxes()
{
    m_EnbCrossfader->SetCheck(Globals::Player->IsCrossfaderEnabled());
    m_EnbEqualizer->SetCheck(Globals::Player->IsEqualizerEnabled());
    m_EnbRandom->SetCheck(Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM);
    m_EnbRepeatPlaylist->SetCheck(Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST);
}

BOOL TransportView::PreTranslateMessage(MSG* pMsg)
{
    m_Tips.RelayEvent(pMsg);
    return CWnd::PreTranslateMessage(pMsg);
}

void TransportView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    CMenu menu;
    menu.LoadMenu(IDR_NOWPLAYING_MENU);
    menu.GetSubMenu(0)->TrackPopupMenu(NULL, point.x, point.y, this);
}

///////////////////////////////////////////////////

} } // namespace musik::Cube