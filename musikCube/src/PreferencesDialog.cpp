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
////////////////////////////////////////////////////

#include "stdafx.h"

#include "musikCubeApp.h"
#include "PreferencesDialog.h"
#include "NameEntryDialog.h"
#include "SourcesView.h"
#include "GlobalHotkeysDialog.h"
#include "GlobalHotkeys.h"

#include <musikCore.h>
#include <atlbase.h>

///////////////////////////////////////////////////

#pragma warning (disable : 4800)    // forcing value bool 'true' to 'false'
#pragma warning(disable : 4805)

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// Interface::Workflow
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(InterfaceWorkflowPreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(InterfaceWorkflowPreferences, PreferencesPropertyPage)
END_MESSAGE_MAP()

/*ctor*/ InterfaceWorkflowPreferences::InterfaceWorkflowPreferences()
: PreferencesPropertyPage(InterfaceWorkflowPreferences::IDD)
{
}

/*dtor*/ InterfaceWorkflowPreferences::~InterfaceWorkflowPreferences()
{
}

void InterfaceWorkflowPreferences::DoDataExchange(CDataExchange* pDX)
{
    PreferencesPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_WHOLEPLAYLIST, m_AddEntireToNP);
    DDX_Control(pDX, IDC_CHECK2, m_PromptDropOnPlaylist);
}

void InterfaceWorkflowPreferences::LoadPrefs()
{
    m_PromptDropOnPlaylist.SetCheck(Globals::Preferences->GetFileDropPrompt() == -1 ? true : false);

    CButton* ptrBtn;

    // now playing behavior
    ptrBtn = (CButton*)GetDlgItem(IDC_WHOLEPLAYLIST);
    ptrBtn->SetCheck(Globals::Preferences->AddEntireToNP());
    ptrBtn = (CButton*)GetDlgItem(IDC_QUEUESONG);
    ptrBtn->SetCheck(!Globals::Preferences->AddEntireToNP());
}

void InterfaceWorkflowPreferences::CommitChanges()
{
    if (!m_PromptDropOnPlaylist.GetCheck())
    {
        if (Globals::Preferences->GetFileDropPrompt() == -1)
        {
            Globals::Preferences->SetFileDropPrompt(1);
        }
    }
    else
    {
        Globals::Preferences->SetFileDropPrompt(-1);
    }

    CButton* ptrBtn;

    // now playing
    ptrBtn = (CButton*)GetDlgItem(IDC_WHOLEPLAYLIST);
    Globals::Preferences->SetAddEntireToNP(ptrBtn->GetCheck());

    m_Modified = false;
    SetModified(FALSE);
}

///////////////////////////////////////////////////
// Sound::Driver
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(SoundDriverPreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(SoundDriverPreferences, PreferencesPropertyPage)
END_MESSAGE_MAP()

/*ctor*/ SoundDriverPreferences::SoundDriverPreferences()
: PreferencesPropertyPage(SoundDriverPreferences::IDD)
{
}

/*dtor*/ SoundDriverPreferences::~SoundDriverPreferences()
{
}

void SoundDriverPreferences::DoDataExchange(CDataExchange* pDX)
{
    DDX_Control(pDX, IDC_SOUND_DRIVER, m_SoundDriver);
    DDX_Control(pDX, IDC_SOUND_PLAYBACK_RATE, m_SoundPlaybackRate);
    DDX_Control(pDX, IDC_SNDBUFFER, m_SoundBufferSize);
}

void SoundDriverPreferences::LoadPrefs()
{
    GetSoundDrivers();
    GetSoundPlaybackRates();

    m_SoundBufferSize.SetWindowText(ItoS(Globals::Preferences->GetPlayerBuffer()).c_str());
}

void SoundDriverPreferences::CommitChanges()
{
    CString tmp;

    int orig_driver = Globals::Preferences->GetPlayerDriver();
    int orig_rate = Globals::Preferences->GetPlayerRate();

    int new_driver = m_SoundDriver.GetCurSel();

    m_SoundPlaybackRate.GetWindowText(tmp);
    int new_rate = musik::Core::StringToInt(tmp.GetBuffer());

    bool fmod_needs_restart = false;
    if (orig_driver != new_driver || orig_rate != new_rate)
        fmod_needs_restart = true;

    Globals::Preferences->SetPlayerDriver(new_driver);
    Globals::Preferences->SetPlayerRate(new_rate);

    // note: we could restart using Globals::Player but
    // to be safe we'll post the event to the main
    // frame so it can update itself.

    if (fmod_needs_restart)
    {
        int res = MessageBox(
            _T("The sound system must be stopped and restarted to apply the new changes. If you do not to restart now, the changes will not take effect until musikCube as been restarted. Restart sound system?"), 
            _T(MUSIK_VERSION_STR),
            MB_ICONINFORMATION | MB_YESNO);
        if (res == IDYES)
        {
            Globals::Player->Stop();

            while (Globals::Player->IsPlaying())
                Sleep(50);

            Globals::Player->DeinitSound();
            Globals::Player->InitSound(0, Globals::Preferences->GetPlayerDriver(), Globals::Preferences->GetPlayerRate(), musik::Core::MUSIK_PLAYER_INIT_RESTART);
        }
    }

    // sound buffer size
    m_SoundBufferSize.GetWindowText(tmp);
    musik::Core::String sbuffer = tmp;
    int size = musik::Core::StringToInt(sbuffer);
    if (size > 0)
    {
        Globals::Preferences->SetPlayerBuffer(size);
        Globals::Player->SetBufferSize(Globals::Preferences->GetPlayerBuffer());
    }
    m_Modified = false;
    SetModified(FALSE);
}

void SoundDriverPreferences::GetSoundDrivers(bool populate)
{
     Globals::Player->GetSoundDrivers(m_DriverInfo);

    if (populate)
    {
        m_SoundDriver.Clear();

        for (size_t i = 0; i < m_DriverInfo.size(); i++)
            m_SoundDriver.InsertString(i, m_DriverInfo.at(i));

        if (m_DriverInfo.size())
        {
            if (Globals::Preferences->GetPlayerDriver() < (int)m_DriverInfo.size())
                m_SoundDriver.SelectString(-1, m_DriverInfo.at(Globals::Preferences->GetPlayerDriver()));
            else
                m_SoundDriver.SelectString(-1, m_DriverInfo.at(0));
        }
    }
}

void SoundDriverPreferences::GetSoundPlaybackRates(bool populate)
{
    m_PlaybackRateInfo.clear();

    m_PlaybackRateInfo.push_back(_T("8000"));
    m_PlaybackRateInfo.push_back(_T("11025"));
    m_PlaybackRateInfo.push_back(_T("22050"));
    m_PlaybackRateInfo.push_back(_T("44100"));

    if (populate)
    {
        m_SoundPlaybackRate.Clear();

        for (size_t i = 0; i < m_PlaybackRateInfo.size(); i++)
            m_SoundPlaybackRate.InsertString(i, m_PlaybackRateInfo.at(i));

        if (m_PlaybackRateInfo.size())
        {
            CString playback_mode_str;
            playback_mode_str.Format(_T("%d"), Globals::Preferences->GetPlayerRate());

            m_SoundPlaybackRate.SelectString(-1, playback_mode_str);
        }
    }
}

///////////////////////////////////////////////////
// Sound::CrossfadeSettings
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CrossfadePreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(CrossfadePreferences, PreferencesPropertyPage)
    ON_BN_CLICKED(IDC_RESET, OnBnClickedReset)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
    ON_LBN_SELCHANGE(IDC_PRESETBOX, OnLbnSelchangePresetbox)
END_MESSAGE_MAP()

/*ctor*/ CrossfadePreferences::CrossfadePreferences()
: PreferencesPropertyPage(CrossfadePreferences::IDD)
{
}

/*dtor*/ CrossfadePreferences::~CrossfadePreferences()
{
}

void CrossfadePreferences::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_NEWSONG, m_NewSong);
    DDX_Control(pDX, IDC_PAUSERESUME, m_PauseResume);
    DDX_Control(pDX, IDC_STOP, m_Stop);
    DDX_Control(pDX, IDC_EXIT, m_Exit);
    DDX_Control(pDX, IDC_PRESETBOX, m_PresetBox);
}

