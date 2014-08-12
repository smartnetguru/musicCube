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
#include "GlobalHotkeysDialog.h"
#include "MainFrameMessages.h"
#include "MainFrame.h"
#include "GlobalHotkeys.h"

#pragma warning(disable : 4800) // forcing bool to int

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// GlobalHotkeysDialog
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(GlobalHotkeysDialog, CDialog)

BEGIN_MESSAGE_MAP(GlobalHotkeysDialog, CDialog)
    ON_BN_CLICKED(IDC_HOTKEYS_OK_BUTTON, OnBnClickedOK)
    ON_BN_CLICKED(IDC_CANCEL_HOTKEYS_CANCEL_BUTTON, OnBnClickedCancel)
END_MESSAGE_MAP()

/*ctor*/ GlobalHotkeysDialog::GlobalHotkeysDialog(CWnd* pParent /*=NULL*/)
: CDialog(GlobalHotkeysDialog::IDD, pParent)
{
}

/*dtor*/ GlobalHotkeysDialog::~GlobalHotkeysDialog()
{
}

void GlobalHotkeysDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PLAYPAUSE_HOTKEY, m_PlayPause);
    DDX_Control(pDX, IDC_STOP_HOTKEY, m_Stop);
    DDX_Control(pDX, IDC_NEXTTRACK_HOTKEY, m_NextTrack);
    DDX_Control(pDX, IDC_PREVTRACK_HOTKEY, m_PrevTrack);
    DDX_Control(pDX, IDC_VOLUMEUP_HOTKEY, m_VolumeUp);
    DDX_Control(pDX, IDC_VOLUMEDOWN_HOTKEY, m_VolumeDown);
    DDX_Control(pDX, IDC_NEXTARTIST_HOTKEY, m_NextArtist);
    DDX_Control(pDX, IDC_PREVARTIST_HOTKEY, m_PrevArtist);
    DDX_Control(pDX, IDC_NEXTALBUM_HOTKEY, m_NextAlbum);
    DDX_Control(pDX, IDC_PREVALBUM_HOTKEY, m_PrevAlbum);
    DDX_Control(pDX, IDC_QUICKSEARCH_HOTKEY, m_QuickSearch);
    DDX_Control(pDX, IDC_ENABLEHOTKEYS_CHECKBOX, m_EnableHotkeys);
    DDX_Control(pDX, IDC_VOLUMEKEYS_CONTROL_MUSIKCUBE_VOL_CHECKBOX, m_EnableVolkeys);
}

void GlobalHotkeysDialog::SetHotkeyControl(CHotKeyCtrl& control, DWORD value)
{
    WORD virtualKey, modifiers;
    GlobalHotkeys::DeserializeHotkey(value, virtualKey, modifiers);
    control.SetHotKey(virtualKey, modifiers);
}

void GlobalHotkeysDialog::LoadHotkeys()
{
	// Fixes bug that prevents resetting a hotkey because it's still active when the dialog is showing.
	GlobalHotkeys::UnloadHotkeys();

    this->m_EnableHotkeys.SetCheck(Globals::Preferences->GetUseGlobalHotkeys());
    //
    this->SetHotkeyControl(this->m_PlayPause, Globals::Preferences->GetPlayPauseHotkey());
    this->SetHotkeyControl(this->m_Stop, Globals::Preferences->GetStopHotkey());
    this->SetHotkeyControl(this->m_NextTrack, Globals::Preferences->GetNextTrackHotkey());
    this->SetHotkeyControl(this->m_PrevTrack, Globals::Preferences->GetPrevTrackHotkey());
    this->SetHotkeyControl(this->m_VolumeUp, Globals::Preferences->GetVolumeUpHotkey());
    this->SetHotkeyControl(this->m_VolumeDown, Globals::Preferences->GetVolumeDownHotkey());
    this->SetHotkeyControl(this->m_NextArtist, Globals::Preferences->GetNextArtistHotkey());
    this->SetHotkeyControl(this->m_PrevArtist, Globals::Preferences->GetPrevArtistHotkey());
    this->SetHotkeyControl(this->m_NextAlbum, Globals::Preferences->GetNextAlbumHotkey());
    this->SetHotkeyControl(this->m_PrevAlbum, Globals::Preferences->GetPrevAlbumHotkey());
    this->SetHotkeyControl(this->m_QuickSearch, Globals::Preferences->GetQuickSearchHotkey());
    this->m_EnableVolkeys.SetCheck(Globals::Preferences->GetUseVolumeHotkeys());
}

void GlobalHotkeysDialog::SaveHotkeys()
{
    Globals::Preferences->SetUseGlobalHotkeys(this->m_EnableHotkeys.GetCheck());
    //
    Globals::Preferences->SetPlayPauseHotkey(this->m_PlayPause.GetHotKey());
    Globals::Preferences->SetStopHotkey(this->m_Stop.GetHotKey());
    Globals::Preferences->SetNextTrackHotkey(this->m_NextTrack.GetHotKey());
    Globals::Preferences->SetPrevTrackHotkey(this->m_PrevTrack.GetHotKey());
    Globals::Preferences->SetVolumeUpHotkey(this->m_VolumeUp.GetHotKey());
    Globals::Preferences->SetVolumeDownHotkey(this->m_VolumeDown.GetHotKey());
    Globals::Preferences->SetNextArtistHotkey(this->m_NextArtist.GetHotKey());
    Globals::Preferences->SetPrevArtistHotkey(this->m_PrevArtist.GetHotKey());
    Globals::Preferences->SetNextAlbumHotkey(this->m_NextAlbum.GetHotKey());
    Globals::Preferences->SetPrevAlbumHotkey(this->m_PrevAlbum.GetHotKey());
    Globals::Preferences->SetQuickSearchHotkey(this->m_QuickSearch.GetHotKey());
    Globals::Preferences->SetUseVolumeHotkeys(this->m_EnableVolkeys.GetCheck());

    if (Globals::Preferences->GetUseGlobalHotkeys())
    {
        GlobalHotkeys::ReloadHotkeys();
    }
    else
    {
        GlobalHotkeys::UnloadHotkeys();
    }
}

void GlobalHotkeysDialog::OnBnClickedOK()
{
    this->SaveHotkeys();
    this->OnOK();
}

void GlobalHotkeysDialog::OnOK()
{
    CDialog::OnOK();
}

void GlobalHotkeysDialog::OnBnClickedCancel()
{
    this->OnCancel();
}

BOOL GlobalHotkeysDialog::OnInitDialog()
{
    BOOL result = CDialog::OnInitDialog();
    this->LoadHotkeys();
    return result;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube