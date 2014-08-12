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
#include "MainFrame.h"
#include "AboutDialog.h"

#include <OpenThreads/Thread>
using namespace OpenThreads;

///////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

#pragma data_seg("SharedSection")
BOOL musikCubeApp::shared_IsRunning = FALSE;
HWND musikCubeApp::shared_MainHwnd = NULL;
#pragma data_seg()

#pragma comment(linker, "/SECTION:SharedSection,RWS")

///////////////////////////////////////////////////
// musikCubeApp
///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(musikCubeApp, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
END_MESSAGE_MAP()

musikCubeApp theApp;

/*ctor*/ musikCubeApp::musikCubeApp()
{
    Thread::Init();

    CString fontStr = GetWorkingDir() + _T("\\musikCube.ttf");
    if (AddFontResourceEx(fontStr, FR_PRIVATE, 0) <= 0)
    {
        MessageBox(
            NULL,
            _T("Failed to register musikCube rating font."),
            _T("musikCube"),
            MB_ICONWARNING
       );
    }
    else
    {
        ::PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
}

/*dtor*/ musikCubeApp::~musikCubeApp()
{
    DeinitCore();

    CString fontStr = GetWorkingDir() + _T("\\musikCube.ttf");
    if (RemoveFontResourceEx(fontStr, FR_PRIVATE, 0) == FALSE)
    {
        MessageBox(
            NULL,
            _T("Failed to unregister musikCube rating font."),
            _T("musikCube"),
            MB_ICONWARNING
       );
    }
    else
    {
        ::PostMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
    }
}

void musikCubeApp::ReloadLibrary()
{
    musik::Core::String path = Globals::Preferences->GetLibraryPath();
    CString realpath = RelToAbs(CString(path)); 
    Globals::MainLibrary->SetFilename(musik::Core::String(realpath));
}

void musikCubeApp::InitCore()
{
    // assure user directory exists
    CString user_dir = GetUserDir();
    user_dir += _T("\\.musikproject\\");
    musik::Core::Filename::RecurseMkDir(user_dir.GetBuffer(NULL));

    Globals::Functor = new MainFrameFunctor(GetMainWnd());

    musik::Core::String inifile = user_dir + _T("musikCube_u.ini");
    Globals::Preferences = new Preferences(inifile);

    Globals::MainLibrary = new musik::Core::Library("");

    musik::Core::String oldpath_rel = _T("~/.musikproject/musik.db");
    CString oldpath_abs = RelToAbs(CString(oldpath_rel));
    if (musik::Core::Filename::FileExists(oldpath_abs.GetBuffer()))
    {
        MessageBox(
            NULL,
            _T("It appears that you had a version of musikCube prior to 0.90 previously installed. ")
            _T("Version 0.90 and 0.91 use a radically different database format, which means your library ")
            _T("will need to be rebuilt. The old library will maintain intact in the following ")
            _T("path:\n\nc:\\Documents and Settings\\username\\.musikproject\\musik.db.old.\n\n")
            _T("This file can be loaded as an external database in all versions of musikCube ")
            _T("prior to 0.91.\n\nSorry for any inconvenience this may cause."),
            _T("musikCube"),
            MB_OK | MB_ICONINFORMATION);

        CString rnm = oldpath_abs + _T(".old");
        _wrename(oldpath_abs.GetBuffer(), rnm);
    }

    oldpath_rel = musik::Core::utf8to16(Globals::Preferences->GetLibraryPath());
    oldpath_abs = RelToAbs(CString(oldpath_rel));

    ReloadLibrary();

    Globals::Player = new musik::Core::PlayerBASS(Globals::Functor, Globals::MainLibrary);

    musik::Core::SetPlayer(Globals::Player);

    Globals::Player->SetBufferSize(Globals::Preferences->GetPlayerBuffer());
    Globals::Player->SetMaxVolume(Globals::Preferences->GetPlayerVolume());
    Globals::Player->InitSound(0, Globals::Preferences->GetPlayerDriver(), Globals::Preferences->GetPlayerRate());
    Globals::Player->SetPlaymode(Globals::Preferences->GetPlayerPlaymode());
    Globals::Player->UseGlobalEQSettings(Globals::Preferences->EqualizerUseGlobal());
    Globals::Player->start();

    // give player a crossfader, it will take
    // care of loading equalizer settings itself...
    if (Globals::Preferences->IsCrossfaderEnabled())
    {
        musik::Core::CrossfadeSettings fade;
        GetCrossfader(&fade);

        Globals::Player->EnableCrossfader();
        Globals::Player->SetCrossfader(fade);
    }

    // enable the equalizer...
    float pre =  Globals::Preferences->GetEqualizerMultiplier();
    if (Globals::Preferences->IsEqualizerEnabled())
        Globals::Player->EnableEqualizer(true, Globals::Preferences->EqualizerUseGlobal());

    // set UTC
    musik::Core::Library::SetUseUTC(Globals::Preferences->GetUseUTC());
}

void musikCubeApp::GetCrossfader(musik::Core::CrossfadeSettings* fader)
{
    if (Globals::MainLibrary->GetDefaultCrossfader(*fader) != musik::Core::MUSIK_LIBRARY_OK)
    {
        fader->Set(2.0f, 0.5f, 0.2f, 1.0f, 3.0f);
        Globals::MainLibrary->UpdateDefaultCrossfader(*fader);
    }
}

void musikCubeApp::DeinitCore()
{
    int lastplayed = 0;
    if (Globals::Player)
    {
        lastplayed = Globals::Player->GetIndex();
        if ((size_t)lastplayed < Globals::Player->GetPlaylist()->size())
            lastplayed = Globals::Player->GetPlaylist()->at(lastplayed).GetID();
        else
            lastplayed = 0;

        Globals::Player->Exit();

        // store now playing before exiting
        if (Globals::Player->GetLibrary() == Globals::MainLibrary)
            Globals::MainLibrary->PopulateTempSongTable(*Globals::Player->GetPlaylist());

        delete Globals::Player;
        Globals::Player = NULL;
    }

    if (Globals::Preferences)
    {
        Globals::Preferences->SetDlgLastPlayed(lastplayed);
        delete Globals::Preferences;
        Globals::Preferences = NULL;
    }

    if (Globals::MainLibrary)
        delete Globals::MainLibrary;

    if (Globals::Functor)
    {
        delete Globals::Functor;
        Globals::Functor = NULL;
    }
}

CString musikCubeApp::GetUserDir()
{
    CString userdir;

    wchar_t buffer[2000];
    GetEnvironmentVariable(_T("USERPROFILE"), buffer, sizeof(buffer));

    userdir = buffer;
    return userdir;
}

CString musikCubeApp::GetWorkingDir(bool include_filename)
{
    CString path;

    wchar_t buffer[2000];
    GetModuleFileName(NULL, buffer, sizeof(buffer));
    path = buffer;

    if (!include_filename)
        path = path.Left(path.ReverseFind('\\'));

    return path;
}

BOOL musikCubeApp::InitInstance()
{
    // only allow a single instance to run
    if (shared_IsRunning)
    {
        if (shared_MainHwnd)
        {
            CString sCmdLine = m_lpCmdLine;
            if (!sCmdLine.IsEmpty())
            {
                if (sCmdLine.Left(1) == "\"")
                {
                    sCmdLine.Delete(0);
                    sCmdLine.Delete(sCmdLine.GetLength() - 1);

                    ATOM atom = ::GlobalAddAtom(sCmdLine);

                    int WM_PLAYSONG = RegisterWindowMessage(_T("PLAYSONG"));
                    SendMessage(shared_MainHwnd, WM_PLAYSONG, (WPARAM)atom, (LPARAM)0);
                }
            }
        }
        return FALSE;
    }

    shared_IsRunning = TRUE;

    AfxOleInit();
    InitCommonControls();
    SetRegistryKey(_T("musikCube"));

    // see if we opened a file
    CString sCmdLine = m_lpCmdLine;
    bool autostart = false;
    bool play_song = false;
    if (!sCmdLine.IsEmpty())
    {
        if (sCmdLine.Left(1) == "\"")
        {
            sCmdLine.Delete(0);
            sCmdLine.Delete(sCmdLine.GetLength() - 1);
            play_song = true;
        }

        else if (sCmdLine.Find(_T("--autostart")) > -1)
            autostart = true;

        if (sCmdLine.Find(_T("--nographics")) > -1)
            Globals::DrawGraphics = false;
        else
            Globals::DrawGraphics = true;
    }

    // load gdi plus
    if (Globals::DrawGraphics)
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
        SourcesView::LoadBitmaps();
    }

    // such as the name of your company or organization
    // To create the main window, this code creates a new frame window
    // object and then sets it as the application's main window object
    MainFrame* pFrame = new MainFrame(autostart);
    m_pMainWnd = pFrame;

    InitCore();
    Globals::Library = Globals::MainLibrary;

    if (!pFrame)
        return FALSE;

    // create and load the frame with its resources
    pFrame->LoadFrame(IDR_MAINFRAME,
        WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
        NULL);

    if (!autostart)
    {
        pFrame->ShowWindow(SW_SHOW);
        pFrame->UpdateWindow();

        if (Globals::Preferences && Globals::Preferences->SynchronizeOnStartup())
        {
            Globals::SynchronizeDirs(Globals::MainLibrary);
        }
    }
    else
    {
        pFrame->ShowWindow(SW_MINIMIZE);
        pFrame->ShowWindow(SW_HIDE);
        pFrame->m_IsMinimized = true;
    }

    shared_MainHwnd = pFrame->GetSafeHwnd();

    if (play_song)
        Play(sCmdLine);

    // call DragAcceptFiles only if there's a suffix
    //  In an SDI app, this should occur after ProcessShellCommand
    return TRUE;
}