void CrossfadePreferences::LoadPrefs()
{
    // intervals
    musik::Core::CrossfadeSettings fader;
    Globals::Library->GetDefaultCrossfader(fader);

    Populate(fader);

    // presets
    musik::Core::StringArray items;
    Globals::Library->GetAllCrossfaderPresets(items, m_IDs, true);

    m_PresetBox.ResetContent();
    for (size_t i = 0; i < items.size(); i++)
        m_PresetBox.AddString(items.at(i));
}

void CrossfadePreferences::Populate(const musik::Core::CrossfadeSettings& fader)
{
    m_NewSong.SetWindowText(FloatToCString(fader.m_NewSong));
    m_PauseResume.SetWindowText(FloatToCString(fader.m_PauseResume));
    m_Stop.SetWindowText(FloatToCString(fader.m_Stop));
    m_Exit.SetWindowText(FloatToCString(fader.m_Exit));
}

void CrossfadePreferences::RevPopulate(musik::Core::CrossfadeSettings& fader)
{
    CString sWnd;

    m_NewSong.GetWindowText(sWnd);
    fader.m_NewSong = musik::Core::StringToFloat(sWnd.GetBuffer());

    m_PauseResume.GetWindowText(sWnd);
    fader.m_PauseResume = musik::Core::StringToFloat(sWnd.GetBuffer());

    fader.m_Seek = 0;

    m_Stop.GetWindowText(sWnd);
    fader.m_Stop = musik::Core::StringToFloat(sWnd.GetBuffer());

    m_Exit.GetWindowText(sWnd);
    fader.m_Exit = musik::Core::StringToFloat(sWnd.GetBuffer());
}

void CrossfadePreferences::CommitChanges()
{
    musik::Core::CrossfadeSettings fader_save;
    RevPopulate(fader_save);

    Globals::Library->UpdateDefaultCrossfader(fader_save);

    if (Globals::Player->IsCrossfaderEnabled())
        Globals::Player->SetCrossfader(fader_save);
}

void CrossfadePreferences::OnBnClickedReset()
{
    Globals::Library->ResetDefaultCrossfader();
    
    musik::Core::CrossfadeSettings fader;
    Globals::Library->GetDefaultCrossfader(fader);

    Populate(fader);
}

void CrossfadePreferences::OnBnClickedAdd()
{
    musik::Core::CrossfadeSettings fader_new;
    RevPopulate(fader_new);

    CString name;
    NameEntryDialog* pDlg = new NameEntryDialog(this, &name);
    if (pDlg->DoModal() == IDOK && !name.IsEmpty())
    {
        fader_new.m_Name = name;
        int ret = Globals::Library->CreateCrossfader(fader_new);
        if (ret == musik::Core::MUSIK_LIBRARY_OK)
        {
            m_PresetBox.AddString(name);
            m_IDs.push_back(fader_new.m_ID);
    
            m_PresetBox.SetCurSel(m_PresetBox.GetCount() - 1);
        }
        else if (ret == musik::Core::MUSIK_LIBRARY_ID_EXISTS)
            MessageBox(
                _T("Sorry, but a crossfader preset with this name already exists. Please enter a unique name."), 
                _T(MUSIK_VERSION_STR), 
                MB_OK | MB_ICONWARNING);
    }
    delete pDlg;
}

void CrossfadePreferences::OnBnClickedDelete()
{
    int nSel = GetIndex();
    
    if (nSel > -1)
    {
        if (Globals::Library->DeleteCrossfader(m_IDs.at(nSel)) == musik::Core::MUSIK_LIBRARY_OK)
        {
            m_PresetBox.DeleteString(nSel);
            m_IDs.erase(m_IDs.begin() + nSel);
    
            --nSel;
            if (nSel == -1)
                nSel = 0;

            m_PresetBox.SetCurSel(nSel);
        }
    }
}

int CrossfadePreferences::GetIndex()
{
    for (size_t i = 0; i < m_IDs.size(); i++)
    {
        if (m_PresetBox.GetSel(i))
            return i;
    }

    return -1;
}

void CrossfadePreferences::OnLbnSelchangePresetbox()
{
    int nSel = GetIndex();

    if (nSel != -1)
    {
        musik::Core::CrossfadeSettings fader;
        Globals::Library->GetCrossfader(m_IDs.at(nSel), fader);

        Populate(fader);
    }
}

BOOL CrossfadePreferences::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE && GetFocus() == &m_PresetBox)
    {
        OnBnClickedDelete();
        return true;
    }

    return PreferencesPropertyPage::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////
// Interface::Transparency
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(InterfaceTransparencyPreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(InterfaceTransparencyPreferences, CPropertyPage)
END_MESSAGE_MAP()

/*ctor*/ InterfaceTransparencyPreferences::InterfaceTransparencyPreferences()
: PreferencesPropertyPage(InterfaceTransparencyPreferences::IDD)
{
}

/*dtor*/ InterfaceTransparencyPreferences::~InterfaceTransparencyPreferences()
{
}

void InterfaceTransparencyPreferences::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK1, m_TransEnabled);
    DDX_Control(pDX, IDC_EDIT1, m_TransFocused);
    DDX_Control(pDX, IDC_EDIT2, m_TransUnfocused);
}

void InterfaceTransparencyPreferences::LoadPrefs()
{
    if (Globals::Preferences->IsTransEnabled())
        m_TransEnabled.SetCheck(TRUE);

    m_TransFocused.SetWindowText(IntToCString(Globals::Preferences->GetTransFocus()));
    m_TransUnfocused.SetWindowText(IntToCString(Globals::Preferences->GetTransUnFocus()));
}

void InterfaceTransparencyPreferences::CommitChanges()
{
    // toggles
    Globals::Preferences->SetTransEnabled((bool)m_TransEnabled.GetCheck());

    CString temp;
    int focus_per, unfocus_per;

    // focused
    m_TransFocused.GetWindowText(temp);
    focus_per = musik::Core::StringToInt(temp.GetBuffer());

    if (focus_per > 255)
        focus_per = 255;
    if (focus_per < 0)
        focus_per = 0;
    Globals::Preferences->SetTransFocus(focus_per);

    // unfocused
    m_TransUnfocused.GetWindowText(temp);
    unfocus_per = musik::Core::StringToInt(temp.GetBuffer());

    if (unfocus_per > 255)
        unfocus_per = 255;
    if (unfocus_per < 0)
        unfocus_per = 0;

    Globals::Preferences->SetTransUnFocus(unfocus_per);

    if (Globals::Preferences->IsTransEnabled())
    {
        int WM_INITTRANS = RegisterWindowMessage(_T("INITTRANS"));
        AfxGetApp()->m_pMainWnd ->SendMessage(WM_INITTRANS);
    }
    else
    {
        int WM_DEINITTRANS = RegisterWindowMessage(_T("DEINITTRANS"));
        AfxGetApp()->m_pMainWnd ->SendMessage(WM_DEINITTRANS);
    }
        
    // reload
    LoadPrefs();

    SetModified(FALSE);
    m_Modified = false;
}

///////////////////////////////////////////////////
// TunagePreferences
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(TunagePreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(TunagePreferences, CPropertyPage)
END_MESSAGE_MAP()

/*ctor*/ TunagePreferences::TunagePreferences()
: PreferencesPropertyPage(TunagePreferences::IDD)
{
}

///////////////////////////////////////////////////

/*dtor*/ TunagePreferences::~TunagePreferences()
{
}

void TunagePreferences::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TUNAGEENABLED, m_Enabled);
    DDX_Control(pDX, IDC_TUNAGEURLENABLED, m_PostURLEnabled);
    DDX_Control(pDX, IDC_TUNAGEURL, m_URL);
    DDX_Control(pDX, IDC_TUNAGEFILEENABLED, m_WriteFileEnabled);
    DDX_Control(pDX, IDC_TUNAGEFILENAME, m_Filename);
    DDX_Control(pDX, IDC_TUNAGEFILELINE, m_Fileline);
    DDX_Control(pDX, IDC_TUNAGEAPPEND, m_Append);
    DDX_Control(pDX, IDC_TUNAGERUNAPPENABLED, m_RunAppEnabled);
    DDX_Control(pDX, IDC_TUNAGECOMMANDLINE, m_CommandLine);
}

void TunagePreferences::LoadPrefs()
{
    m_Enabled.SetCheck(Globals::Preferences->GetTunageEnabled());
    m_PostURLEnabled.SetCheck(Globals::Preferences->GetTunagePostURL());
    m_URL.SetWindowText(musik::Core::utf8to16(Globals::Preferences->GetTunageURL()).c_str());
    m_WriteFileEnabled.SetCheck(Globals::Preferences->GetTunageWriteFile());
    m_Filename.SetWindowText(musik::Core::utf8to16(Globals::Preferences->GetTunageFilename()).c_str());
    m_Fileline.SetWindowText(musik::Core::utf8to16(Globals::Preferences->GetTunageFileLine()).c_str());
    m_Append.SetCheck(Globals::Preferences->GetTunageAppendFile());
    m_RunAppEnabled.SetCheck(Globals::Preferences->GetTunageRunApp());
    m_CommandLine.SetWindowText(musik::Core::utf8to16(Globals::Preferences->GetTunageCmdLine()).c_str());
}

void TunagePreferences::CommitChanges()
{
    CString sWnd;

    Globals::Preferences->SetTunageEnabled(m_Enabled.GetCheck() );

    Globals::Preferences->SetTunagePostURL(m_PostURLEnabled.GetCheck() );
    m_URL.GetWindowText(sWnd);
    Globals::Preferences->SetTunageURL(sWnd.GetBuffer());

    Globals::Preferences->SetTunageWriteFile(m_WriteFileEnabled.GetCheck() );
    m_Filename.GetWindowText(sWnd);
    Globals::Preferences->SetTunageFilename(sWnd.GetBuffer());

    m_Fileline.GetWindowText(sWnd);
    Globals::Preferences->SetTunageFileLine(sWnd.GetBuffer());

    Globals::Preferences->SetTunageAppendFile(m_Append.GetCheck() );
    Globals::Preferences->SetTunageRunApp(m_RunAppEnabled.GetCheck() );

    m_CommandLine.GetWindowText(sWnd);
    Globals::Preferences->SetTunageCmdLine(sWnd.GetBuffer());
        
    // reload
    LoadPrefs();

    SetModified(FALSE);
    m_Modified = false;
}

///////////////////////////////////////////////////
// AutoCapitalizePreferences
///////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(AutoCapitalizePreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(AutoCapitalizePreferences, PreferencesPropertyPage)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
END_MESSAGE_MAP()

/*ctor*/ AutoCapitalizePreferences::AutoCapitalizePreferences()
: PreferencesPropertyPage(IDD)
{
}

/*dtor*/ AutoCapitalizePreferences::~AutoCapitalizePreferences()
{
}

void AutoCapitalizePreferences::DoDataExchange(CDataExchange* pDX)
{
    PreferencesPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ListBox);
}

void AutoCapitalizePreferences::OnBnClickedAdd()
{
    CString name;
    NameEntryDialog pDlg(this, &name);
    if (pDlg.DoModal() == IDOK && !name.IsEmpty())
    {
        int ret = Globals::Library->AddChgCase(name.GetBuffer(0));
        if (ret == musik::Core::MUSIK_LIBRARY_OK)
        {
            m_ListBox.AddString(name);
            m_Items.push_back(name.GetBuffer(0));
    
            m_ListBox.SetCurSel(m_ListBox.GetCount() - 1);
        }
        else
            MessageBox(
                _T("Sorry, but this word already exists in the library. Please enter a unique word."), 
                _T(MUSIK_VERSION_STR), 
                MB_OK | MB_ICONWARNING);
    }
}

void AutoCapitalizePreferences::OnBnClickedDelete()
{
    int nSel = -1;
    for (size_t i = 0; i < m_Items.size(); i++)
    {
        if (m_ListBox.GetSel(i))
        {    
            nSel = i;
            break;
        }
    }
    
    if (nSel > -1)
    {
        int ret = Globals::Library->RemoveChgCase(m_Items.at(nSel));
        if (ret == musik::Core::MUSIK_LIBRARY_OK)
        {
            m_ListBox.DeleteString(nSel);
            m_Items.erase(m_Items.begin() + nSel);
    
            --nSel;
            if (nSel == -1)
                nSel = 0;

            m_ListBox.SetCurSel(nSel);
        }
    }
}

void AutoCapitalizePreferences::LoadPrefs()
{
    Globals::Library->GetAllChgCase(m_Items);

    m_ListBox.ResetContent();
    for (size_t i = 0; i < m_Items.size(); i++)
        m_ListBox.AddString(m_Items.at(i));
}

///////////////////////////////////////////////////
// InterfaceGeneralPreferences
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(InterfaceGeneralPreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(InterfaceGeneralPreferences, PreferencesPropertyPage)
    ON_BN_CLICKED(IDC_GLOBALHOTKEYS_BUTTON, OnBnGlobalHotkeys)
END_MESSAGE_MAP()

/*ctor*/ InterfaceGeneralPreferences::InterfaceGeneralPreferences()
: PreferencesPropertyPage(InterfaceGeneralPreferences::IDD)
{
 
}

/*dtor*/ InterfaceGeneralPreferences::~InterfaceGeneralPreferences()
{
}

void InterfaceGeneralPreferences::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK_ENABLETRAYICON, m_EnableTrayIcon);
    DDX_Control(pDX, IDC_CHECK_STARTWITHWINDOWS, m_StartWithWindows);
    DDX_Control(pDX, IDC_CHECK4, m_AutoSynchronize);
}

void InterfaceGeneralPreferences::LoadPrefs()
{
    m_EnableTrayIcon.SetCheck(Globals::Preferences->MinimizeToTray());
    m_StartWithWindows.SetCheck(GetStartup());
    m_AutoSynchronize.SetCheck(Globals::Preferences->SynchronizeOnStartup());

    CButton* ptrBtn;

    // utc
    ptrBtn = (CButton*)GetDlgItem(IDC_USEUTC);
    ptrBtn->SetCheck(Globals::Preferences->GetUseUTC());

    // balloon tips
    ptrBtn = (CButton*)GetDlgItem(IDC_CHECK_ENABLEBALLOONTIPS);
    ptrBtn->SetCheck(Globals::Preferences->GetShowBalloonTips());

    // close to system tray
    ptrBtn = (CButton*)GetDlgItem(IDC_CLOSETOTRAY);
    ptrBtn->SetCheck(Globals::Preferences->GetCloseToSystemTray());
}

void InterfaceGeneralPreferences::CommitChanges()
{
    Globals::Preferences->SetMinimizeToTray(m_EnableTrayIcon.GetCheck());

    if (m_StartWithWindows.GetCheck())
        EnableStartup();
    else
        DisableStartup();

    CButton* ptrBtn;

    // use UTC
    ptrBtn = (CButton*)GetDlgItem(IDC_USEUTC);
    if ((bool)ptrBtn->GetCheck() != Globals::Preferences->GetUseUTC())
    {
        Globals::Library->SetUseUTC(ptrBtn->GetCheck());
        Globals::Preferences->SetUseUTC(ptrBtn->GetCheck());
    }

    // balloon tips
    ptrBtn = (CButton*)GetDlgItem(IDC_CHECK_ENABLEBALLOONTIPS);
    Globals::Preferences->SetShowBalloonTips(ptrBtn->GetCheck());

    // auto sync
    Globals::Preferences->SetSynchronizeOnStartup(m_AutoSynchronize.GetCheck());

    // close to tray
    ptrBtn = (CButton*)GetDlgItem(IDC_CLOSETOTRAY);
    Globals::Preferences->SetCloseToSystemTray(ptrBtn->GetCheck());
}

bool InterfaceGeneralPreferences::GetStartup()
{
    musikCubeApp* app = (musikCubeApp*)AfxGetApp();
    musik::Core::String userdir = app->GetUserDir();
    userdir += _T("\\Start Menu\\Programs\\Startup\\musikCube.lnk");

    return musik::Core::Filename::FileExists(userdir);
}

void InterfaceGeneralPreferences::EnableStartup()
{
    musikCubeApp* app = (musikCubeApp*)AfxGetApp();

    // where we want to store the startup shortcut
    musik::Core::String shortcut_path;
    shortcut_path = app->GetUserDir();
    shortcut_path += _T("\\Start Menu\\Programs\\Startup\\musikCube.lnk");
    
    if (musik::Core::Filename::FileExists(shortcut_path))
        return;

    // path to musik's exe
    musik::Core::String program_path;
    program_path = musikCubeApp::GetWorkingDir(true);

    // start code 
    HRESULT hRes = E_FAIL;
    DWORD dwRet = 0;
    CComPtr<IShellLink> ipShellLink;

    // buffer that receives the null-terminated string 
    // for the drive and path
    TCHAR szPath[MAX_PATH];

    // buffer that receives the address of the final 
    // file name component in the path
    LPTSTR lpszFilePart;    
    WCHAR wszTemp[MAX_PATH];
        
    // Retrieve the full path and file name of a specified file
    dwRet = GetFullPathName(program_path.c_str(), sizeof(szPath) / sizeof(TCHAR), szPath, &lpszFilePart);

    if (!dwRet)
        return;

    // Get a pointer to the IShellLink interface
    hRes = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&ipShellLink);
                             
    if (SUCCEEDED(hRes))
    {
        // Get a pointer to the IPersistFile interface
        CComQIPtr<IPersistFile> ipPersistFile(ipShellLink);

        // Set the path to the shortcut target and add the description
        hRes = ipShellLink->SetPath(szPath);
        if (FAILED(hRes))
            return;

        hRes = ipShellLink->SetArguments(_T("--autostart"));
        if (FAILED(hRes))
            return;        

        hRes = ipShellLink->SetDescription(_T("musikCube"));
        if (FAILED(hRes))
            return;

        // IPersistFile is using LPCOLESTR, so make sure 
        // that the string is Unicode
        #if !defined _UNICODE
            MultiByteToWideChar(CP_ACP, 0, shortcut_path.c_str(), -1, wszTemp, MAX_PATH);
        #else
            wcsncpy(wszTemp, shortcut_path.c_str(), MAX_PATH);
        #endif

        // Write the shortcut to disk
        hRes = ipPersistFile->Save(wszTemp, TRUE);
    }
}

void InterfaceGeneralPreferences::DisableStartup()
{
    musik::Core::String shortcut_path, program_path;
    wchar_t buffer[2000];

    // where we want to store the startup shortcut
    GetEnvironmentVariable(_T("USERPROFILE"), buffer, sizeof(buffer));
    shortcut_path = buffer;
    shortcut_path += _T("\\Start Menu\\Programs\\Startup\\musikCube.lnk");
    
    if (musik::Core::Filename::FileExists(shortcut_path))
    {
        _wremove(shortcut_path.c_str());
    }

}

