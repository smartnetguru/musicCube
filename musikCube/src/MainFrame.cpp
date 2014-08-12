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
#include <shlobj.h>

#include "musikCubeApp.h"
#include "MainFrame.h"

#include "FileDialog.h"
#include "ThemeEditorDialog.h"
#include "PreferencesDialog.h"
#include "PreferencesPropertyPage.h"
#include "Tunage.h"
#include "AddNetStreamDialog.h"
#include "MainFrameMessages.h"
#include "GlobalHotkeys.h"

#include <io.h>
#include <Direct.h>
#include <Windows.h>

#include "3rdparty/memdc.h"
#include "3rdparty/CShellFileOp.h"

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// MainFrameUpdater
///////////////////////////////////////////////////

void MainFrameUpdater::run()
{
    m_Stop = false;
    m_Active = true;
    m_Finished = false;
    bool is_frame_focused = false;
        
    size_t pos = 0;

    int task_count = 0, update_count = 0;

    CString sCaption;
    while (!m_Stop)
    {
        // attempt refresh the title every second. this is ugly, fix it.
        m_Parent->UpdateTitle();

        // check transparency every 1 seconds. ugh, move this to SetFocus(), KillFocus()
        if (Globals::Preferences->IsTransEnabled() && m_Parent->IsTransparencyEnabled())
        {
            if (!is_frame_focused && GetForegroundWindow() == m_Parent->GetSafeHwnd())
            {
                m_Parent->SetTransparency(Globals::Preferences->GetTransFocus());
                is_frame_focused = true;
            }
            else if (is_frame_focused && GetForegroundWindow() != m_Parent->GetSafeHwnd())
            {
                m_Parent->SetTransparency(Globals::Preferences->GetTransUnFocus());
                is_frame_focused = false;
            }
        }

        Sleep(1000);
    }

    m_Finished = true;
}

///////////////////////////////////////////////////
// MainFrame
///////////////////////////////////////////////////

#define MUSIKWINDOWCLASSNAME _T("musikCubeWindow1.0")

IMPLEMENT_DYNAMIC(MainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CFrameWnd)

    // mfc message maps
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_SYSCOMMAND()
    ON_MESSAGE(WM_HOTKEY,OnHotKey)
    ON_WM_NCCALCSIZE()
    // menu
    ON_COMMAND(ID_FILE_PREFERENCES, OnFilePreferences)
    ON_COMMAND(ID_ADD_FILES, OnMenuAddFiles)
    ON_COMMAND(ID_ADD_DIRECTORY, OnMenuAddDirectory)
    ON_COMMAND(ID_VIEW_PLAYLISTINFORMATION, OnViewPlaylistinformation)
    ON_COMMAND(ID_SOURCES_VISIBLE, OnViewSources)
    ON_COMMAND(ID_VIEW_NOWPLAYING, OnViewNowplaying)
    ON_COMMAND(ID_AUDIO_EQUALIZER_ENABLED, OnAudioEqualizerEnabled)
    ON_COMMAND(ID_AUDIO_CROSSFADER_ENABLED, OnAudioCrossfaderEnabled)
    ON_COMMAND(ID_PLAYBACKMODE_REPEATSINGLE, OnPlaybackmodeRepeatsingle)
    ON_COMMAND(ID_PLAYBACKMODE_REPEATPLAYLIST, OnPlaybackmodeRepeatplaylist)
    ON_COMMAND(ID_PLAYBACKMODE_INTRO, OnPlaybackmodeIntro)
    ON_COMMAND(ID_VIEWMODIFIEDTAGS_VIEW, OnMenuViewModifiedTags)
    ON_COMMAND(ID_UNSYNCHRONIZEDTAGS_WRITETOFILE, OnUnsynchronizedtagsWritetofile)
    ON_COMMAND(ID_UNSYNCHRONIZEDTAGS_FINALIZEFORDATABASEONLY, OnUnsynchronizedtagsFinalizefordatabaseonly)
    ON_COMMAND(ID_VIEW_EQUALIZER, OnViewEqualizer)
    ON_COMMAND(ID_FILE_SYNCHRONIZEDDIRECTORIES, OnFileSynchronizeddirectories)
    ON_COMMAND(ID_VIEW_RESETINTERFACETODEFAULT, OnViewResetinterfacetodefault)
    ON_COMMAND(ID_NOTIFICATIONTRAY_EXIT, OnNotificationtrayExit)
    ON_COMMAND(ID_NOTIFICATIONTRAY_RESTORE, OnNotificationtrayRestore)
    ON_COMMAND(ID_NOTIFICATIONTRAY_PLAY, OnNotificationtrayPlay)
    ON_COMMAND(ID_NOTIFICATIONTRAY_STOP, OnNotificationtrayStop)
    ON_COMMAND(ID_NOTIFICATIONTRAY_PAUSE, OnNotificationtrayPause)
    ON_COMMAND(ID_NOTIFICATIONTRAY_NEXT, OnNotificationtrayNext)
    ON_COMMAND(ID_NOTIFICATIONTRAY_PREV, OnNotificationtrayPrev)
    ON_COMMAND(ID_FILE_CLEARLIBRARY, OnMenuEraseLibrary)
    ON_COMMAND(ID_LIBRARY_SYNCHRONIZEDIRECTORIESNOW, OnMenuSynchronize)
    ON_COMMAND(ID_PLAYBACKMODE_SHUFFLECURRENTPLAYLIST, OnPlaybackmodeShufflecurrentplaylist)
    ON_COMMAND(ID_VIEW_VISUALIZATION, OnViewVisualization)
    ON_COMMAND(ID_VIEW_ALWAYSONTOP, OnViewAlwaysontop)
    ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullscreen)
    ON_COMMAND(ID_ADD_NETSTREAM, OnMenuAddNetRadio)
    ON_COMMAND(ID_RATECURRENT_N1, OnRatecurrentN1)
    ON_COMMAND(ID_RATECURRENT_UNRATED, OnRatecurrentUn)
    ON_COMMAND(ID_RATECURRENT_1, OnRatecurrent1)
    ON_COMMAND(ID_RATECURRENT_2, OnRatecurrent2)
    ON_COMMAND(ID_RATECURRENT_3, OnRatecurrent3)
    ON_COMMAND(ID_RATECURRENT_4, OnRatecurrent4)
    ON_COMMAND(ID_RATECURRENT_5, OnRatecurrent5)
    ON_COMMAND(ID_FILE_EXPORTLIBRARY, OnMenuExportLibrary)
    ON_COMMAND(ID_FILE_ATTEMPTTOSANITISETAGS, OnFileAttempttosanitisetags)
    ON_COMMAND(ID_PLAYBACK_RANDOM, OnPlaybackRandom)
    ON_COMMAND(ID_TAGS_REMOVEEXTRAWHITESPACE, OnTagsRemoveextrawhitespace)
    ON_COMMAND(ID_TAGS_AUTO, OnTagsAuto)
    ON_COMMAND(ID_FILE_PLUGINMANAGER, OnFilePluginmanager)
    ON_COMMAND(ID_HELP_ONLINEHELP, OnHelpOnlinehelp)
    ON_COMMAND(ID_SOURCES_SHOWDEVICES, OnSourcesShowdevices)
    ON_COMMAND(ID_SOURCES_SHOWPLAYLISTS, OnSourcesShowplaylists)
    ON_COMMAND(ID_SOURCES_SHOWDYNAMICPLAYLISTS, OnSourcesShowdynamicplaylists)
    ON_COMMAND(ID_SOURCES_SHOWPLUGINS, OnSourcesShowplugins)
    ON_COMMAND(ID_FILE_THEMEEDITOR, OnFileThemeeditor)
    // update ui
    ON_UPDATE_COMMAND_UI(ID_SOURCES_VISIBLE, OnUpdateViewSources)
    ON_UPDATE_COMMAND_UI(ID_VIEW_SELECTIONBOXES, OnUpdateViewSelectionboxes)
    ON_UPDATE_COMMAND_UI(ID_VIEW_NOWPLAYING, OnUpdateViewNowplaying)
    ON_UPDATE_COMMAND_UI(ID_AUDIO_EQUALIZER_ENABLED, OnUpdateAudioEqualizerEnabled)
    ON_UPDATE_COMMAND_UI(ID_AUDIO_CROSSFADER_ENABLED, OnUpdateAudioCrossfaderEnabled)
    ON_UPDATE_COMMAND_UI(ID_VIEW_PLAYLISTINFORMATION, OnUpdateViewPlaylistinformation)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_REPEATSINGLE, OnUpdatePlaybackmodeRepeatsingle)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_REPEATPLAYLIST, OnUpdatePlaybackmodeRepeatplaylist)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_INTRO, OnUpdatePlaybackmodeIntro)
    ON_UPDATE_COMMAND_UI(ID_VIEW_EQUALIZER, OnUpdateViewEqualizer)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PLAY, OnUpdateNotificationtrayPlay)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PAUSE, OnUpdateNotificationtrayPause)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_NEXT, OnUpdateNotificationtrayNext)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_PREV, OnUpdateNotificationtrayPrev)
    ON_UPDATE_COMMAND_UI(ID_NOTIFICATIONTRAY_STOP, OnUpdateNotificationtrayStop)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACKMODE_SHUFFLECURRENTPLAYLIST, OnUpdatePlaybackmodeShufflecurrentplaylist)
    ON_UPDATE_COMMAND_UI(ID_VIEW_ALWAYSONTOP, OnUpdateViewAlwaysontop)
    ON_UPDATE_COMMAND_UI(ID_VIEW_VISUALIZATION, OnUpdateViewVisualization)
    ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, OnUpdateViewFullscreen)
    ON_UPDATE_COMMAND_UI(ID_PLAYBACK_RANDOM, OnUpdatePlaybackRandom)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWDEVICES, OnUpdateSourcesShowdevices)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWPLAYLISTS, OnUpdateSourcesShowplaylists)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWDYNAMICPLAYLISTS, OnUpdateSourcesShowdynamicplaylists)
    ON_UPDATE_COMMAND_UI(ID_SOURCES_SHOWPLUGINS, OnUpdateSourcesShowplugins)
    ON_COMMAND(ID_VIEW_SELECTIONBOXES, OnViewSelectionboxes)
    ON_COMMAND(ID_MUSICLIBRARY_CHANGE, OnMenuChangeLibrary)
    // custom message maps
    ON_REGISTERED_MESSAGE(WM_SOURCESUNSEL, OnSourcesUnsel)
    ON_REGISTERED_MESSAGE(WM_SELBOXUPDATE, OnSelectionCtrlChanged)
    ON_REGISTERED_MESSAGE(WM_SONGCHANGE, OnSongChange)
    ON_REGISTERED_MESSAGE(WM_SONGSTOP, OnSongStop)
    ON_REGISTERED_MESSAGE(WM_SONGPAUSE, OnSongPause)
    ON_REGISTERED_MESSAGE(WM_SONGRESUME, OnSongResume)
    ON_REGISTERED_MESSAGE(WM_SOURCESLIBRARY, OnSourcesLibrary)
    ON_REGISTERED_MESSAGE(WM_SOURCESNOWPLAYING, OnSourcesNowPlaying)
    ON_REGISTERED_MESSAGE(WM_SOURCESDYNPLAYLIST, OnSourcesDynPlaylist)
    ON_REGISTERED_MESSAGE(WM_SOURCESSTDPLAYLIST, OnSourcesStdPlaylist)
    ON_REGISTERED_MESSAGE(WM_SOURCESQUICKSEARCH, OnSourcesQuickSearch)
    ON_REGISTERED_MESSAGE(WM_SOURCESQUICKSEARCHSTR, OnSourcesQuickSearchStr)
    ON_REGISTERED_MESSAGE(WM_PLAYSONG, OnPlaySong)
    ON_REGISTERED_MESSAGE(WM_SELBOXEDITCOMMIT, OnSelectionCtrlEditCommit)
    ON_REGISTERED_MESSAGE(WM_SELECTIONCTRLDELETESELECTED, OnSelectionCtrlDeleteSelected)
    ON_REGISTERED_MESSAGE(WM_TASK_PROGRESS, OnTaskProgress)
    ON_REGISTERED_MESSAGE(WM_TASK_END, OnTaskEnd)
    ON_REGISTERED_MESSAGE(WM_PLAYER_PLAYSEL, OnPlayerPlaySel)
    ON_REGISTERED_MESSAGE(WM_BATCHADD_VERIFY_PLAYLIST, OnVerifyPlaylist)
    ON_REGISTERED_MESSAGE(WM_SELBOXADDREMOVE, OnSelBoxAddRemove)
    ON_REGISTERED_MESSAGE(WM_SELBOXREQUESTUPDATE, OnSelBoxRequestUpdate)
    ON_REGISTERED_MESSAGE(WM_UPDATEPLAYLIST, OnUpdateCurrPlaylist)
    ON_REGISTERED_MESSAGE(WM_EQUALIZERCHANGE, OnEqualizerChange)
    ON_REGISTERED_MESSAGE(WM_SOURCESITEMCLICKED, OnSourcesItemClicked)
    ON_REGISTERED_MESSAGE(WM_INITTRANS, OnInitTrans)
    ON_REGISTERED_MESSAGE(WM_DEINITTRANS, OnDeinitTrans)
    ON_REGISTERED_MESSAGE(WM_NOWPLAYINGUPDATED, OnNowPlayingUpdated)
    ON_REGISTERED_MESSAGE(WM_SOURCESCDAUDIO, OnSourcesCDAudio)
    ON_REGISTERED_MESSAGE(WM_SOURCESQUERYFREEDB, OnSourcesQueryFreeDB)
    ON_REGISTERED_MESSAGE(WM_SOURCESREMOVABLEDEV, OnSourcesRemovableDev)
    ON_REGISTERED_MESSAGE(WM_SOURCESREFRESH, OnSourcesRefresh)
    ON_REGISTERED_MESSAGE(WM_SOURCESRESET, OnSourcesReset)
    ON_REGISTERED_MESSAGE(WM_SOURCESNETSTREAMS, OnSourcesNetRadio)
    ON_REGISTERED_MESSAGE(WM_SELBOXRESET, OnSelectionBoxReset)
    ON_REGISTERED_MESSAGE(WM_SELBOXACTIVATE, OnSelectionBoxActivate)
    ON_REGISTERED_MESSAGE(WM_TASKBARCREATED, OnTaskbarCreated)
    ON_REGISTERED_MESSAGE(WM_SHOWMUSIKCUBE, OnShowCube)
    ON_REGISTERED_MESSAGE(WM_HIDEMUSIKCUBE, OnHideCube)
    ON_REGISTERED_MESSAGE(WM_UPDATEVOLUME, OnUpdateVolume)
    ON_COMMAND(ID_HELP_CHECKFORUPDATES, OnHelpCheckForUpdates)
END_MESSAGE_MAP()

/*ctor*/ MainFrame::MainFrame(bool autostart)
{
    m_AutoStart         = autostart;
    m_TransEnb          = false;
    m_FullScreen        = false;
    m_IsMinimized       = false;
    m_GoingFullScreen   = false;
    m_Progress          = 0;
    m_CDView            = NULL;
    m_PlaylistView      = NULL;
    m_MainView          = NULL;
    m_SourcesView       = NULL;

    m_hIcon16 = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_MAINFRAME), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
    m_hIcon32 = (HICON)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_MAINFRAME), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);

    InitDragTypes();
}

/*dtor*/ MainFrame::~MainFrame()
{
    // Save selection control floating window states
    //
    musik::Core::IntArray sel_modes;
    //
    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        sel_modes.push_back(m_AllFilterViews.at(i)->GetCtrl()->GetType());
        delete m_AllFilterViews.at(i);
    }
    //
    Globals::Preferences->SetSelBoxTypes(sel_modes);

    delete m_PlaylistView;
    delete m_SourcesView;
    delete m_NowPlayingView;
    delete m_CDView;
    delete m_EqualizerView;

    ::UnSetMMShellHook(GetSafeHwnd());
}

LRESULT MainFrame::OnHotKey(WPARAM wParam, LPARAM lParam)
{
    std::wstring statusString;

    if (GlobalHotkeys::ProcessHotkey((GlobalHotkeys::Hotkey) wParam, statusString))
    {
        if (Globals::Preferences->GetShowBalloonTips())
        {
            ShowBalloonTip(L"musikCube", statusString.c_str());
        }
    }

    return 0L;
}

void MainFrame::InitDragTypes()
{
    m_uPlaylistDrop_L   = RegisterClipboardFormat (_T("musikPlaylist_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82"));
    m_uPlaylistDrop_R   = RegisterClipboardFormat (_T("musikPlaylist_9543E137-7131-49fd-ADB5-208F0EF7A05C"));
    m_uSourcesDrop      = RegisterClipboardFormat (_T("musikSources_3BCFE9D1_6D61_4cb6_9D0B_3BB3F643CA82"));
    m_uSelectionDrop_R  = RegisterClipboardFormat (_T("musikSelection_070D6C9E-0E34-4b56-990D-5E2A2FA719A0"));
    m_uSelectionDrop_L  = RegisterClipboardFormat (_T("musikSelection_374DA040-66D6-4b2d-9E8C-592421FF499D"));
}

void MainFrame::LoadDlgSize()
{
    CRect rcNormal = CRect(Globals::Preferences->GetDlgPos(), Globals::Preferences->GetDlgSize());
    MoveWindow(rcNormal);
    
    if (Globals::Preferences->IsMaximized())
    {
        WINDOWPLACEMENT max;
        max.showCmd = SW_MAXIMIZE;
        max.rcNormalPosition = rcNormal;
        SetWindowPlacement(&max);
        return;
    }
    else
        MoveWindow(rcNormal);

}

void MainFrame::ResetDialogPosition()
{
    MoveWindow(0, 0, 800, 600);

    CSize size;
    size.cx = 800;
    size.cy = 48;

    // equalizer
    size.cx = 800;
    size.cy = 84;
    m_EqualizerView->ForceDockedSize(size, LM_HORZDOCK, true);

    size.cx = 520;
    size.cy = 130;
    m_EqualizerView->ForceFloatSize(size);

    // now playing
    size.cx = 800;
    size.cy = 48;
    m_NowPlayingView->ForceDockedSize(size, LM_HORZDOCK, true);
    m_NowPlayingView->ForceFloatSize(size);

    // sources
    size.cx = 140;
    size.cy = 600;
    m_SourcesView->ForceDockedSize(size, LM_VERTDOCK, true);
    m_SourcesView->ForceFloatSize(size);

    // selection box
    size.cx = NULL;
    size.cy = 120;
    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews[i]->ForceDockedSize(size, LM_HORZDOCK, true);
    }

    // hide equalizer
    ShowControlBar(m_EqualizerView, FALSE, TRUE);
}

void MainFrame::ResetSelectionCtrls()
{
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews[i]->GetCtrl()->Reset(this->GetPlaylistInfoAsFilter());
    }
    //
    this->m_ActiveFilterViews.clear();
}

void MainFrame::DockBarLeftOf(CSizingControlBar* pBar, CSizingControlBar* pTargetBar)
{
    ASSERT(pBar != NULL);
    ASSERT(pTargetBar != NULL);
    ASSERT(pBar != pTargetBar);

    // the neighbor must be already docked
    CDockBar* pDockBar = pTargetBar->m_pDockBar;
    ASSERT(pDockBar != NULL);
    UINT nDockBarID = pTargetBar->m_pDockBar->GetDlgCtrlID();
    ASSERT(nDockBarID != AFX_IDW_DOCKBAR_FLOAT);

    bool bHorz = (nDockBarID == AFX_IDW_DOCKBAR_TOP ||
        nDockBarID == AFX_IDW_DOCKBAR_BOTTOM);

    // dock normally (inserts a new row)
    DockControlBar(pBar, nDockBarID);

    // delete the new row (the bar pointer and the row end mark)
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);
    pDockBar->m_arrBars.RemoveAt(pDockBar->m_arrBars.GetSize() - 1);

    // find the target bar
    for (int i = 0; i < pDockBar->m_arrBars.GetSize(); i++)
    {
        void* p = pDockBar->m_arrBars[i];
        if (p == pTargetBar) // and insert the new bar after it
            pDockBar->m_arrBars.InsertAt(i + 1, pBar);
    }

    // move the new bar into position
    CRect rBar;
    pTargetBar->GetWindowRect(rBar);
    rBar.OffsetRect(bHorz ? 1 : 0, bHorz ? 0 : 1);
    pBar->MoveWindow(rBar);
}

int MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // setup hotkeys
    if(Globals::Preferences->GetUseGlobalHotkeys())
    {
        GlobalHotkeys::ReloadHotkeys();
    }

    // tray icon
    
    InitTrayIcon();
    if (Globals::Preferences->MinimizeToTray() || Globals::Preferences->GetCloseToSystemTray())
    {
        ShowTrayIcon();
    }

    // initialize plugins
    Globals::LoadMainFramePlugins();

    SetIcon(m_hIcon32, true);
    SetIcon(m_hIcon16, false);

    // set default dock order
    EnableDocking(CBRS_ALIGN_BOTTOM);
    EnableDocking(CBRS_ALIGN_LEFT);
    EnableDocking(CBRS_ALIGN_RIGHT);
    EnableDocking(CBRS_ALIGN_TOP);

    #ifdef _SCB_REPLACE_MINIFRAME
        m_pFloatingFrameClass = RUNTIME_CLASS(CSCBMiniDockFrameWnd);
    #endif

    // create the view
    m_PlaylistView = new PlaylistViewContainer(this, m_uPlaylistDrop_L, m_uPlaylistDrop_R, m_uSourcesDrop, m_uSelectionDrop_L, m_uSelectionDrop_R, Globals::Preferences->PlaylistInfoVisible());
    m_PlaylistView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL);
    m_MainView = (CView*)m_PlaylistView;

    // sources control
    m_SourcesView = new SourcesView(this, m_uSourcesDrop, m_uPlaylistDrop_R, m_uSelectionDrop_R);
    m_SourcesView->Create(_T("Sources"), this, ID_SOURCESBOX);
    DockControlBar(m_SourcesView, AFX_IDW_DOCKBAR_LEFT);

    // now playing control
    m_NowPlayingView = new TransportViewContainer(this);
    m_NowPlayingView->Create(_T("musik Now Playing"), this, ID_NOWPLAYING);
    DockControlBar(m_NowPlayingView, AFX_IDW_DOCKBAR_BOTTOM);
    
    // equalizer control
    m_EqualizerView = new EqualizerViewContainer();
    m_EqualizerView->Create(_T("Equalizer"), this, ID_EQUALIZER);
    m_EqualizerView->GetCtrl()->SetBandState(Globals::Preferences->GetEqualizerBandState());
    FloatControlBar(m_EqualizerView, CPoint(28, 28));
    ShowControlBar(m_EqualizerView, FALSE, FALSE);

    // selection controls
    MetadataFilterViewContainer* pBar;
    for (size_t i = 0; i < Globals::Preferences->GetSelBoxCount(); i++)
    {
        pBar = new MetadataFilterViewContainer(this, Globals::Preferences->GetSelBoxType(i), i, m_uSelectionDrop_L, m_uSelectionDrop_R);
        pBar->Create(_T("musik Selection Box"), this, ID_SELECTIONBOX_START + i);
        if (i == 0)
            DockControlBar(pBar);
        else
            DockBarLeftOf(pBar, m_AllFilterViews.at(i - 1));

        m_AllFilterViews.push_back(pBar);
    }

    // append the system menu
    CMenu* pMenu = GetSystemMenu(false);
    if (pMenu)
        pMenu->InsertMenu(0, MF_STRING, SYSMENU_RESETDIALOG, _T("Reset Window"));

    // load dockbar sizes and positions
    
    ResetDialogPosition();
    LoadDlgSize();

    if (VerifyBarState(_T("musikProfile0.90")))
    {
        CSizingControlBar::GlobalLoadState(this, _T("musikProfile0.90"));
        LoadBarState(_T("musikProfile0.90"));
    }

    // fire the updater task off
    m_Updater = new MainFrameUpdater;
    m_Updater->m_Parent = this;
    m_Updater->start();
    
    ImportTrans();
    if (Globals::Preferences->IsTransEnabled())
        InitTrans();

    // always on top?
    if (Globals::Preferences->IsAlwaysOnTop())
    {
        CRect rcWnd;
        GetWindowRect(rcWnd);

        ::SetWindowPos(m_hWnd, 
            HWND_TOPMOST, 
            rcWnd.TopLeft().x, 
            rcWnd.TopLeft().y, 
            rcWnd.Width(), 
            rcWnd.Height(), 
            NULL);
    }

    // restore last known playlist
    Globals::Library->GetAllTempSongs(*Globals::Player->GetPlaylist());
    Globals::Library->ClearTempSongTable();

    // focus now playing
    if(Globals::Player->GetPlaylist()->GetCount())
    {
        m_SourcesView->FocusNowPlaying();

        int last = Globals::Preferences->GetDlgLastPlayed();
        //
        if (last >= 0)
        {
            Globals::Player->FindNewIndex(last);
            last = Globals::Player->GetIndex();
            //
            if (last >= 0)
            {
                m_PlaylistView->GetCtrl()->EnsureVisible(last, FALSE);
                m_PlaylistView->GetCtrl()->SetSelectionMark(last);
                m_PlaylistView->GetCtrl()->SetItemState(last, LVIS_SELECTED, LVIS_SELECTED);
            }
        }
    }
    else
    {
        this->m_SourcesView->FocusLibrary();
    }

    ::SetMMShellHook(GetSafeHwnd());

    return 0;
}

void MainFrame::GivePlaylistToView(musik::Core::Playlist* playlist, bool giveCopy)
{
    m_PlaylistView->GetCtrl()->SavePlaylist();

    musik::Core::Playlist *playlistToGive = playlist;
    //
    if (giveCopy)
    {
        playlistToGive = new musik::Core::Playlist(*playlist);
    }

    m_PlaylistView->GetCtrl()->SetPlaylist(playlistToGive);
}

bool MainFrame::PlayTrackFromExplorer(const CString& fn)
{
    if (fn.IsEmpty())
        return false;

    if (!m_SourcesView)
        return false;

    if ( ! Globals::Collected)
    {
        m_SourcesView->LoadCollectedLibrary();
    }

    // add song to library, if necessary
    if (!Globals::Collected->AddSong((musik::Core::String)fn))
        return false;

    // get player's playlist
    musik::Core::Playlist* pPlaylist = Globals::Player->GetPlaylist();
    if (pPlaylist)
    {
        // get the song we just added...
        musik::Core::Song song;
        Globals::Collected->GetSongAttrFromFilename((musik::Core::String)fn, song);

        // add to the control's playlist
        if (song.GetID() >= 0)
        {
            pPlaylist->Add(song);

            // set the player's playlist, and play
            if (Globals::Player)
            {
                Globals::Player->Play(pPlaylist->GetCount() - 1);
                UpdateFilters(UpdateActionNone, UpdateActionRequery);
                m_SourcesView->FocusNowPlaying();
            }

            UpdateFilters(UpdateActionQuery, UpdateActionRequery);

            return true;
        }
    }

    return false;
}

BOOL MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
    if(!CFrameWnd::PreCreateWindow(cs))
        return FALSE;

    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    // assure class not already registered
    if(!(::GetClassInfo(hInst, MUSIKWINDOWCLASSNAME, &wndcls)))   
    {
        // get default MFC class settings
        if(::GetClassInfo(hInst, cs.lpszClass, &wndcls))        
        {
            wndcls.lpszClassName = MUSIKWINDOWCLASSNAME;
            wndcls.style |= CS_OWNDC;
            wndcls.hbrBackground = NULL;

            if (!AfxRegisterClass(&wndcls)) 
            {
                AfxThrowResourceException();
            }
        }
        else
        {
            AfxThrowResourceException();
        }
    }

    cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
    cs.lpszClass = MUSIKWINDOWCLASSNAME;

    return TRUE;
}

BOOL MainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
    // let the view have first crack at the command

    // otherwise, do default handling
    return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

BOOL MainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
    return TRUE;
}

BOOL MainFrame::DestroyWindow()
{
    // make sure all the tasks are finished
    // before destroying the window...
    KillTasks(true, true);

    // stop all plugins
    Globals::UnloadMainFramePlugins();

    CSizingControlBar::GlobalSaveState(this, _T("musikProfile0.90"));
    SaveBarState(_T("musikProfile0.90"));

    HideTrayIcon();

    return CFrameWnd::DestroyWindow();
}

BOOL MainFrame::VerifyBarState(LPCTSTR lpszProfileName)
{
    // This function is Copyright (c) 2000, Cristi Posea.
    // See www.datamekanix.com for more control bars tips&tricks.
    CDockState state;
    state.LoadState(lpszProfileName);

    for (int i = 0; i < state.m_arrBarInfo.GetSize(); i++)
    {
        CControlBarInfo* pInfo = (CControlBarInfo*)state.m_arrBarInfo[i];
        ASSERT(pInfo != NULL);
        int nDockedCount = (int)pInfo->m_arrBarID.GetSize();
        if (nDockedCount > 0)
        {
            // dockbar
            for (int j = 0; j < nDockedCount; j++)
            {
                UINT nID = (UINT) pInfo->m_arrBarID[j];
                if (nID == 0) continue; // row separator
                if (nID > 0xFFFF)
                    nID &= 0xFFFF; // placeholder - get the ID
                if (GetControlBar(nID) == NULL)
                    return FALSE;
            }
        }
        
        if (!pInfo->m_bFloating) // floating dockbars can be created later
            if (GetControlBar(pInfo->m_nBarID) == NULL)
                return FALSE; // invalid bar ID
    }

    return TRUE;
}

void MainFrame::OnDestroy()
{
    CSizingControlBar::GlobalSaveState(this, _T("musikDockBars0.90"));
    
    SaveWindowState();

    CFrameWnd::OnDestroy();
}

void MainFrame::SaveWindowState()
{
    WINDOWPLACEMENT max;
    GetWindowPlacement(&max);

    // let full screen do its own thing, other wise set
    // the maximize flag.
    if (!m_GoingFullScreen && !m_FullScreen)
    {
        if (max.showCmd == SW_SHOWMAXIMIZED && !m_IsMinimized)
            Globals::Preferences->SetMaximized(true);
        else if (!m_IsMinimized)
        {
            CRect rc_dlg;
            GetWindowRect(&rc_dlg);

            Globals::Preferences->SetDlgSize(CSize(rc_dlg.right - rc_dlg.left, rc_dlg.bottom - rc_dlg.top));
            Globals::Preferences->SetDlgPos(CPoint(rc_dlg.left, rc_dlg.top));
            Globals::Preferences->SetMaximized(false);
        }
            
    }
}

LRESULT MainFrame::OnSelectionCtrlEditCommit(WPARAM wParam, LPARAM lParam)
{
    // setup vars
    MetadataFilterView* pSel = (MetadataFilterView*)wParam;
    int nLibType = (int)lParam;
    musik::Core::String sNew = pSel->GetEditCommitStr();

    // the selected items are whats currently visible in the playlist
    musik::Core::Playlist selectedSongs;
    this->QueryPlaylist(selectedSongs);

    // create a new musik::Core::SongInfoArray and update all the respective values...
    if (selectedSongs.GetCount())
    {
        // get songs    
        musik::Core::SongInfoArray* pSongInfoArray = new musik::Core::SongInfoArray();
        Globals::Library->GetInfoArrayFromPlaylist(selectedSongs, *pSongInfoArray, nLibType, sNew, true);

        // setup params
        musik::Core::BatchRetag* params = new musik::Core::BatchRetag(
            Globals::Library, 
            Globals::Functor, 
            pSongInfoArray);

        params->m_WriteToFile = false;
        params->m_DeleteUpdatedTags = true;

        // spawn the task
        musik::Core::BatchRetagTask* task = new musik::Core::BatchRetagTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);

        return 1L;
    }

    return 0L;
}

LRESULT MainFrame::OnSelectionCtrlChanged(WPARAM wParam, LPARAM lParam)
{
    MetadataFilterView* senderCtrl = (MetadataFilterView*) wParam;
    MetadataFilterView* parentCtrl = this->GetParentSelectionCtrl();

    // Show All... was selected
    //
    if (senderCtrl->IsItemSelected(0))
    {
        UpdateFilters(UpdateActionReset, UpdateActionReset);
        return 0L;
    }

    // If there is no parent control, let the sender be the parent
    //
    if (parentCtrl == NULL)
    {
        senderCtrl->SetParent(true);
        senderCtrl->RedrawWindow();
    }

    // We are now part of the filter! if we are not part of the filter
    // "senderCtrl" will be added to. if we are already a part of the
    // filter, child filters may be pruned.
    //
    this->UpdateSelectionCtrlOrder(senderCtrl);

    UpdateFilters(UpdateActionQuery, UpdateActionQuery);

    return 0L;
}

void MainFrame::SetTransparency(int trans)
{
    if (trans < 20)
    {
        trans = 20;
    }

    m_pSetLayeredWindowAttributes(GetSafeHwnd(), 0, trans, LWA_ALPHA);
    m_Trans = trans;
}

void MainFrame::ImportTrans()
{
    HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
    m_pSetLayeredWindowAttributes = 
        (lpfnSetLayeredWindowAttributes)GetProcAddress(hUser32, "SetLayeredWindowAttributes");
}

void MainFrame::DeinitTrans()
{
    if (m_TransEnb)
    {
        SetWindowLong(GetSafeHwnd(), GWL_EXSTYLE, GetWindowLong(GetSafeHwnd() , GWL_EXSTYLE) & ~WS_EX_LAYERED);
        RedrawWindow();

        m_TransEnb = false;
    }
}

void MainFrame::InitTrans(bool set_trans)
{
    if (!m_TransEnb)
    {
        SetWindowLong(
            GetSafeHwnd(),
            GWL_EXSTYLE, 
            GetWindowLong(GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED 
       );

        if (set_trans)
        {
            SetTransparency(m_Trans);
        }
        else
        {
            RedrawWindow();
        }

        m_TransEnb = true;
    }
}

LRESULT MainFrame::OnInitTrans(WPARAM wParam, LPARAM lParam)
{
    InitTrans(true);
    return 0L;
}

LRESULT MainFrame::OnDeinitTrans(WPARAM wParam, LPARAM lParam)
{
    DeinitTrans();
    return 0L;
}

void MainFrame::RequerySelectionCtrls()
{
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        MetadataFilterView* currentCtrl = this->m_AllFilterViews[i]->GetCtrl();
        currentCtrl->Requery();
    }
}

void MainFrame::QuerySelectionCtrls()
{
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        MetadataFilterView* currentCtrl = this->m_AllFilterViews[i]->GetCtrl();

        int currentOrder = this->GetSelectionCtrlOrder(currentCtrl);
        //
        if (currentOrder == -1)     // only Query boxes that are not filtered
        {
            currentCtrl->Query(this->GetSelectionCtrlFilter(currentCtrl));
        }
    }
}

void MainFrame::QueryPlaylist(musik::Core::Playlist& playlist)
{
    // if there are selection boxes visible
    musik::Core::String query = this->GetPlaylistQuery();
    //
    if (query != L"")
    {
        // see what the order should be by...
        int orderByType = musik::Core::MUSIK_LIBRARY_TYPE_ARTIST;
        MetadataFilterView* parent = this->GetParentSelectionCtrl();
        if (parent)
        {
            orderByType = parent->GetType();
        }

        Globals::Library->RawQuerySongs(query, playlist);
    }
}

LRESULT MainFrame::OnSelBoxRequestUpdate(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateActionQuery, UpdateActionNone);
    return 0L;
}

LRESULT MainFrame::OnEqualizerChange(WPARAM wParam, LPARAM lParam)
{
    m_EqualizerView->GetCtrl()->LoadCurrSong();

    return 0L;
}

LRESULT MainFrame::OnSongChange(WPARAM wParam, LPARAM lParam)
{
    // tell the child windows to redraw their
    // state accordingly
    if ((PlaylistViewContainer*)m_MainView == m_PlaylistView)
    {
        if (m_PlaylistView->GetCtrl()->GetPlaylist() == Globals::Player->GetPlaylist())
            m_PlaylistView->GetCtrl()->ScrollToCurr();

        m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPrevSong());
        m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());
    }
    else if (m_MainView == (CView*) m_CDView)
    {
        m_CDView->RedrawWindow();
    }

    m_NowPlayingView->GetCtrl()->GetTimeCtrl()->OnNewSong();
    m_NowPlayingView->GetCtrl()->UpdateInfo();

    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews.at(i)->GetCtrl()->OnNewSong();
        m_AllFilterViews.at(i)->GetCtrl()->RedrawWindow();
    }
    
    // if the player is playing, then we need
    // to do our own updating
    if (Globals::Player->IsPlaying())
    {
        this->UpdateTitle();
    }

    // update notification tooltip
    if (Globals::Preferences->MinimizeToTray())
    {
        CString tray = musik::Cube::GetPlaybackStr(PBSTRTYPE_TASKTRAY);
        if (tray.GetLength() > 127)
            tray = tray.Left(127); 
        wcscpy(m_TrayIcon.szTip, tray.GetBuffer());
        Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);

        if (Globals::Preferences->GetShowBalloonTips())
        {
            ShowBalloonTip(
                musik::Cube::GetPlaybackStr(PBSTRTYPE_BALLOONTITLE), 
                musik::Cube::GetPlaybackStr(PBSTRTYPE_BALLOONARTISTALBUM));
        }
    }

    if (Globals::Preferences->GetTunageEnabled())
    {
        Tunage tunage;
        tunage.Execute();
    }

    PropertyTreeItem* focused = m_SourcesView->GetFocusedItem();
    if (focused && focused->GetPlaylistType() == musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC)
    {
        m_PlaylistView->GetCtrl()->Refresh();
    }

    return 0L;
}   

LRESULT MainFrame::OnSongStop(WPARAM wParam, LPARAM lParam)
{
    m_NowPlayingView->GetCtrl()->GetTimeCtrl()->OnNewSong();
    m_NowPlayingView->GetCtrl()->UpdateInfo();

    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPrevSong());
    m_PlaylistView->GetCtrl()->RedrawWindow();
    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews.at(i)->GetCtrl()->RedrawWindow();
    }

    // update title bar
    this->UpdateTitle();

    if (Globals::Preferences->MinimizeToTray())
    {
        CString tray;
        wcscpy (m_TrayIcon.szTip, _T(MUSIK_VERSION_STR));

        // clears balloon tip info
        wcscpy(m_TrayIcon.szInfoTitle, _T("\0"));
        wcscpy(m_TrayIcon.szInfo, _T("\0"));

        Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);
    }

    return 0L;
}

LRESULT MainFrame::OnSongPause(WPARAM wParam, LPARAM lParam)
{
    m_NowPlayingView->GetCtrl()->UpdateInfo();
    return 0L;
}

LRESULT MainFrame::OnSongResume(WPARAM wParam, LPARAM lParam)
{
    m_NowPlayingView->GetCtrl()->UpdateInfo();
    return 0L;
}

LRESULT MainFrame::OnSourcesItemClicked(WPARAM wParam, LPARAM lParam)
{
    // SELECTING AN ITEM ALWAYS SETS ITSELF AS THE NEW GLOBALLY SELECTED PLAYLIST INFO
    this->m_PlaylistInfo = this->m_SourcesView->GetFocusedItem()->GetPlaylistInfo();
    HideCDView();

    return 0L;
}

LRESULT MainFrame::OnSourcesLibrary(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateActionReset, UpdateActionReset);
    ShowSelectionBoxes(true);

    return 0L;
}

LRESULT MainFrame::OnSourcesQuickSearchStr(WPARAM wParam, LPARAM lParam)
{
    if (wParam)
    {
        musik::Core::Playlist* playlist = new musik::Core::Playlist();
        CString* sPattern = (CString*)wParam;
        if (*sPattern != L"")
        {
            Globals::Library->QuickQuery((musik::Core::String)*sPattern, *playlist);
        }

        this->m_PlaylistInfo = playlist->GetPlaylistInfo();
        this->GivePlaylistToView(playlist);
        //
        UpdateFilters(UpdateActionQuery, UpdateActionRequery);
    }

    return 0L;
}

LRESULT MainFrame::OnSourcesQuickSearch(WPARAM wParam, LPARAM lParam)
{
    ShowSelectionBoxes(true);
    UpdateFilters(UpdateActionClear, UpdateActionClear);

    return 0L;
}

LRESULT MainFrame::OnSourcesNowPlaying(WPARAM wParam, LPARAM lParam)
{
    if (Globals::Player->GetPlayerType() == musik::Core::MUSIK_PLAYER_CDROM)
    {
        OnSourcesCDAudio((WPARAM)Globals::Player->GetDiscDrive(), NULL);
        return 0L;
    }

    this->GivePlaylistToView(Globals::Player->GetPlaylist());
    UpdateFilters(UpdateActionNone, UpdateActionRequery);
    m_PlaylistView->GetCtrl()->ScrollToCurr();

    ShowSelectionBoxes(false);

    return 0L;
}

void MainFrame::UpdateFilters(enum UpdateAction selectionUpdateAction, enum UpdateAction playlistUpdateAction)
{
    switch (selectionUpdateAction)
    {
    case MainFrame::UpdateActionReset:
        ResetSelectionCtrls();
        break;

    case MainFrame::UpdateActionRequery:
        RequerySelectionCtrls();
        break;

    case MainFrame::UpdateActionQuery:
        QuerySelectionCtrls();
        break;

    case MainFrame::UpdateActionClear:
        ClearSelectionCtrls();
        break;
    }

    switch (playlistUpdateAction)
    {
    case MainFrame::UpdateActionQuery:
        this->QueryPlaylistCtrl();
        break;
 
    case MainFrame::UpdateActionReset:
        this->ResetPlaylistCtrl();
        break;

    case MainFrame::UpdateActionRequery:
        this->RequeryPlaylistCtrl();
        break;

    case MainFrame::UpdateActionClear:
        this->ClearPlaylistCtrl();
        break;
    }
}

void MainFrame::UpdateTitle()
{
    CString s, caption;

    if (Globals::Player->IsPlaying())
    {
        s.Format(_T("[%s - %s] "), 
            Globals::Player->GetPlaying()->GetTitle().c_str(), 
            Globals::Player->GetPlaying()->GetArtist().c_str());

        if (Globals::Player->GetFormat() == MUSIK_LIBRARY_FORMAT_NET)
            s += _T(" (streaming audio) ");
    }

    caption += s;
    caption += _T(MUSIK_VERSION_STR);

    size_t task_count = Globals::Library->GetTaskCount();
    if (task_count)
    {      
        int type = Globals::Library->GetTaskType();
        switch (type)
        {
        case musik::Core::MUSIK_TASK_TYPE_BATCHADD:
            s.Format(_T(" (adding files, %d%%) "), m_Progress);
            break;
        case musik::Core::MUSIK_TASK_TYPE_REMOVEOLD:
            s.Format(_T(" (removing non-existent files, %d%%) "), m_Progress);
            break;
        case musik::Core::MUSIK_TASK_TYPE_BATCHRETAG:
            s.Format(_T(" (retagging files, %d%%) "), m_Progress);
            break;
        case musik::Core::MUSIK_TASK_TYPE_TAGFROMFN:
            s.Format(_T(" (converting filename to tag, %d%%) "), m_Progress);
            break;
        case musik::Core::MUSIK_TASK_TYPE_TAGTOFN:
            s.Format(_T(" (renaming files based on tag, %d%%) "), m_Progress);
            break;
        case musik::Core::MUSIK_TASK_TYPE_BATCHCHGCASE:
            s.Format(_T(" (changing tag case, %d%%) "), m_Progress);
            break;
        case musik::Core::MUSIK_TASK_TYPE_RELOAD_TAGS:
            s.Format(_T(" (reloading tags from file, %d%%) "), m_Progress);
            break;
        default:
            s.Format(_T(" (background processing) "));
            break;
        }
        caption += s;
    }

    if (this->m_LastCaption != caption)
    {
        SetWindowText(caption);
        this->m_LastCaption = caption;
    }
}

LRESULT MainFrame::OnSourcesStdPlaylist(WPARAM wParam, LPARAM lParam)
{
    musik::Core::Playlist* standardPlaylist = new musik::Core::Playlist();
    //
    // m_PlaylistInfo already set from OnSourcesItemClicked
    Globals::Library->GetStdPlaylist(
        this->m_PlaylistInfo.GetID(),
        *standardPlaylist,
        true);
    //
    this->m_PlaylistInfo = standardPlaylist->GetPlaylistInfo();      // m_Query will now be valid
    this->GivePlaylistToView(standardPlaylist);
    UpdateFilters(UpdateActionReset, UpdateActionRequery);
    //
    ShowSelectionBoxes(true);

    return 0L;
}

LRESULT MainFrame::OnSourcesDynPlaylist(WPARAM wParam, LPARAM lParam)
{
    musik::Core::Playlist* dynamicPlaylist = new musik::Core::Playlist();
    //
    // m_PlaylistInfo already set from OnSourcesItemClicked
    Globals::Library->QueryDynPlaylist(
        this->m_PlaylistInfo.GetID(),
        *dynamicPlaylist,
        true);
    //
    this->m_PlaylistInfo = dynamicPlaylist->GetPlaylistInfo();      // m_Query will now be valid
    this->GivePlaylistToView(dynamicPlaylist);
    this->UpdateFilters(UpdateActionReset, UpdateActionRequery);
    //
    this->ShowSelectionBoxes(true);

    return 0L;
}

BOOL MainFrame::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

void MainFrame::OnPaint()
{
    CPaintDC dc(this);
}

void MainFrame::OnSysColorChange()
{
    CFrameWnd::OnSysColorChange();

    RedrawWindow(
        NULL,
        NULL,
        RDW_ERASE|RDW_INVALIDATE|RDW_UPDATENOW|RDW_ALLCHILDREN
   );
}

void MainFrame::OnFilePreferences()
{
    // initialize the property pages
    InterfaceGeneralPreferences wndPageInterfaceGeneral;
    InterfaceWorkflowPreferences wndPageInterfaceWorkflow;
    InterfaceDevicesPreferences wndPageInterfaceDevices;
    InterfaceTransparencyPreferences wndPageInterfaceTrans;
    CrossfadePreferences wndPageSoundCrossfader;
    SoundDriverPreferences wndPageSoundDriver;
    TunagePreferences wndPageTunage;
    InternetPreferences wndPageInternet;
    AutoCapitalizePreferences wndPageAutoCapitalize;
    FileAssociationPreferences wndFileAssociations;

    // remove help icon from gripper
    wndPageInterfaceGeneral.m_psp.dwFlags&=     ~PSP_HASHELP;
    wndPageInterfaceWorkflow.m_psp.dwFlags&=    ~PSP_HASHELP;
    wndPageInterfaceDevices.m_psp.dwFlags&=     ~PSP_HASHELP;
    wndPageInterfaceTrans.m_psp.dwFlags&=       ~PSP_HASHELP;
    wndPageSoundCrossfader.m_psp.dwFlags&=      ~PSP_HASHELP;
    wndPageSoundDriver.m_psp.dwFlags&=          ~PSP_HASHELP;
    wndPageTunage.m_psp.dwFlags&=               ~PSP_HASHELP;
    wndPageInternet.m_psp.dwFlags&=             ~PSP_HASHELP;
    wndPageAutoCapitalize.m_psp.dwFlags&=       ~PSP_HASHELP;
    wndFileAssociations.m_psp.dwFlags&=          ~PSP_HASHELP;

    // initialize the CTreePropSheet class 
    // and remove help icon from gripper
    PreferencesPropertySheet PrefSheet(_T("Preferences - musikCube"));
    PrefSheet.m_psh.dwFlags&= ~PSH_HASHELP;
    PrefSheet.m_psh.dwFlags|= PSH_NOCONTEXTHELP|PSH_NOAPPLYNOW;

    // physically add the preference sheets
    PrefSheet.AddPage(&wndPageInterfaceGeneral);
    PrefSheet.AddPage(&wndPageInterfaceWorkflow);
    PrefSheet.AddPage(&wndPageInterfaceDevices);
    PrefSheet.AddPage(&wndPageInterfaceTrans);
    PrefSheet.AddPage(&wndPageSoundCrossfader);
    PrefSheet.AddPage(&wndPageSoundDriver);
    PrefSheet.AddPage(&wndPageTunage);
    PrefSheet.AddPage(&wndPageInternet);
    PrefSheet.AddPage(&wndPageAutoCapitalize);
    PrefSheet.AddPage(&wndFileAssociations);

    PrefSheet.SetEmptyPageText(_T("Please select one of the items under \"%s.\""));

    // remember the amount of hidden devices we started with
    int hiddencnt = Globals::Preferences->GetHiddenDevices().size();

    // set some miscellaneous tree view properties
    PrefSheet.SetTreeViewMode(true, true, false);
    PrefSheet.SetActivePage(&wndPageInterfaceGeneral);
    PrefSheet.DoModal();

    //
    // update ui after preferences
    //

    if (!Globals::Preferences->GetShowBalloonTips())
    {
        wcscpy(m_TrayIcon.szInfoTitle, _T(""));
        wcscpy(m_TrayIcon.szInfo, _T(""));
        Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);
    }

    if (!Globals::Preferences->IsTransEnabled() && m_Trans != 255)
        SetTransparency(255);

    if (Globals::Preferences->MinimizeToTray() || Globals::Preferences->GetCloseToSystemTray())
    {
        if (!m_TrayIconVisible)
        {
            ShowTrayIcon();
        }
    }
    else if (m_TrayIconVisible)
    {
        HideTrayIcon();
    }

    // update the player's buffer size
    Globals::Player->SetBufferSize(Globals::Preferences->GetPlayerBuffer());

    // hack! requery the playlist view if UTC has changed.
    this->UpdateFilters(UpdateActionNone, UpdateActionRequery);
} 

void MainFrame::OnMenuAddFiles()
{
    // string used to populate the File Types dropdown box. the first
    CString str_types = _T("Supported Media Files |");
    const musik::Core::StringArray& formats = musik::Core::GetPlayerFormats();

    for (size_t i = 0; i < formats.size(); i++)
    {
        CString curr_type = formats.at(i);
        if (curr_type == _T("cdda") || curr_type == _T("net"))
            continue;

        str_types += _T("*.");
        str_types += curr_type;
        str_types += _T(";");
    }

    str_types += _T("|");

    for (size_t i = 0; i < formats.size(); i++)
    {
        CString curr_type = formats.at(i);
        if (curr_type == _T("cdda") || curr_type == _T("net"))
        {
            continue;
        }

        str_types += curr_type.MakeUpper();
        str_types += _T(" files (*.");
        str_types += curr_type.MakeLower();
        str_types += _T(")|*.");
        str_types += curr_type;
        str_types += _T("|");
    }

    str_types += _T("|");

    // create the open dialog object
    FileDialog opendlg(
        TRUE, 
        _T("mp3"), 
        NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
        str_types);

    if (opendlg.DoModal() == IDOK)
    {
        musik::Core::StringArray* files = new musik::Core::StringArray();

        POSITION posCurr = opendlg.GetStartPosition();
        while (posCurr)
            files->push_back((musik::Core::String)opendlg.GetNextPathName(posCurr));

        if (files->size() > 0)
        {
            // setup params
            musik::Core::BatchAdd* params = new musik::Core::BatchAdd(
                files, 
                "",
                NULL, 
                Globals::MainLibrary, 
                NULL, 
                Globals::Functor, 
                0, 
                0, 
                1);

            // spawn task
            musik::Core::BatchAddTask* task = new musik::Core::BatchAddTask;
            task->m_Params = params;
            Globals::MainLibrary->QueueTask(task);
        }
        else
            delete files;
    }
}

LRESULT MainFrame::OnTaskProgress(WPARAM wParam, LPARAM lParam)
{
    m_Progress = (int)wParam;
    return 0L;
}

LRESULT MainFrame::OnTaskEnd(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateActionRequery, UpdateActionQuery);
    m_Progress = 0;

    return 0L;
}

void MainFrame::OnMenuAddDirectory()
{
    TCHAR path[MAX_PATH];
    BROWSEINFO bi = { 0 };
    bi.lpszTitle = _T("Please select a directory containing music.");
    bi.hwndOwner = GetSafeHwnd();

    LPITEMIDLIST pidl = SHBrowseForFolder (&bi);

    if (pidl != 0)
    {
        // get the name of the folder and put it in path
        SHGetPathFromIDList (pidl, path);

        // free memory used
        IMalloc * imalloc = 0;
        if (SUCCEEDED(SHGetMalloc (&imalloc)))
        {
            imalloc->Free (pidl);
            imalloc->Release ();
        }

        musik::Core::StringArray* files = new musik::Core::StringArray();

        // get all the musik related files
        // in the directory we just found...
        musik::Core::String sPath = path;
        if (sPath.Right(1) != _T("\\"))
            sPath += _T("\\*.*");
        else
            sPath += _T("*.*");

        // setup params
        musik::Core::BatchAdd* params = new musik::Core::BatchAdd(
            files, 
            sPath,
            NULL, 
            Globals::MainLibrary, 
            NULL, 
            Globals::Functor, 
            0, 
            0, 
            1);
        
        // start it up
        musik::Core::BatchAddTask* task = new musik::Core::BatchAddTask;
        task->m_Params = params;
        Globals::MainLibrary->QueueTask(task);
    }
}

LRESULT MainFrame::OnPlayerPlaySel(WPARAM wParam, LPARAM lParam)
{
    // player is paused, so resume...
    if (Globals::Player->IsPlaying() && Globals::Player->IsPaused())
        Globals::Player->Resume();
    
    else
    {
        if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM)
        {
            int first = musik::Core::Library::Rand(0, (int)Globals::Player->GetPlaylist()->GetCount());
            Globals::Player->Play(first);
        }

        else if (!m_PlaylistView->GetCtrl()->PlayItem())
            Globals::Player->Play(0);
    }

    return 0L;
}

LRESULT MainFrame::OnVerifyPlaylist(WPARAM wParam, LPARAM lParam)
{
    musik::Core::Playlist* playlist = (musik::Core::Playlist*)wParam;

    if (playlist == NULL || m_PlaylistView->GetCtrl()->GetPlaylist() == NULL)
        return 0L;

    if (playlist == m_PlaylistView->GetCtrl()->GetPlaylist())
        return 1L;

    return 0L;
}

void MainFrame::OnViewSources()
{
    if (m_SourcesView->IsVisible())
        ShowControlBar(m_SourcesView, FALSE, FALSE);
    else
        ShowControlBar(m_SourcesView, TRUE, FALSE);
}

void MainFrame::OnUpdateViewSources(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_SourcesView->IsVisible());
}

void MainFrame::ShowSelectionBoxes(bool show)
{
    if (Globals::Preferences->ShowSelectionBoxes() == false)
    {
        show = false;
    }

    if (show)
    {
        CSizingControlBar::GlobalLoadState(this, _T("musikProfile0.90"));
    }
    else
    {
        CSizingControlBar::GlobalSaveState(this, _T("musikProfile0.90"));
    }

    for (size_t i = 0; i < m_AllFilterViews.size(); i++)
    {
        ShowControlBar(m_AllFilterViews[i], show, TRUE);
    }
}

void MainFrame::OnViewNowplaying()
{
    if (m_NowPlayingView->IsVisible())
        ShowControlBar(m_NowPlayingView, FALSE, FALSE);
    else
        ShowControlBar(m_NowPlayingView, TRUE, FALSE);
}

void MainFrame::OnUpdateViewNowplaying(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_NowPlayingView->IsVisible());
}

void MainFrame::OnAudioEqualizerEnabled()
{
    if (Globals::Preferences->IsEqualizerEnabled())
    {
        Globals::Preferences->SetEqualizerEnabled(false);
        Globals::Player->EnableEqualizer(false, Globals::Preferences->EqualizerUseGlobal());
    }
    else
    {
        Globals::Preferences->SetEqualizerEnabled(true);
        Globals::Player->EnableEqualizer(true, Globals::Preferences->EqualizerUseGlobal());
        musik::Core::SetEQPreampLevel(Globals::Preferences->GetEqualizerMultiplier());
        Globals::Player->UpdateEQSettings();
    }

    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

void MainFrame::OnUpdateAudioEqualizerEnabled(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(Globals::Preferences->IsEqualizerEnabled());
}

void MainFrame::OnUpdateAudioCrossfaderEnabled(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(Globals::Preferences->IsCrossfaderEnabled());
}

void MainFrame::OnAudioCrossfaderEnabled()
{
    if (Globals::Preferences->IsCrossfaderEnabled())
    {
        Globals::Preferences->SetCrossfaderEnabled(false);
        Globals::Player->EnableCrossfader(false);
    }
    else
    {
        Globals::Preferences->SetCrossfaderEnabled(true);

        musik::Core::CrossfadeSettings fade;

        // function to get default crossfader
        musikCubeApp* ptrApp = (musikCubeApp*)AfxGetApp();
        ptrApp->GetCrossfader(&fade);

        Globals::Player->EnableCrossfader(true);
        Globals::Player->SetCrossfader(fade);
    }

    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

void MainFrame::OnViewPlaylistinformation()
{
    if (Globals::Preferences->PlaylistInfoVisible())
    {
        Globals::Preferences->SetPlaylistInfoVisible(false);
    
        if (!Globals::Preferences->GetPlaylistInfoVizStyle())
        {
            m_PlaylistView->SetUsePlaylistInfo(false); 
            m_PlaylistView->DeinitPlaylistInfo();
        }
    }
    else
    {
        m_PlaylistView->SetUsePlaylistInfo(true);    
        Globals::Preferences->SetPlaylistInfoVisible(true);
    }
    // send a "dummy" size event to the
    // window, so it re-creates or removes
    // the now playing window.
    CRect lpRect;
    m_PlaylistView->GetClientRect(&lpRect);
    m_PlaylistView->OnSize(NULL, lpRect.Width(), lpRect.Height());
    RedrawWindow();
    m_PlaylistView->UpdatePlaylistInfo();

}

void MainFrame::OnUpdateViewPlaylistinformation(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(Globals::Preferences->PlaylistInfoVisible());
}

void MainFrame::OnUpdatePlaybackmodeRepeatsingle(CCmdUI *pCmdUI)
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

void MainFrame::OnPlaybackmodeRepeatsingle()
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE)
        Globals::Player->ModifyPlaymode(NULL, musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE);
    else
        Globals::Player->ModifyPlaymode(musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_SINGLE, NULL);

    Globals::Preferences->SetPlayerPlaymode(Globals::Player->GetPlaymode());
}

void MainFrame::OnUpdatePlaybackmodeRepeatplaylist(CCmdUI *pCmdUI)
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

void MainFrame::OnPlaybackmodeRepeatplaylist()
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST)
        Globals::Player->ModifyPlaymode(NULL, musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST);
    else
        Globals::Player->ModifyPlaymode(musik::Core::MUSIK_PLAYER_PLAYMODE_REPEAT_PLAYLIST, NULL);

    Globals::Preferences->SetPlayerPlaymode(Globals::Player->GetPlaymode());
    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

void MainFrame::OnUpdatePlaybackmodeIntro(CCmdUI *pCmdUI)
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_INTRO)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

void MainFrame::OnPlaybackmodeIntro()
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_INTRO)
        Globals::Player->ModifyPlaymode(NULL, musik::Core::MUSIK_PLAYER_PLAYMODE_INTRO);
    else
        Globals::Player->ModifyPlaymode(musik::Core::MUSIK_PLAYER_PLAYMODE_INTRO, NULL);

    Globals::Preferences->SetPlayerPlaymode(Globals::Player->GetPlaymode());
}

void MainFrame::OnMenuViewModifiedTags()
{
    ShowSelectionBoxes(true);
    m_SourcesView->DeselectAllItems();

    musik::Core::Playlist* unsyncTags = new musik::Core::Playlist();
    Globals::Library->GetDirtySongs(*unsyncTags);
    //
    this->m_PlaylistInfo = unsyncTags->GetPlaylistInfo();
    this->GivePlaylistToView(unsyncTags);
    this->UpdateFilters(UpdateActionReset, UpdateActionRequery);
}

void MainFrame::OnUnsynchronizedtagsWritetofile()
{
    // get dirty items into a new playlist
    musik::Core::Playlist playlist;
    Globals::Library->GetDirtySongs(playlist);

    // create a new musik::Core::SongInfoArray and update
    // all the respective values...
    if (playlist.GetCount())
    {
        musik::Core::SongInfoArray* pSongInfoArray = new musik::Core::SongInfoArray();
        Globals::Library->GetInfoArrayFromPlaylist(playlist, *pSongInfoArray);

        // setup params
        musik::Core::BatchRetag* params = new musik::Core::BatchRetag(
            Globals::Library, 
            Globals::Functor,
            pSongInfoArray);
        params->m_WriteToFile = true;

        // setup and fire off task
        musik::Core::BatchRetagTask* task = new musik::Core::BatchRetagTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
}

void MainFrame::OnUnsynchronizedtagsFinalizefordatabaseonly()
{
    Globals::Library->FinalizeDirtySongs();
}

void MainFrame::SendToTray()
{
    SaveWindowState();
    ShowWindow(SW_MINIMIZE);
    m_IsMinimized = true;
}

void MainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
    if (nID == SYSMENU_RESETDIALOG)
    {
        this->ResetDialogPosition();
        return;
    }

    if (nID == SC_MINIMIZE)
    {
        SendToTray();
        if (Globals::Preferences->MinimizeToTray())
            ShowWindow(SW_HIDE);
        return;
    }

    if (nID == SC_CLOSE)
    {
        if (Globals::Preferences->GetCloseToSystemTray())
        {
            SendToTray();
            if (Globals::Preferences->GetCloseToSystemTray())
                ShowWindow(SW_HIDE);
            return;
        }
    }

    // dunno what it is, but 61730 gets
    // called when double clicking the title
    // bar to restore...
    else if (nID == SC_RESTORE || nID == 61730)
    {
        if (m_FullScreen)
        {
            KillFullScreen();
            return;
        }
        else if (m_IsMinimized)
        {
            ShowWindow(SW_RESTORE);
            RestoreDialogPosition();
        }
        else
        {
            ShowWindow(SW_RESTORE);
        }

        return;
    }

    else if (nID == SC_MAXIMIZE /*|| nID == 61458*/)
        SaveWindowState();

    CFrameWnd::OnSysCommand(nID, lParam);
}

void MainFrame::OnViewEqualizer()
{
    if (m_EqualizerView->IsVisible())
        ShowControlBar(m_EqualizerView, FALSE, FALSE);
    else
        ShowControlBar(m_EqualizerView, TRUE, FALSE);
}

void MainFrame::OnUpdateViewEqualizer(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_EqualizerView->IsVisible());
}

void MainFrame::OnFileSynchronizeddirectories()
{
    DirectorySyncDialog dirSyncDlg(this, Globals::MainLibrary);
    dirSyncDlg.DoModal();
}

void MainFrame::InitTrayIcon()
{
    ZeroMemory(&m_TrayIcon, sizeof(NOTIFYICONDATA));

    m_TrayIcon.cbSize = sizeof(NOTIFYICONDATA); 
    m_TrayIcon.hWnd   = GetSafeHwnd(); 
    m_TrayIcon.uID    = ID_NOTIFY_ICON;
    m_TrayIcon.hIcon  = m_hIcon16; 
    m_TrayIcon.uCallbackMessage = TRAY_NOTIFY_EVT; 
    m_TrayIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_INFO; 
    m_TrayIcon.dwInfoFlags = NIIF_INFO;
    m_TrayIcon.uTimeout = 5000;

    wcscpy (m_TrayIcon.szTip, _T(MUSIK_VERSION_STR));

    m_TrayIconVisible = false;
}

void MainFrame::ShowTrayIcon()
{
    if (!m_TrayIconVisible)
    {
        Shell_NotifyIcon(NIM_ADD, &m_TrayIcon);   
        m_TrayIconVisible = true;
    }
}

void MainFrame::ShowBalloonTip(const CString& str_title, const CString& str_info)
{
    CString title = str_title;
    if (title.GetLength() > 63)
    {
        title = title.Left(59);
        title += _T("...");
    }

    CString info = str_info;
    if (info.GetLength() > 250)
    {
        info = info.Left(250);
        info += _T("...");
    }

    // show it
    wcsncpy(m_TrayIcon.szInfoTitle, title, 64);
    wcsncpy(m_TrayIcon.szInfo, info, 256);
    m_TrayIcon.uTimeout = 2000;
    m_TrayIcon.dwInfoFlags |= NIIF_NOSOUND;
    Shell_NotifyIcon(NIM_MODIFY, &m_TrayIcon);
}

void MainFrame::HideTrayIcon()
{
    if (m_TrayIconVisible)
    {
        m_TrayIcon.uFlags = NIF_ICON;
        Shell_NotifyIcon(NIM_DELETE, &m_TrayIcon);
        m_TrayIconVisible = false;
    }
}

LRESULT MainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    // process messages from the tray
    if (message == TRAY_NOTIFY_EVT)
    {
        switch(lParam)
        {
        case WM_MBUTTONDOWN:
            {
                if (m_IsMinimized)
                    RestoreFromTray();
                else
                    ::SetForegroundWindow(GetSafeHwnd());
            }
            break;

        case WM_LBUTTONDOWN:
            {
                if (m_IsMinimized)
                    RestoreFromTray();
                else
                {
                    SendToTray();
                    if (Globals::Preferences->MinimizeToTray())
                        ShowWindow(SW_HIDE);
                }

                return 1L;
            }
            break;

        case WM_RBUTTONDOWN:
        case WM_CONTEXTMENU:
            {
                CPoint pos;
                ::GetCursorPos(&pos);

                CMenu main_menu;
                CMenu* popup_menu;

                main_menu.LoadMenu(IDR_TRAY_MENU);
                popup_menu = main_menu.GetSubMenu(0);
                
                // check the correct rating
                if (Globals::Player->IsPlaying())
                {
                    int rating = musik::Core::StringToInt(
                        Globals::Player->GetPlaying()->GetRating());

                    switch(rating)
                    {   
                    case -1:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_N1, MF_CHECKED);
                        break;
                    case 0:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_UNRATED, MF_CHECKED);
                        break;
                    case 1:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_1, MF_CHECKED);
                        break;
                    case 2:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_2, MF_CHECKED);
                        break;
                    case 3:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_3, MF_CHECKED);
                        break;
                    case 4:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_4, MF_CHECKED);
                        break;
                    case 5:
                        popup_menu->CheckMenuItem(ID_RATECURRENT_5, MF_CHECKED);
                        break;
                    }
                }

                SetForegroundWindow();
                popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
                PostMessage(WM_NULL, 0, 0);

                return 1L;
            }
            break;
        }
    }

    return CFrameWnd::WindowProc(message, wParam, lParam);
}

void MainFrame::RestoreFromTray()
{
    RestoreDialogPosition();
    SetForegroundWindow();
    SetFocus();

    if (m_AutoStart)
    {
        if (Globals::Preferences->SynchronizeOnStartup())
        {
            Globals::SynchronizeDirs(Globals::Library);
        }

        m_AutoStart = false;
    }
}

void MainFrame::RestoreDialogPosition()
{
    CRect rcNormal = CRect(Globals::Preferences->GetDlgPos(), Globals::Preferences->GetDlgSize());

    // kludge.. i cannot figure out for the
    // life of me why this is happening
    if (rcNormal.left == -32000 || rcNormal.top == -32000)
    {
        rcNormal.left = 0;
        rcNormal.top = 0;
        rcNormal.right = 800;
        rcNormal.bottom = 600;
    }

    if (m_IsMinimized && Globals::Preferences->IsMaximized())
    {
        ShowWindow(SW_NORMAL);
        MoveWindow(rcNormal);

        WINDOWPLACEMENT max;
        max.showCmd = SW_MAXIMIZE;
        max.rcNormalPosition = rcNormal;
        SetWindowPlacement(&max);

        Globals::Preferences->SetMaximized(true);
    }
    else
    {
        ShowWindow(SW_NORMAL);
        MoveWindow(rcNormal);
        Globals::Preferences->SetMaximized(false);
    }

    m_IsMinimized = false;
}

void MainFrame::OnViewResetinterfacetodefault()
{
    ResetDialogPosition();
}

void MainFrame::OnNotificationtrayExit()
{
    RestoreFromTray();
    OnClose();
}

void MainFrame::OnNotificationtrayRestore()
{
    RestoreFromTray();
}

void MainFrame::OnUpdateNotificationtrayPlay(CCmdUI *pCmdUI)
{
    if (Globals::Player->IsPlaying())
        pCmdUI->Enable(false);
    else
        pCmdUI->Enable(true);
}

void MainFrame::OnNotificationtrayPlay()
{
    if (Globals::Player->GetPlaylist())
        Globals::Player->Play(0);
}

void MainFrame::OnUpdateNotificationtrayPause(CCmdUI *pCmdUI)
{
    if (Globals::Player->IsPlaying() && !Globals::Player->IsPaused())
        pCmdUI->SetText(_T("Pause"));
    else
        pCmdUI->SetText(_T("Resume"));  

    if (Globals::Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

void MainFrame::OnNotificationtrayPause()
{
    if (Globals::Player->IsPlaying() && !Globals::Player->IsPaused())
        Globals::Player->Pause();
    else
        Globals::Player->Resume();
}

void MainFrame::OnUpdateNotificationtrayNext(CCmdUI *pCmdUI)
{
    if (Globals::Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

void MainFrame::OnNotificationtrayNext()
{
    if (Globals::Player->IsPlaying())
        Globals::Player->Next();
}

void MainFrame::OnUpdateNotificationtrayPrev(CCmdUI *pCmdUI)
{
    if (Globals::Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

void MainFrame::OnNotificationtrayPrev()
{
    if (Globals::Player->IsPlaying())
        Globals::Player->Prev();
}

void MainFrame::OnUpdateNotificationtrayStop(CCmdUI *pCmdUI)
{
    if (Globals::Player->IsPlaying())
        pCmdUI->Enable(true);
    else
        pCmdUI->Enable(false);
}

void MainFrame::OnNotificationtrayStop()
{
    if (Globals::Player->IsPlaying())
        Globals::Player->Stop();
}

LRESULT MainFrame::OnSelBoxAddRemove(WPARAM wParam, LPARAM lParam)
{
    BOOL add_new = (BOOL)wParam;
    bool save = false;

    MetadataFilterViewContainer* pBar;

    if (add_new)
    {
        pBar = new MetadataFilterViewContainer(this, musik::Core::MUSIK_LIBRARY_TYPE_ARTIST, 123 + m_AllFilterViews.size(), m_uSelectionDrop_L, m_uSelectionDrop_R);
        pBar->Create(_T("musik Selection Box"), this, ID_SELECTIONBOX_START + m_AllFilterViews.size());
        FloatControlBar(pBar, CPoint(0, 0));

        m_AllFilterViews.push_back(pBar);
        this->UpdateFilters(UpdateActionRequery, UpdateActionNone);

        save = true;
    }

    else
    {
        pBar = (MetadataFilterViewContainer*)lParam;

        // remove the bar
        for (size_t i = 0; i < m_AllFilterViews.size(); i++)
        {
            if (pBar == m_AllFilterViews.at(i))
            {
                m_AllFilterViews.at(i)->DestroyWindow();
                delete m_AllFilterViews.at(i);
                m_AllFilterViews.erase(m_AllFilterViews.begin() + i);

                save = true;
                break;
            }
        }

        RecalcLayout();
    }

    // save new state to prefs
    if (save)
    {
        musik::Core::IntArray new_selboxes;
        for (size_t i = 0; i < m_AllFilterViews.size(); i++)
            new_selboxes.push_back(m_AllFilterViews.at(i)->GetCtrl()->GetType());

        Globals::Preferences->SetSelBoxTypes(new_selboxes);
    }

    return 0L;
}

void MainFrame::KillTasks(bool updater, bool helpers)
{
    if (updater)
    {
        if (m_Updater)
        {
            m_Updater->StopWait(4);

            delete m_Updater;
            m_Updater = NULL;
        }
    }

    if (helpers)
    {
        Globals::Library->FlushTaskQueue();
    }
}

void MainFrame::OnMenuEraseLibrary()
{
    int prompt = MessageBox(
        _T("Are you sure you want to erase your music library?"), 
        _T(MUSIK_VERSION_STR), 
        MB_ICONWARNING | MB_YESNO);

    if (prompt != IDYES)
    {
        return;
    }

    // kill all tasks that may be accessing the database
    KillTasks(true, true);
    this->UpdateTitle();

    // stop player
    Globals::Player->Stop();

    while (Globals::Player->IsPlaying() || Globals::Player->IsPaused())
    {
        Sleep(50);
    }

    Globals::Player->GetPlaylist()->Clear();

    // clear
    Globals::MainLibrary->ClearLibrary(true);

    m_SourcesView->ReloadAllItems();
    m_SourcesView->FocusLibrary();

    OnSourcesLibrary(NULL, NULL);
    UpdateFilters(UpdateActionReset, UpdateActionNone);

    // start the updater again
    m_Updater = new MainFrameUpdater;
    m_Updater->m_Parent = this;
    m_Updater->start();
}

void MainFrame::OnMenuSynchronize()
{
    Globals::SynchronizeDirs(Globals::MainLibrary);
}

BOOL MainFrame::PreTranslateMessage(MSG* pMsg)
{
    switch(pMsg->message)
    {
    case WM_KEYDOWN:
        {
            if (pMsg->wParam == VK_ESCAPE && Globals::Library->GetTaskCount())
            {
                KillTasks(false, true);
                this->UpdateTitle();
                return TRUE;
            }

            if (pMsg->wParam == VK_F5)
            {
                musik::Core::Playlist* playlist = m_PlaylistView->GetCtrl()->GetPlaylist();
                if (playlist && playlist->GetType() == musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC)
                {
                    Globals::Library->QueryDynPlaylist(
                        playlist->GetID(),
                        *m_PlaylistView->GetCtrl()->GetPlaylist());

                    // its possible that we re-arranged the now playing
                    // playlist. if we did, have it relocate the currently
                    // playing song. also, after querying a dynamic playlist
                    // it's type becomes MUSIK_PLAYLIST_TYPE_DYNAMIC, we need to
                    // toggle it back to MUSIK_PLAYLIST_TYPE_NOWPLAYING.
                    if (m_PlaylistView->GetCtrl()->GetPlaylist() == Globals::Player->GetPlaylist())
                    {
                        int playid = Globals::Player->GetPlaying()->GetID();
                        Globals::Player->FindNewIndex(playid);
                        Globals::Player->GetPlaylist()->SetType(musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING);
                    }

                    m_PlaylistView->GetCtrl()->Refresh();
                    return TRUE;
                }

            }

            if ((wchar_t)pMsg->wParam == 'f' || (wchar_t)pMsg->wParam == 'F')
            {
                if (GetKeyState(VK_CONTROL) < 0)
                {
                    m_SourcesView->FocusQuickSearch();
                    m_SourcesView->EnableQuickSearch();
                    return TRUE;
                }
            }
        }
        break;

    case WM_APPCOMMAND:
        {
            switch(GET_APPCOMMAND_LPARAM(pMsg->lParam))
            {

            case APPCOMMAND_MEDIA_NEXTTRACK:
                {
                    if (Globals::Player->IsPlaying())
                        Globals::Player->Next();
                }
                break;

            case APPCOMMAND_MEDIA_PREVIOUSTRACK:
                {
                    if (Globals::Player->IsPlaying())
                        Globals::Player->Prev();
                }
                break;

            case APPCOMMAND_MEDIA_STOP:
                {
                    if (Globals::Player->IsPlaying())
                        Globals::Player->Stop();
                }
                break;

            case APPCOMMAND_MEDIA_PLAY_PAUSE:
                {
                    if (Globals::Player->IsPlaying())
                    {
                        if (!Globals::Player->IsPaused())
                        {
                            Globals::Player->Pause();
                        }
                        else
                        {
                            Globals::Player->Resume();
                        }
                    }
                    else
                    {
                        musik::Core::Playlist* nowPlayingPlaylist = Globals::Player->GetPlaylist();
                        if (nowPlayingPlaylist->GetCount() == 0)
                        {
                            Globals::Library->GetAllSongs(*nowPlayingPlaylist);

                            if (m_PlaylistView->GetCtrl()->GetPlaylist() == nowPlayingPlaylist)
                            {
                                UpdateFilters(UpdateActionNone, UpdateActionRequery);
                            }
                        }

                        Globals::Player->Play();
                    }
                }
                break;
            }

            return true;
        }
        break;
    }

    return CFrameWnd::PreTranslateMessage(pMsg);
}

LRESULT MainFrame::OnUpdateCurrPlaylist(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateActionNone, UpdateActionRequery);
    m_PlaylistView->GetCtrl()->ScrollToCurr();
    return 0L;
}

void MainFrame::OnPlaybackmodeShufflecurrentplaylist()
{
    Globals::Player->Shuffle();

    if (m_PlaylistView->GetCtrl()->GetPlaylist() == Globals::Player->GetPlaylist())
    {
        UpdateFilters(UpdateActionNone, UpdateActionRequery);
        m_PlaylistView->GetCtrl()->ScrollToCurr();
    }
}

LRESULT MainFrame::OnSelectionCtrlDeleteSelected(WPARAM wParam, LPARAM lParam)
{
    // setup vars
    MetadataFilterView* pSel = (MetadataFilterView*)wParam;
    BOOL fromComputer = (BOOL)lParam;
    BOOL fromLibrary = TRUE;

    // the selected items are whats currently visible in the playlist
    musik::Core::Playlist playlist;
    this->QueryPlaylist(playlist);

    if (fromComputer)
    {
        CShellFileOp deleteFileOp;
        deleteFileOp.SetOperationFlags(FO_DELETE, this, FOF_ALLOWUNDO);
        //
        musik::Core::String filename;
        for (size_t i = 0; i < playlist.GetCount(); i++)
        {
            Globals::Library->GetFieldFromID(
                playlist.GetSong(i).GetID(), 
                musik::Core::MUSIK_LIBRARY_TYPE_FILENAME, 
                filename);
            //
            if (musik::Core::Filename::FileExists(filename))
            {
                deleteFileOp.AddSourceFile(filename.c_str());
            }
        }

        BOOL notUsed, apiResult, aborted;
        if (deleteFileOp.GetSourceFileList().GetCount())
        {
            deleteFileOp.Go(&notUsed, &apiResult, &aborted);
            if (aborted || apiResult)
            {
                fromLibrary = false;
            }
        }
    }
    
    // delete from library
    if (fromLibrary)
    {
        Globals::Library->DeleteSongs(playlist);
        UpdateFilters(UpdateActionRequery, UpdateActionQuery);

        // notify Player to update its internal playlist
        Globals::Player->UpdatePlaylist();
    }

    return 0L;
}

void MainFrame::OnUpdatePlaybackmodeShufflecurrentplaylist(CCmdUI *pCmdUI)
{
    pCmdUI->Enable(Globals::Player->GetPlaylist()->GetCount());
}

void MainFrame::OnViewVisualization()
{
    if (Globals::Preferences->GetPlaylistInfoVizStyle())
    {
        Globals::Preferences->SetPlaylistInfoVizStyle(NULL);

        if (!Globals::Preferences->PlaylistInfoVisible())
        {
            m_PlaylistView->SetUsePlaylistInfo(false);
            m_PlaylistView->DeinitPlaylistInfo();
        }
    }
    else
    {
        Globals::Preferences->SetPlaylistInfoVizStyle(1);
        m_PlaylistView->SetUsePlaylistInfo(true);
        m_PlaylistView->InitPlaylistInfo();
    }

    // send a "dummy" size event to the
    // window, so it re-creates or removes
    // the now playing window.
    CRect lpRect;
    m_PlaylistView->GetClientRect(&lpRect);
    m_PlaylistView->OnSize(NULL, lpRect.Width(), lpRect.Height());
    RedrawWindow();
    m_PlaylistView->UpdatePlaylistInfo();

}

void MainFrame::OnUpdateViewVisualization(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(Globals::Preferences->GetPlaylistInfoVizStyle());
}

void MainFrame::OnViewAlwaysontop()
{
    SetRedraw(FALSE);

    CRect rcWnd;
    GetWindowRect(rcWnd);

    if (Globals::Preferences->IsAlwaysOnTop())
    {
        ::SetWindowPos(m_hWnd, 
            HWND_NOTOPMOST, 
            rcWnd.TopLeft().x, 
            rcWnd.TopLeft().y, 
            rcWnd.Width(), 
            rcWnd.Height(), 
            NULL);
        m_EqualizerView->GetCtrl()->OnAlwaysOnTop();
        Globals::Preferences->SetAlwaysOnTop(false);
    }

    else
    {
        ::SetWindowPos(m_hWnd, 
            HWND_TOPMOST, 
            rcWnd.TopLeft().x, 
            rcWnd.TopLeft().y, 
            rcWnd.Width(), 
            rcWnd.Height(), 
            NULL);

        Globals::Preferences->SetAlwaysOnTop(true);
    }

    SetRedraw(TRUE);
}

void MainFrame::OnUpdateViewAlwaysontop(CCmdUI *pCmdUI)
{
    if (Globals::Preferences->IsAlwaysOnTop())
        pCmdUI->SetCheck(TRUE);
    else
        pCmdUI->SetCheck(FALSE);
}

void MainFrame::GoFullScreen()
{
    m_GoingFullScreen = true;

    // backup original size and state
    CSizingControlBar::GlobalSaveState(this, _T("musikProfile0.90"));
    SaveBarState(_T("musikProfile0.90"));
    SaveWindowState();

    // maximize and grab window size
    CRect rcWnd(Globals::Preferences->GetDlgPos(), Globals::Preferences->GetDlgSize());

    WINDOWPLACEMENT max;
    max.showCmd = SW_MAXIMIZE;
    max.rcNormalPosition = rcWnd;
    SetWindowPlacement(&max);

    GetWindowRect(rcWnd);

    // grab desktop size
    int cx, cy; 
    HDC dc = ::GetDC(NULL); 
    cx = GetDeviceCaps(dc,HORZRES) + 
        GetSystemMetrics(SM_CXBORDER); 
    cy = GetDeviceCaps(dc,VERTRES) +
        GetSystemMetrics(SM_CYBORDER); 
    ::ReleaseDC(0, dc); 

    // remove caption and border
    SetWindowLong(m_hWnd, 
        GWL_STYLE, 
        GetWindowLong(m_hWnd, GWL_STYLE) & (~(WS_CAPTION | WS_BORDER))); 

    // Put window on top and expand it to fill screen
    ::SetWindowPos(m_hWnd, 
        HWND_TOPMOST, 
        rcWnd.TopLeft().x + GetSystemMetrics(SM_CXBORDER) + 1, 
        rcWnd.TopLeft().y + GetSystemMetrics(SM_CYBORDER) + 1,
        cx + 1,
        cy + 1, 
        NULL); 

    m_FullScreen = true;
    m_GoingFullScreen = false;
}

void MainFrame::KillFullScreen()
{
    // restore
    WINDOWPLACEMENT max;
    max.showCmd = SW_RESTORE;
    SetWindowPlacement(&max);

    // add caption bar and border
    SetWindowLong(m_hWnd, GWL_STYLE, 
        GetWindowLong(m_hWnd, GWL_STYLE) | (WS_CAPTION | WS_BORDER));

    // restore window "always on top" status...
    ::SetWindowPos(m_hWnd, 
        Globals::Preferences->IsAlwaysOnTop() ? HWND_TOPMOST : HWND_NOTOPMOST, 
        Globals::Preferences->GetDlgPos().x, 
        Globals::Preferences->GetDlgPos().y, 
        Globals::Preferences->GetDlgSize().cx,
        Globals::Preferences->GetDlgSize().cy, 
        NULL); 

    // set position
    MoveWindow(CRect(Globals::Preferences->GetDlgPos(), Globals::Preferences->GetDlgSize()));

    // restore prior window state
    if (VerifyBarState(_T("musikProfile0.90")))
    {
        CSizingControlBar::GlobalLoadState(this, _T("musikProfile0.90"));
        LoadBarState(_T("musikProfile0.90"));
    }

    m_FullScreen = false;
}

void MainFrame::OnViewFullscreen()
{
    m_FullScreen ? KillFullScreen() : GoFullScreen();
}


void MainFrame::OnUpdateViewFullscreen(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck(m_FullScreen == true);
}

void MainFrame::OnMenuAddNetRadio()
{
    AddNetstreamDialog dlg(Globals::MainLibrary);
    if (dlg.DoModal() == IDOK)
    {
        PropertyTreeItem* pItem = m_SourcesView->GetFocusedItem();
        if (pItem && pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_NETSTREAMS)
        {
            this->OnSourcesNetRadio(NULL, NULL);
        }
    }
}

LRESULT MainFrame::OnPlaySong(WPARAM wParam, LPARAM lParam)
{
    ATOM atom = (ATOM)wParam;

    wchar_t str_buf[256];
    ::GlobalGetAtomName(atom, str_buf, sizeof(str_buf));
    ::GlobalDeleteAtom(atom);

    PlayTrackFromExplorer(CString(str_buf));

    return 0L;
}

LRESULT MainFrame::OnNowPlayingUpdated(WPARAM wParam, LPARAM lParam)
{
    // if now playing is visible then update it
    if (m_PlaylistView->GetCtrl()->GetPlaylist() == Globals::Player->GetPlaylist())
    {
        UpdateFilters(UpdateActionNone, UpdateActionRequery);
        m_PlaylistView->UpdatePlaylistInfo();
    }

    return 0L;
}

void MainFrame::OnRatecurrentN1()
{
    Globals::Library->SetSongRating(Globals::Player->GetPlaying()->GetID(), -1);
    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());

    if (Globals::Player->IsPlaying())
        Globals::Player->GetPlaying()->SetRating(_T("-1"));
}

void MainFrame::OnRatecurrentUn()
{
    Globals::Library->SetSongRating(Globals::Player->GetPlaying()->GetID(), 0);
    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());

    if (Globals::Player->GetPlaying())
        Globals::Player->GetPlaying()->SetRating(_T("0"));
}

void MainFrame::OnRatecurrent1()
{
    Globals::Library->SetSongRating(Globals::Player->GetPlaying()->GetID(), 1);
    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());

    if (Globals::Player->GetPlaying())
        Globals::Player->GetPlaying()->SetRating(_T("1"));
}

void MainFrame::OnRatecurrent2()
{
    Globals::Library->SetSongRating(Globals::Player->GetPlaying()->GetID(), 2);
    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());

    if (Globals::Player->GetPlaying())
        Globals::Player->GetPlaying()->SetRating(_T("2"));
}

void MainFrame::OnRatecurrent3()
{
    Globals::Library->SetSongRating(Globals::Player->GetPlaying()->GetID(), 3);
    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());

    if (Globals::Player->GetPlaying())
        Globals::Player->GetPlaying()->SetRating(_T("3"));
}

void MainFrame::OnRatecurrent4()
{
    Globals::Library->SetSongRating(Globals::Player->GetPlaying()->GetID(), 4);
    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());

    if (Globals::Player->GetPlaying())
        Globals::Player->GetPlaying()->SetRating(_T("4"));
}

void MainFrame::OnRatecurrent5()
{
    Globals::Library->SetSongRating(Globals::Player->GetPlaying()->GetID(), 5);
    m_PlaylistView->GetCtrl()->ResyncItem(Globals::Player->GetPlaying()->GetID());

    if (Globals::Player->GetPlaying())
        Globals::Player->GetPlaying()->SetRating(_T("5"));
}

LRESULT MainFrame::OnSourcesCDAudio(WPARAM wParam, LPARAM lParam)
{
    if (!m_CDView)
    {
        m_CDView = new PlaylistViewContainer(this, NULL, NULL, NULL, NULL, NULL, false, PLAYLISTCTRL_CDRIPPER);
        m_CDView->Create(NULL, NULL, NULL, CRect(0, 0, 0, 0), this, 42452, NULL);

        PlaylistViewContainer* view = (PlaylistViewContainer*)m_CDView;
        CDPlaylistView* ctrl = (CDPlaylistView*)view->GetCtrl();
        ctrl->SetSongInfoArray(new musik::Core::SongInfoArray());
    }

    ShowSelectionBoxes(false);
    SwitchMainView((CView*)m_CDView);

    PlaylistViewContainer* view = (PlaylistViewContainer*)m_CDView;
    CDPlaylistView* ctrl = (CDPlaylistView*)view->GetCtrl();
    musik::Core::SongInfoArray* target = ctrl->m_SongInfoArray;

    if ((int)wParam != ctrl->GetDiscDrive() || target->size() != CDSystem::GetDiscTrackCount((int)wParam))
    {
        ctrl->SetDiscDrive((int)wParam);
        CDSystem::FillWithBlanks(ctrl, target, (int)wParam);
    }

    return 0L;
}

CView* MainFrame::SwitchMainView(CView* pNewView)
{
    if (pNewView == m_MainView)
        return pNewView;

    CView* oldview = m_MainView;

    // set the correct window attributes
    ::SetWindowLong(m_MainView->m_hWnd, GWL_ID, 0);
    ::SetWindowLong(pNewView->m_hWnd, GWL_ID, AFX_IDW_PANE_FIRST);

    // show new, hide old
    pNewView->ShowWindow(SW_SHOW);
    oldview->ShowWindow(SW_HIDE);

    m_MainView = pNewView;
        
    RecalcLayout();

    return m_MainView;
}

void MainFrame::HideCDView()
{
    if (m_MainView == (CView*) m_CDView)
    {
        SwitchMainView((CView*) m_PlaylistView);
    }
}

LRESULT MainFrame::OnSourcesQueryFreeDB(WPARAM wParam, LPARAM lParam)
{
    PlaylistViewContainer* pview = (PlaylistViewContainer*)m_CDView;
    CDPlaylistView* pctrl = (CDPlaylistView*)pview->GetCtrl();
    CDSystem::QueryToPlaylist(pctrl, pctrl->m_SongInfoArray, pctrl->GetDiscDrive());
    if (pctrl->m_SongInfoArray->size() == 0)
        CDSystem::FillWithBlanks(pctrl, pctrl->m_SongInfoArray, pctrl->GetDiscDrive());

    Globals::Player->SetCDInfo(*pctrl->m_SongInfoArray, pctrl->GetDiscDrive());
    return 0L;
}

void MainFrame::OnMenuExportLibrary()
{
    // create the open dialog object
    FileDialog savedlg(
        FALSE, 
        _T("db"), 
        _T("musik_u.db"),  
        OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
        _T("musikCube database files |*.db||")
   );

    if (savedlg.DoModal() == IDOK)
    {
        CString fn(Globals::Preferences->GetLibraryPath().c_str());
        fn = musikCubeApp::RelToAbs(fn);
        CopyFile(
            fn,
            savedlg.GetPathName(),
            true);
    }
}

void MainFrame::OnFileAttempttosanitisetags()
{
    musik::Core::Playlist pl;
    Globals::Library->GetAllSongs(pl);
    
    musik::Core::SongInfoArray* all = new musik::Core::SongInfoArray();

    Globals::Library->BeginTransaction();
    musik::Core::SongInfo song;
    for (size_t i = 0; i < pl.size(); i++)
    {
        Globals::Library->GetSongInfoFromID(
            pl.at(i).GetID(),
            song);
        all->push_back(song);
    }
    Globals::Library->EndTransaction();

    if (all->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            all, 
            musik::Core::MUSIK_CHGCASE_CLEAN);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete all; 
}

void MainFrame::OnPlaybackRandom()
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM)
        Globals::Player->ModifyPlaymode(NULL, musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM);
    else
        Globals::Player->ModifyPlaymode(musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM, NULL);

    Globals::Preferences->SetPlayerPlaymode(Globals::Player->GetPlaymode());
    m_NowPlayingView->GetCtrl()->UpdateCheckboxes();
}

void MainFrame::OnUpdatePlaybackRandom(CCmdUI *pCmdUI)
{
    if (Globals::Player->GetPlaymode() & musik::Core::MUSIK_PLAYER_PLAYMODE_RANDOM)
        pCmdUI->SetCheck(true);
    else
        pCmdUI->SetCheck(false);
}

LRESULT MainFrame::OnSourcesRemovableDev(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateActionReset, UpdateActionReset);
    ShowSelectionBoxes(true);
    return 0L;
}

LRESULT MainFrame::OnSourcesRefresh(WPARAM wParam, LPARAM lParam)
{
    m_SourcesView->RedrawWindow();
    return 0L;
}

LRESULT MainFrame::OnSourcesReset(WPARAM wParam, LPARAM lParam)
{
    m_SourcesView->ReloadAllItems();
    return 0L;
}

LRESULT MainFrame::OnSourcesNetRadio(WPARAM wParam, LPARAM lParam)
{
    ShowSelectionBoxes(false);

    musik::Core::String query;
    //
    query.Format(
        _T(" SELECT songid, duration, filesize ")
        _T(" FROM songs ")
        _T(" WHERE format = 'net';"),
        MUSIK_LIBRARY_FORMAT_NET);
    //
    musik::Core::Playlist* netStreams = new musik::Core::Playlist();
    Globals::MainLibrary->RawQuerySongs(query, *netStreams);
    //
    this->GivePlaylistToView(netStreams);
    this->UpdateFilters(UpdateActionNone, UpdateActionRequery);

    return 0L;
}

LRESULT MainFrame::OnSelectionBoxReset(WPARAM wParam, LPARAM lParam)
{
    UpdateFilters(UpdateActionReset, UpdateActionReset);
    return 0L;
}

void MainFrame::OnTagsRemoveextrawhitespace()
{
    musik::Core::Playlist pl;
    Globals::Library->GetAllSongs(pl);

    musik::Core::SongInfoArray* all = new musik::Core::SongInfoArray();

    Globals::Library->BeginTransaction();
    musik::Core::SongInfo song;
    for (size_t i = 0; i < pl.size(); i++)
    {
        Globals::Library->GetSongInfoFromID(
            pl.at(i).GetID(),
            song);
        all->push_back(song);
    }
    Globals::Library->EndTransaction();

    if (all->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            all, 
            musik::Core::MUSIK_CHGCASE_REMOVEWHITESPACE);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete all; 
}

void MainFrame::OnTagsAuto()
{
    musik::Core::Playlist pl;
    Globals::Library->GetAllSongs(pl);

    musik::Core::SongInfoArray* all = new musik::Core::SongInfoArray();

    Globals::Library->BeginTransaction();
    musik::Core::SongInfo song;
    for (size_t i = 0; i < pl.size(); i++)
    {
        Globals::Library->GetSongInfoFromID(
            pl.at(i).GetID(),
            song);
        all->push_back(song);
    }
    Globals::Library->EndTransaction();

    if (all->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            all, 
            musik::Core::MUSIK_CHGCASE_CAPITALIZE);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete all; 
}

void MainFrame::OnFilePluginmanager()
{
    PluginManagerDialog dlg;
    dlg.DoModal();
}

LRESULT MainFrame::OnSelectionBoxActivate(WPARAM wParam, LPARAM lParam)
{
    if (Globals::Preferences->AddEntireToNP())
    {
        // we want to send the visible playlist to the player.
        Globals::Player->SetPlaylist(m_PlaylistView->GetCtrl()->GetPlaylist());
        if (Globals::Player->GetPlaylist()->GetCount())
            Globals::Player->Play(0);
    }
    else
    {
        musik::Core::Playlist* pl = m_PlaylistView->GetCtrl()->GetPlaylist();
        int size = Globals::Player->GetPlaylist()->GetCount();
        for (size_t i = 0; i < pl->size(); i++)
            Globals::Player->GetPlaylist()->Add(pl->at(i));

        if (!Globals::Player->IsPlaying())
            Globals::Player->Play(size);
    }

    return 0L;
}

LRESULT MainFrame::OnSourcesUnsel(WPARAM wParam, LPARAM lParam)
{
    m_SourcesView->DeselectAllItems();
    return 0L;
}

LRESULT MainFrame::OnTaskbarCreated(WPARAM wParam, LPARAM lParam)
{
    if (m_TrayIconVisible)
    {
        InitTrayIcon();
        ShowTrayIcon();
    }
    return 0L;
}

LRESULT MainFrame::OnShowCube(WPARAM wParam, LPARAM lParam)
{
    if (m_IsMinimized )
    {
        if (Globals::Preferences->MinimizeToTray() || Globals::Preferences->GetCloseToSystemTray())
            RestoreFromTray();
        else
            ShowWindow(SW_RESTORE);
    }

    return 0L;
}

LRESULT MainFrame::OnHideCube(WPARAM wParam, LPARAM lParam)
{
    if (!m_IsMinimized)
    {
        if (Globals::Preferences->MinimizeToTray() || Globals::Preferences->GetCloseToSystemTray())
        {       
            SendToTray();
            ShowWindow(SW_HIDE);
        }
        else
            ShowWindow(SW_MINIMIZE);
    }

    return 0L;
}

LRESULT MainFrame::OnUpdateVolume(WPARAM wParam, LPARAM lParam)
{
    m_NowPlayingView->GetCtrl()->UpdateVolume();
    return 0L;
}

void MainFrame::OnHelpOnlinehelp()
{
    ShellExecute(
        NULL,
        _T("OPEN"),
        _T("http://musikcube.com/help"),
        NULL,
        NULL,
        SW_SHOWNORMAL
   );
}

void MainFrame::OnSourcesShowdevices()
{
    m_SourcesView->ToggleDevicesVisible();
}

void MainFrame::OnUpdateSourcesShowdevices(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)Globals::Preferences->IsDevicesVisible());
}

void MainFrame::OnSourcesShowplaylists()
{
    m_SourcesView->ToggleStdPlaylistsVisible();
}

void MainFrame::OnUpdateSourcesShowplaylists(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)Globals::Preferences->IsPlaylistsVisible());
}

void MainFrame::OnSourcesShowdynamicplaylists()
{
    m_SourcesView->ToggleDynPlaylistsVisible();
}

void MainFrame::OnUpdateSourcesShowdynamicplaylists(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)Globals::Preferences->IsDynPlaylistsVisible());
}

void MainFrame::OnSourcesShowplugins()
{
    m_SourcesView->TogglePluginsVisible();
}

void MainFrame::OnUpdateSourcesShowplugins(CCmdUI *pCmdUI)
{
    pCmdUI->SetCheck((BOOL)Globals::Preferences->IsPluginsVisible());
}

void MainFrame::OnFileThemeeditor()
{
    ThemeEditorDialog dlg;
    dlg.DoModal();
}

void MainFrame::OnViewSelectionboxes()
{
    Globals::Preferences->SetShowSelectionBoxes( ! Globals::Preferences->ShowSelectionBoxes());
    this->ShowSelectionBoxes(Globals::Preferences->ShowSelectionBoxes());
}

void MainFrame::OnUpdateViewSelectionboxes(CCmdUI *pCmdUI)
{
    PropertyTreeItem* pItem = m_SourcesView->GetFocusedItem();

    if (!pItem)
    {
        pCmdUI->SetCheck(Globals::Preferences->ShowSelectionBoxes());
        return;
    }

    switch(pItem->GetPlaylistType())
    {
    case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
    case musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY:
    case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
    case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        pCmdUI->SetCheck(Globals::Preferences->ShowSelectionBoxes());
        pCmdUI->Enable(1);
        break;

    default:
        pCmdUI->SetCheck(0);
        pCmdUI->Enable(0);
        break;
    }
}

MetadataFilterView* MainFrame::GetParentSelectionCtrl()
{
    if (this->m_AllFilterViews.size())
    {
        return this->m_AllFilterViews[0]->GetCtrl();
    }

    return NULL;
}

musik::Core::String MainFrame::GetPlaylistSelectionFilter()
{
    // Loops through all the SelectionCtrls that are currently being used as filters and
    // concatenates their criteria: (ARTIST IN(...)) AND (ALBUM IN(...)) ... etc

    musik::Core::String selectedFilter = L"";

    size_t selectionCtrlCount = this->m_ActiveFilterViews.size();
    //
    if (selectionCtrlCount)
    {
        for (size_t i = 0; i < selectionCtrlCount; i++)
        {
            selectedFilter += _T(" (");
            selectedFilter += this->m_ActiveFilterViews[i]->GetSelectedItemsQuery();
            selectedFilter += _T(")");

            if (i < (selectionCtrlCount - 1)) { selectedFilter += L" AND "; }
        }
    }
    else
    {
        return L"";
    }

    selectedFilter += L" ";

    return selectedFilter;
}

musik::Core::String MainFrame::GetPlaylistQuery()
{
    musik::Core::String query = L" SELECT songid, duration, filesize FROM songs ";

    // append selection box filter
    //
    musik::Core::String selectionFilter = this->GetPlaylistSelectionFilter();
    //
    if (selectionFilter != L"")
    {
        query += L"WHERE ";
        query += selectionFilter;
        query += L" ";
    }

    // append playlist filter, if any (i.e. Standard or Dynamic Playlist selected)
    //
    musik::Core::String playlistFilter = this->GetPlaylistInfoAsFilter();
    //
    if (playlistFilter != L"")
    {
        if (selectionFilter != L"")
        {
            query += L" AND ";
        }
        else
        {
            query += L" WHERE ";
        }

        query += playlistFilter;
    }

    query += musik::Core::Library::GetOrder(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST, true, true);

    return query;
}

musik::Core::String MainFrame::GetPlaylistInfoAsFilter()
{
    musik::Core::String filter;
    musik::Core::String playlistQuery = this->m_PlaylistInfo.GetQuery();
    //
    if (playlistQuery != L"")
    {
        // the query contained within the PlaylistInfo will be the full query,
        // so we need to project the "songid" out of it.
        filter += " songid IN( SELECT songid FROM (";
        filter += playlistQuery;
        filter += "))";
    }

    return filter;
}

musik::Core::String MainFrame::GetSelectionCtrlFilter(MetadataFilterView* selectionCtrl)
{
    // The selection CTRL needs a filterCriteria. Examples:
    //    ARTIST IN(...) AND ALBUM IN(...)

    musik::Core::String filter;

    // Get a concatenation of all selection control filters that we must listen to.
    //
    int start = this->GetSelectionCtrlOrder(selectionCtrl);
    if (start < 0) start = 0;       // not an active filter; must honor all active filters
    //
    int end = this->m_ActiveFilterViews.size();
    //
    if (end - start > 0)
    {
        MetadataFilterView* currentControl;
        int lasti = 0;
        for (int i = start; i < end - 1; i++)
        {
            currentControl = this->m_ActiveFilterViews[i];
            //
            filter += L"(";
            filter += currentControl->GetSelectedItemsQuery();
            filter += L") AND ";
            lasti = i;
        }

        currentControl = this->m_ActiveFilterViews[lasti];
            filter += L"(";
            filter += currentControl->GetSelectedItemsQuery();
            filter += L")";
    }

    // append playlist filter, if any (i.e. Standard or Dynamic Playlist selected)
    //
    musik::Core::String playlistFilter = this->GetPlaylistInfoAsFilter();
    //
    if (playlistFilter != L"")
    {
        if (filter != L"")
        {
            filter += L" AND ";
        }

        filter += playlistFilter;
    }

    return filter;
}

void MainFrame::UpdateSelectionCtrlOrder(MetadataFilterView* newSelection)
{
    int selectedOrder = this->GetSelectionCtrlOrder(newSelection);
    //
    if (selectedOrder > -1)
    {
        int invalidatedCount = this->m_ActiveFilterViews.size() - selectedOrder - 1;
        while (invalidatedCount)
        {
            this->m_ActiveFilterViews.pop_back();
            invalidatedCount--;
        }
    }
    else
    {
        this->m_ActiveFilterViews.push_back(newSelection);
    }
}

int MainFrame::GetSelectionCtrlOrder(MetadataFilterView* selectionCtrl)
{
    for (size_t i = 0; i < this->m_ActiveFilterViews.size(); i++)
    {
        if (this->m_ActiveFilterViews[i] == selectionCtrl)
        {
            return i;
        }
    }

    return -1;
}

void MainFrame::ClearSelectionCtrls()
{
    this->m_ActiveFilterViews.clear();
    for (size_t i = 0; i < this->m_AllFilterViews.size(); i++)
    {
        m_AllFilterViews[i]->GetCtrl()->Clear();
    }
}

void MainFrame::ResetPlaylistCtrl()
{
    musik::Core::Playlist* resetPlaylist = new musik::Core::Playlist();
    this->QueryPlaylist(*resetPlaylist);
    this->GivePlaylistToView(resetPlaylist);
    this->m_PlaylistView->GetCtrl()->Refresh();
}

void MainFrame::QueryPlaylistCtrl()
{
    this->ResetPlaylistCtrl();
}

void MainFrame::RequeryPlaylistCtrl()
{
    PlaylistView * PlC = this->m_PlaylistView->GetCtrl();
    musik::Core::Playlist * CurrentPl = PlC->GetPlaylist();
    this->QueryPlaylist(*CurrentPl);
    this->m_PlaylistView->GetCtrl()->Refresh();
}

void MainFrame::ClearPlaylistCtrl()
{
    musik::Core::Playlist* resetPlaylist = new musik::Core::Playlist();
    this->GivePlaylistToView(resetPlaylist);
    this->m_PlaylistView->GetCtrl()->Refresh();
}

void MainFrame::OnMenuChangeLibrary()
{
    // create the open dialog object
    FileDialog opendlg(
        TRUE, 
        _T("db"), 
        NULL, OFN_ALLOWMULTISELECT | OFN_EXPLORER, 
        _T("musikCore database files |*.db||"));

    if (opendlg.DoModal() == IDOK)
    {
        if (Globals::Player->IsPlaying())
        {
            Globals::Player->Stop();
        }

        Globals::Player->GetPlaylist()->Clear();
        KillTasks(false, true);

        musik::Core::String p = musikCubeApp::AbsToRel(opendlg.GetPathName());
        Globals::Preferences->SetLibraryPath(p);
        Globals::Library->SetFilename(opendlg.GetPathName().GetBuffer());

        this->UpdateFilters(UpdateActionReset, UpdateActionReset);
    }
}

void MainFrame::OnHelpCheckForUpdates()
{
    ShellExecute(
        NULL,
        _T("OPEN"),
        _T("http://www.musikcube.com/updatecheck?p=musikcube;v=1.0.0"),
        NULL,
        NULL,
        SW_SHOWNORMAL);
}

#ifdef _DEBUG
void MainFrame::AssertValid() const
{
    CFrameWnd::AssertValid();
}

void MainFrame::Dump(CDumpContext& dc) const
{
    CFrameWnd::Dump(dc);
}
#endif //_DEBUG

///////////////////////////////////////////////////

} } // namespace musik::Cube