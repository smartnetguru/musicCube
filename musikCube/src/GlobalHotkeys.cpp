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

#include "GlobalHotkeys.h"
#include "musikCubeApp.h"
#include "MainFrameMessages.h"
#include "MainFrame.h"

#include <musikCore.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

void GlobalHotkeys::SetHotkey(HWND hwnd, enum Hotkey hotkey, DWORD serializedHotkey)
{
    if (serializedHotkey != 0)
    {
        WORD virtualKey, modifiers;
        GlobalHotkeys::DeserializeHotkey(serializedHotkey, virtualKey, modifiers);

        WORD realModifiers = 0;
        if (modifiers & HOTKEYF_ALT) realModifiers |= MOD_ALT;
        if (modifiers & HOTKEYF_CONTROL) realModifiers |= MOD_CONTROL;
        if (modifiers & HOTKEYF_SHIFT) realModifiers |= MOD_SHIFT;
        if (modifiers & HOTKEYF_EXT) realModifiers |= MOD_WIN;

        ::RegisterHotKey(hwnd, (int) hotkey, realModifiers, virtualKey);
    }
}

void GlobalHotkeys::ReloadHotkeys()
{
    MainFrame* musikFrame = (MainFrame*) AfxGetApp()->GetMainWnd();
    HWND musikHWND = musikFrame->GetSafeHwnd();

    GlobalHotkeys::UnloadHotkeys();

    GlobalHotkeys::SetHotkey(musikHWND, HotkeyPlayPauseResume, Globals::Preferences->GetPlayPauseHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyStop, Globals::Preferences->GetStopHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyNextTrack, Globals::Preferences->GetNextTrackHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyPrevTrack, Globals::Preferences->GetPrevTrackHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyVolumeUp, Globals::Preferences->GetVolumeUpHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyVolumeDown, Globals::Preferences->GetVolumeDownHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyNextArtist, Globals::Preferences->GetNextArtistHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyPrevArtist, Globals::Preferences->GetPrevArtistHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyNextAlbum, Globals::Preferences->GetNextAlbumHotkey());
    GlobalHotkeys::SetHotkey(musikHWND, HotkeyPrevAlbum, Globals::Preferences->GetPrevAlbumHotkey());
	GlobalHotkeys::SetHotkey(musikHWND, HotkeyQuickSearch, Globals::Preferences->GetQuickSearchHotkey());
	//
    ::RegisterHotKey(musikHWND, HotkeyMediaKeyPlayPauseResume, 0,VK_MEDIA_PLAY_PAUSE);
    ::RegisterHotKey(musikHWND, HotkeyMediaKeyStop, 0, VK_MEDIA_STOP);
    ::RegisterHotKey(musikHWND, HotkeyMediaKeyNextTrack, 0, VK_MEDIA_NEXT_TRACK);
    ::RegisterHotKey(musikHWND, HotkeyMediaKeyPrevTrack, 0, VK_MEDIA_PREV_TRACK);
    ::RegisterHotKey(musikHWND, HotkeyMediaKeyVolumeUp, 0, VK_VOLUME_UP);
    ::RegisterHotKey(musikHWND, HotkeyMediaKeyVolumeDown, 0, VK_VOLUME_DOWN);
}

void GlobalHotkeys::UnloadHotkeys()
{
    MainFrame* musikFrame = (MainFrame*) AfxGetApp()->GetMainWnd();
    HWND musikHWND = musikFrame->GetSafeHwnd();

    ::UnregisterHotKey(musikHWND, HotkeyPlayPauseResume);
    ::UnregisterHotKey(musikHWND, HotkeyStop);
    ::UnregisterHotKey(musikHWND, HotkeyNextTrack);
    ::UnregisterHotKey(musikHWND, HotkeyPrevTrack);
    ::UnregisterHotKey(musikHWND, HotkeyVolumeUp);
    ::UnregisterHotKey(musikHWND, HotkeyVolumeDown);
    ::UnregisterHotKey(musikHWND, HotkeyNextArtist);
    ::UnregisterHotKey(musikHWND, HotkeyPrevArtist);
    ::UnregisterHotKey(musikHWND, HotkeyNextAlbum);
    ::UnregisterHotKey(musikHWND, HotkeyPrevAlbum);
    //
    ::UnregisterHotKey(musikHWND, HotkeyMediaKeyPlayPauseResume);
    ::UnregisterHotKey(musikHWND, HotkeyMediaKeyStop);
    ::UnregisterHotKey(musikHWND, HotkeyMediaKeyNextTrack);
    ::UnregisterHotKey(musikHWND, HotkeyMediaKeyPrevTrack);
    ::UnregisterHotKey(musikHWND, HotkeyMediaKeyVolumeUp);
    ::UnregisterHotKey(musikHWND, HotkeyMediaKeyVolumeDown);
}

