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

#pragma once

///////////////////////////////////////////////////

#include "afxwin.h"
#include "musikCubeApp.h"
#include "PreferencesPropertyPage.h"
#include "FileAssociationPreferences.h"
#include <musikCore.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// Interface::Workflow
///////////////////////////////////////////////////

class InterfaceWorkflowPreferences : public PreferencesPropertyPage
{

public:
    // construct / destruct
    InterfaceWorkflowPreferences();
    virtual ~InterfaceWorkflowPreferences();

    // saving / loading
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // dialog data
    enum { IDD = IDD_PROPPAGE_INTERFACE_WORKFLOW };

    // macros
    DECLARE_DYNAMIC(InterfaceWorkflowPreferences)
    DECLARE_MESSAGE_MAP()

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX); 

    // controls
    CButton m_AddEntireToNP;
    CButton m_PromptDropOnPlaylist;
};

///////////////////////////////////////////////////
// Sound::Driver
///////////////////////////////////////////////////

class SoundDriverPreferences : public PreferencesPropertyPage
{
public:

    // construct / destruct
    SoundDriverPreferences();
    virtual ~SoundDriverPreferences();

    // dialog data
    enum { IDD = IDD_PROPPAGE_SOUND_DRIVER };

    // saving / loading
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // macros
    DECLARE_DYNAMIC(SoundDriverPreferences)
    DECLARE_MESSAGE_MAP()

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);

    // for getting sound devices
    void GetSoundDrivers(bool populate = true);
    void GetSoundPlaybackRates(bool populate = true);

    // controls
    CComboBox m_SoundDriver;
    CComboBox m_SoundPlaybackRate;
    CEdit m_SoundBufferSize;

    // arrays that hold sound driver / device info
    musik::Core::StringArray m_DriverInfo;
    musik::Core::StringArray m_PlaybackRateInfo;
};

///////////////////////////////////////////////////
// Sound::CrossfadeSettings
///////////////////////////////////////////////////

class CrossfadePreferences : public PreferencesPropertyPage
{

public:

    // construct and destruct
    CrossfadePreferences();
    virtual ~CrossfadePreferences();
    
    // from PreferencesPropertyPage
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // mfc vars and messages
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    afx_msg void OnBnClickedReset();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDelete();
    afx_msg void OnLbnSelchangePresetbox();

    CEdit m_NewSong;
    CEdit m_PauseResume;
    CEdit m_Stop;
    CEdit m_Exit;
    CListBox m_PresetBox;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_SOUND_CROSSFADER };
    void Populate(const musik::Core::CrossfadeSettings& fader);
    void RevPopulate(musik::Core::CrossfadeSettings& fader);
    int GetIndex();

    musik::Core::IntArray m_IDs;

    // macros
    DECLARE_DYNAMIC(CrossfadePreferences)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
// Interface::Transparency
///////////////////////////////////////////////////

class InterfaceTransparencyPreferences : public PreferencesPropertyPage
{

public:

    // construct and destruct
    InterfaceTransparencyPreferences();
    virtual ~InterfaceTransparencyPreferences();

    // from PreferencesPropertyPage
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // objects
    CButton m_TransEnabled;
    CButton m_TransAdaptive;
    CEdit m_TransFocused;
    CEdit m_TransUnfocused;
    CEdit m_TransFadeDuration;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    enum { IDD = IDD_PROPPAGE_INTERFACE_TRANPARENCY };

    // macros
    DECLARE_DYNAMIC(InterfaceTransparencyPreferences)
    DECLARE_MESSAGE_MAP()

};


///////////////////////////////////////////////////
// TunagePreferences
///////////////////////////////////////////////////

class TunagePreferences : public PreferencesPropertyPage
{

public:

    // construct and destruct
    TunagePreferences();
    virtual ~TunagePreferences();

    // from PreferencesPropertyPage
    virtual void CommitChanges();
    virtual void LoadPrefs();

    // objects
    CButton m_Enabled;
    CButton m_PostURLEnabled;
    CEdit m_URL;
    CButton m_WriteFileEnabled;
    CEdit m_Filename;
    CEdit m_Fileline;
    CButton m_Append;
    CButton m_RunAppEnabled;
    CEdit m_CommandLine;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    enum { IDD = IDD_PROPPAGE_TUNAGE };

    // macros
    DECLARE_DYNAMIC(TunagePreferences)
    DECLARE_MESSAGE_MAP()

};

///////////////////////////////////////////////////
// AutoCapitalizePreferences
///////////////////////////////////////////////////

class AutoCapitalizePreferences : public PreferencesPropertyPage
{
public:

    AutoCapitalizePreferences();
    virtual ~AutoCapitalizePreferences();

    virtual void LoadPrefs();

    CListBox m_ListBox;

    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDelete();

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_AUTOCAPITALIZE };

    musik::Core::StringArray m_Items;

    DECLARE_DYNCREATE(AutoCapitalizePreferences)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
// InterfaceGeneralPreferences
///////////////////////////////////////////////////

class InterfaceGeneralPreferences : public PreferencesPropertyPage
{
public:
    InterfaceGeneralPreferences();
    virtual ~InterfaceGeneralPreferences();

    // overrides
    virtual void LoadPrefs();
    virtual void CommitChanges();
    afx_msg void OnBnGlobalHotkeys();

protected:
    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_INTERFACE_GENERAL };
    
    // controls
    CButton m_EnableTrayIcon;
    CButton m_StartWithWindows;
    CButton m_AutoSynchronize;

    // startup
    void EnableStartup();
    void DisableStartup();
    bool GetStartup();

    DECLARE_DYNAMIC(InterfaceGeneralPreferences)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
// InterfaceDevicesPreferences
///////////////////////////////////////////////////

class InterfaceDevicesPreferences : public PreferencesPropertyPage
{
public:

    InterfaceDevicesPreferences();
    virtual ~InterfaceDevicesPreferences();

    // overrides
    virtual void LoadPrefs();
    virtual void CommitChanges();

    afx_msg void OnBnClickedHidedev();
    afx_msg void OnBnClickedShowdev();

    CListBox m_lstVisible;
    CListBox m_lstHidden;

protected:

    // misc
    virtual void DoDataExchange(CDataExchange* pDX);
    enum { IDD = IDD_PROPPAGE_INTERFACE_DEVICES };

    musik::Core::StringArray m_Drives;

    DECLARE_DYNAMIC(InterfaceDevicesPreferences)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
// InternetPreferences
///////////////////////////////////////////////////

class InternetPreferences : public PreferencesPropertyPage
{
public:
    InternetPreferences();
    virtual ~InternetPreferences();

    // overrides
    virtual void LoadPrefs();
    virtual void CommitChanges();

    enum { IDD = IDD_PROPPAGE_INTERNET };

protected:

    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    DECLARE_DYNAMIC(InternetPreferences)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

} } // namespace musik::Cube