void musikCubeApp::Play(const CString& fn)
{
    if (m_pMainWnd)
    {
        MainFrame* pMain = (MainFrame*) m_pMainWnd;
        pMain->PlayTrackFromExplorer(fn);
    }
}

void musikCubeApp::OnAppAbout()
{
    AboutDialog aboutDlg;
    aboutDlg.DoModal();
}

CString musikCubeApp::RelToAbs(CString& path)
{
    CString abs = musikCubeApp::GetUserDir();
    CString ret(path);
    ret.Replace(_T("~"), abs);
    ret.Replace(_T("/"), _T("\\"));

    return ret;
}

CString musikCubeApp::AbsToRel(CString& path)
{
    CString rel = path;
    path.Replace(musikCubeApp::GetUserDir(), _T("~/"));  

    return path;
}

int musikCubeApp::ExitInstance()
{
    if (Globals::DrawGraphics)
        Gdiplus::GdiplusShutdown(m_gdiplusToken);

    return CWinApp::ExitInstance();
}

///////////////////////////////////////////////////
// TODO: move this somewhere not retarded
///////////////////////////////////////////////////

CString GetPlaybackStr(int type)
{
    musik::Core::SongInfo* info = Globals::Player->GetPlaying();
    bool album_empty = info->GetAlbum().IsEmpty();
    bool artist_empty = info->GetArtist().IsEmpty();
    bool title_empty = info->GetTitle().IsEmpty();
    CString set_str;

    if (type == PBSTRTYPE_NOWPLAYING_TITLE)
    {
        if (!Globals::Player->IsPlaying())
            set_str += _T("%cNot Playing");

        else if (title_empty)
            set_str += _T("%cuntitled");

        else
            set_str += _T("%b18 %a4");
    }
    else if (type == PBSTRTYPE_NOWPLAYING_ALBUM_ARTIST)
    {
        if (!Globals::Player->IsPlaying())
        {
            set_str += _T("%cNot Playing");
            goto finish;
        }

        if (!artist_empty)
            set_str += _T("%cby %b14 %a0 ");        // show artist, if exists

        if (!album_empty)
            set_str += _T("%c from the album %b14 %a1");    // show album if exists

        if (set_str.IsEmpty())
            set_str += _T("%cno song information available");
    }
    else if (type == PBSTRTYPE_TASKTRAY)
    {
        set_str += _T("[ ");
        
        if (!title_empty)
            set_str += info->GetTitle();

        if (!title_empty && !artist_empty)
            set_str += _T(" - ");

        if (!artist_empty)
            set_str += info->GetArtist();

        if (title_empty && artist_empty)
            set_str += _T("no song information available");

        set_str += _T(" ] ");
        set_str += MUSIK_VERSION_STR;
    }
    else if (type == PBSTRTYPE_BALLOONTITLE)
    {
        if (title_empty)
            set_str += _T("unknown title");
        else
            set_str +=    info->GetTitle();
    }
    else if (type == PBSTRTYPE_BALLOONARTISTALBUM)
    {
        if (!artist_empty)
        {
            set_str += _T("by \"");
            set_str += Globals::Player->GetPlaying()->GetArtist();
            set_str += _T("\"");
        }

        if (!album_empty)
        {
            if (!artist_empty)
                set_str += _T(" ");
            set_str += _T("from the album \"");
            set_str += Globals::Player->GetPlaying()->GetAlbum();
            set_str += _T("\"");
        }

        if (set_str.IsEmpty())
            set_str += _T("no song information available");
    }

finish:

    return set_str;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube