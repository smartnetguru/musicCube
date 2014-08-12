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
#include "EqualizerPresetsDialog.h"
#include "EqualizerView.h"
#include "NameEntryDialog.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// EqualizerPresetsDialog
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(EqualizerPresetsDialog, CDialog)

BEGIN_MESSAGE_MAP(EqualizerPresetsDialog, CDialog)
    ON_BN_CLICKED(IDC_CLOSE_DLG, OnBnClickedCloseDlg)
    ON_BN_CLICKED(IDC_ADD, OnBnClickedAdd)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_RENAME_SEL, OnBnClickedRenameSel)
    ON_LBN_SELCHANGE(IDC_PRESET_BOX, OnLbnSelchangePresetBox)
    ON_BN_CLICKED(IDC_DELETE_SEL, OnBnClickedDeleteSel)
END_MESSAGE_MAP()

/*ctor*/ EqualizerPresetsDialog::EqualizerPresetsDialog(CWnd* pParent)
: CDialog(EqualizerPresetsDialog::IDD, pParent)
{
    m_Parent = pParent;
}

/*dtor*/EqualizerPresetsDialog::~EqualizerPresetsDialog()
{
}

void EqualizerPresetsDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PRESET_BOX, m_PresetBox);
}

void EqualizerPresetsDialog::OnBnClickedCloseDlg()
{
    OnClose();
}

BOOL EqualizerPresetsDialog::OnInitDialog()
{
    CDialog::OnInitDialog();
    ReloadEqualizers();
    return TRUE;
}

void EqualizerPresetsDialog::ReloadEqualizers()
{
    musik::Core::StringArray items;
    Globals::Library->GetAllEqualizerPresets(items, m_IDs, true);

    m_PresetBox.ResetContent();
    for (size_t i = 0; i < items.size(); i++)
        m_PresetBox.AddString(items.at(i));
}

int EqualizerPresetsDialog::GetIndex()
{
    for (size_t i = 0; i < m_IDs.size(); i++)
    {
        if (m_PresetBox.GetSel(i))
            return i;
    }

    return -1;
}

void EqualizerPresetsDialog::OnBnClickedAdd()
{    
    musik::Core::EqualizerSettings settings;
    EqualizerViewContainer* pBar = (EqualizerViewContainer*)m_Parent;
    pBar->GetCtrl()->BandsToEQSettings(&settings);

    CString name;
    NameEntryDialog* pDlg = new NameEntryDialog(this, &name);
    if (pDlg->DoModal() == IDOK && !name.IsEmpty())
    {
        settings.m_Name = name;
        int ret = Globals::Library->CreateEqualizer(settings, true);
        if (ret == 0)
        {
            ReloadEqualizers();
        }
        else if (ret == musik::Core::MUSIK_LIBRARY_ID_EXISTS)
            MessageBox(
                _T("Sorry, but an equalizer preset with this name already exists. Please enter a unique name."), 
                _T(MUSIK_VERSION_STR), 
                MB_OK | MB_ICONWARNING);
    }
    delete pDlg;
}

void EqualizerPresetsDialog::OnClose()
{
    int WM_CLOSEEQUALIZERPRESETS = RegisterWindowMessage(_T("CLOSEEQUALIZERPRESETS"));
    m_Parent->PostMessage(WM_CLOSEEQUALIZERPRESETS);
}

BOOL EqualizerPresetsDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        OnClose();
        return true;
    }
    else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE && GetFocus() == &m_PresetBox)
    {
        OnBnClickedDeleteSel();
        return true;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void EqualizerPresetsDialog::OnBnClickedRenameSel()
{
    int nSel = GetIndex();

    if (nSel > -1)
    {
        CString rename;
        NameEntryDialog* pDlg = new NameEntryDialog(this, &rename);
        if (pDlg->DoModal() == IDOK && !rename.IsEmpty())
        {
            musik::Core::EqualizerSettings settings;
            EqualizerViewContainer* pBar = (EqualizerViewContainer*)m_Parent;
            pBar->GetCtrl()->BandsToEQSettings(&settings);

            settings.m_ID = m_IDs.at(nSel);
            settings.m_Name = rename;

            if (Globals::Library->UpdateEqualizer(m_IDs.at(nSel), settings) == 0)
            {
                ReloadEqualizers();
            }
        }
    }
}

void EqualizerPresetsDialog::OnLbnSelchangePresetBox()
{
    int nSel = GetIndex();

    if (nSel != -1)
    {
        musik::Core::EqualizerSettings settings;
        Globals::Library->GetEqualizer(m_IDs.at(nSel), settings);

        EqualizerViewContainer* pBar = (EqualizerViewContainer*)m_Parent;
        pBar->GetCtrl()->SetBandsFrom(settings);
        pBar->GetCtrl()->OnBandChange(NULL, NULL);
    }
}

void EqualizerPresetsDialog::GetActiveEqualizer(musik::Core::EqualizerSettings* settings)
{
    EqualizerViewContainer* pBar = (EqualizerViewContainer*)m_Parent;
    pBar->GetCtrl()->BandsToEQSettings(settings);
}

void EqualizerPresetsDialog::OnBnClickedDeleteSel()
{
    int nSel = GetIndex();

    if (nSel > -1)
    {
        if (Globals::Library->DeleteEqualizer(m_IDs.at(nSel)) == musik::Core::MUSIK_LIBRARY_OK)
        {
            ReloadEqualizers();
        }
    }
}

///////////////////////////////////////////////////

} } // namespace musik::Cube