bool GlobalHotkeys::ProcessHotkey(enum GlobalHotkeys::Hotkey hotkey, std::wstring& statusString)
{
    MainFrame* musikFrame = (MainFrame*) AfxGetApp()->GetMainWnd();
    HWND musikHWND = musikFrame->GetSafeHwnd();

    if (!Globals::Preferences->GetUseVolumeHotkeys())
    {
        ::UnregisterHotKey(musikHWND, HotkeyMediaKeyVolumeUp);
        ::UnregisterHotKey(musikHWND, HotkeyMediaKeyVolumeDown);
    }
    switch(hotkey)
    {
    ////////////////////////
    // play, pause, resume
    ////////////////////////
    case HotkeyPlayPauseResume:
    case HotkeyMediaKeyPlayPauseResume:
        {
            if (Globals::Player->IsPaused())
            {
                Globals::Player->Resume();
                statusString = L"Resume Playback";
            }
            else if (Globals::Player->IsPlaying())
            {
                statusString = L"Pause Playback";
                Globals::Player->Pause();
            }
            else
            {
                statusString = L"Start Playback";
                ::PostMessage(musikHWND, WM_PLAYER_PLAYSEL, 0, 0);
            }
        }
        return true;

    ////////////////////////
    // stop
    ////////////////////////
    case HotkeyStop:
    case HotkeyMediaKeyStop:
        {
            statusString = L"Stop Playback";
            Globals::Player->Stop();
        }
        break;

    ////////////////////////
    // next track
    ////////////////////////
    case HotkeyNextTrack:
    case HotkeyMediaKeyNextTrack:
        if (Globals::Player->IsPlaying())
        {
            statusString = L"Next Song";
            Globals::Player->Next();
        }
        return true;

    ////////////////////////
    // prev track
    ////////////////////////
    case HotkeyPrevTrack:
    case HotkeyMediaKeyPrevTrack:
        if (Globals::Player->IsPlaying())
        {
            statusString = L"Prev Song";
            Globals::Player->Prev();
        }
        return true;

    ////////////////////////
    // volume up
    ////////////////////////
    case HotkeyVolumeUp:
    case HotkeyMediaKeyVolumeUp:
        {
            int volume = Globals::Player->GetMaxVolume() + 12;
            if (volume > 255)
            {
                volume = 255;
            }
            Globals::Player->SetMaxVolume(volume);
            musikFrame->m_NowPlayingView->GetCtrl()->UpdateVolume();     // hack
                
            int vol = 100 * Globals::Player->GetMaxVolume() / 255;
            musik::Core::String info;
            info.Format(_T("Volume: %d%%"), vol);
            //
            statusString = info;
        }
        return true;


    ////////////////////////
    // volume down
    ////////////////////////
    case HotkeyVolumeDown:
    case HotkeyMediaKeyVolumeDown:
        {
            int volume = Globals::Player->GetMaxVolume() - 12;
            if (volume < 0)
            {
                volume = 0;
            }
            Globals::Player->SetMaxVolume(volume);
            musikFrame->m_NowPlayingView->GetCtrl()->UpdateVolume();     // hack
                    int vol = 100 * Globals::Player->GetMaxVolume() / 255;
            musik::Core::String info;
            info.Format(L"Volume: %d%%", vol);
            //
            statusString = info;
        }
        return true;


    ////////////////////////
    // previous artist, next artist, prev artist album, next artist album
    ////////////////////////
    case HotkeyPrevArtist:
    case HotkeyNextArtist:
    case HotkeyPrevAlbum:
    case HotkeyNextAlbum:
        {
            musik::Core::String currArtist = Globals::Player->GetPlaying()->GetArtist();
            musik::Core::String currAlbum = Globals::Player->GetPlaying()->GetAlbum(); 
            musik::Core::Playlist* nowPlaying = Globals::Player->GetPlaylist();

            switch(hotkey)
            {
            case HotkeyPrevArtist:
                Globals::Library->GetPrevArtist(currArtist, *nowPlaying);
                statusString = L"Previous Artist";
                break;
            case HotkeyNextArtist:
                Globals::Library->GetNextArtist(currArtist, *nowPlaying);
                statusString = L"Next Artist";
                break;
            case HotkeyPrevAlbum:
                Globals::Library->GetPrevAlbum(currArtist, currAlbum, *nowPlaying);
                statusString = L"Previous Album by Artist";
                break;
            case HotkeyNextAlbum:
                Globals::Library->GetNextAlbum(currArtist, currAlbum, *nowPlaying);
                statusString = L"Next Album by Artist";
                break;
            }
            
            if (musikFrame->m_PlaylistView->GetCtrl()->GetPlaylist() == nowPlaying)
            {
                musikFrame->UpdateFilters(MainFrame::UpdateActionNone, MainFrame::UpdateActionRequery);   // hack
            }

            if (nowPlaying->size())
            {
                Globals::Player->Play(0);
            }
        }
        return true;

    case HotkeyQuickSearch:
        //CmusikCubeFrame* musikFrame = (CmusikCubeFrame*) AfxGetApp()->GetMainWnd();
        if (musikFrame->m_IsMinimized)
            musikFrame->RestoreFromTray();
        else
            ::SetForegroundWindow(musikFrame->GetSafeHwnd());
        musikFrame->m_SourcesView->FocusQuickSearch();
        musikFrame->m_SourcesView->EnableQuickSearch();
        return true;
	}

    return false;
}

void GlobalHotkeys::DeserializeHotkey(DWORD hotkey, WORD& virtualKey, WORD& modifiers)
{
    short hotkeyShort = (short) hotkey;
    virtualKey = (WORD) (hotkeyShort & 0x00ff);
    modifiers = (WORD) ((hotkeyShort >> 8) & 0x0ff);
}

///////////////////////////////////////////////////

} } // namespace musik::Cube