void InterfaceGeneralPreferences::OnBnGlobalHotkeys()
{
    GlobalHotkeysDialog dlg;
    dlg.DoModal();
}

///////////////////////////////////////////////////
// InterfaceDevicesPreferences
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(InterfaceDevicesPreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(InterfaceDevicesPreferences, PreferencesPropertyPage)
    ON_BN_CLICKED(IDC_HIDEDEV, OnBnClickedHidedev)
    ON_BN_CLICKED(IDC_SHOWDEV, OnBnClickedShowdev)
END_MESSAGE_MAP()

/*ctor*/ InterfaceDevicesPreferences::InterfaceDevicesPreferences()
: PreferencesPropertyPage(InterfaceDevicesPreferences::IDD)
{
 
}

/*dtor*/ InterfaceDevicesPreferences::~InterfaceDevicesPreferences()
{
}

void InterfaceDevicesPreferences::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST3, m_lstVisible);
    DDX_Control(pDX, IDC_LIST1, m_lstHidden);
}

void InterfaceDevicesPreferences::LoadPrefs()
{
    SourcesView::FindAttachedDevices(m_Drives);

    musik::Core::StringArray hidden = Globals::Preferences->GetHiddenDevices();
    for (size_t i = 0; i < hidden.size(); i++)
        m_lstHidden.AddString(hidden.at(i));

    for (size_t i = 0; i < m_Drives.size(); i++)
    {
        bool found = false;
        for (size_t j = 0; j < hidden.size(); j++)
        {
            if (m_Drives.at(i) == hidden.at(j))
            {
                found = true;
                break;
            }
        }

        if (!found)
            m_lstVisible.AddString(m_Drives.at(i));
    }
}

void InterfaceDevicesPreferences::CommitChanges()
{
    musik::Core::StringArray hidden;
    for (int i = 0; i < m_lstHidden.GetCount(); i++)
    {
        CString target;
        m_lstHidden.GetText(i, target);
        hidden.push_back(musik::Core::String(target));
    }

    Globals::Preferences->SetHiddenDevices(hidden);
}

void InterfaceDevicesPreferences::OnBnClickedHidedev()
{
    for (int i = 0; i < m_lstVisible.GetCount(); i++)
    {
        if (m_lstVisible.GetSel(i))
        {
            CString target;
            m_lstVisible.GetText(i, target);
            m_lstVisible.DeleteString(i);
            m_lstHidden.AddString(target);
            return;
        }
    }
}

void InterfaceDevicesPreferences::OnBnClickedShowdev()
{
    for (int i = 0; i < m_lstHidden.GetCount(); i++)
    {
        if (m_lstHidden.GetSel(i))
        {
            CString target;
            m_lstHidden.GetText(i, target);
            m_lstHidden.DeleteString(i);
            m_lstVisible.AddString(target);
            return;
        }
    }
}

///////////////////////////////////////////////////
// InternetPreferences dialog
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(InternetPreferences, PreferencesPropertyPage)

BEGIN_MESSAGE_MAP(InternetPreferences, PreferencesPropertyPage)
END_MESSAGE_MAP()

/*ctor*/ InternetPreferences::InternetPreferences()
: PreferencesPropertyPage(InternetPreferences::IDD)
{
}

/*dtor*/ InternetPreferences::~InternetPreferences()
{
}

void InternetPreferences::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
}

void InternetPreferences::LoadPrefs()
{
    GetDlgItem(IDC_FREEDBSERVER)->SetWindowText(musik::Core::utf8to16(Globals::Preferences->GetFreeDBServer()).c_str());
}

void InternetPreferences::CommitChanges()
{
    CString str;
    GetDlgItem(IDC_FREEDBSERVER)->GetWindowText(str);
    Globals::Preferences->SetFreeDBServer(str.GetBuffer());
}

///////////////////////////////////////////////////

} } // namespace musik::Cube
