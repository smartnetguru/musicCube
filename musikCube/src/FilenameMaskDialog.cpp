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
#include "FilenameMaskDialog.h"

///////////////////////////////////////////////////

#pragma warning(disable : 4800)

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// FilenameMaskDialog
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(FilenameMaskDialog, CDialog)

BEGIN_MESSAGE_MAP(FilenameMaskDialog, CDialog)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_BN_CLICKED(IDC_TITLE, OnBnClickedTitle)
    ON_BN_CLICKED(IDC_ARTIST, OnBnClickedArtist)
    ON_BN_CLICKED(IDC_ALBUM, OnBnClickedAlbum)
    ON_BN_CLICKED(IDC_TRACK, OnBnClickedTrack)
    ON_BN_CLICKED(IDC_GENRE, OnBnClickedGenre)
    ON_BN_CLICKED(IDC_YEAR, OnBnClickedYear)
    ON_BN_CLICKED(IDC_RATING, OnBnClickedRating)
    ON_BN_CLICKED(IDC_PATH, OnBnClickedClr)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_PATH2, OnBnClickedPath2)
    ON_BN_CLICKED(IDC_PATH5, OnBnClickedPath5)
    ON_BN_CLICKED(IDC_PATH3, OnBnClickedPath3)
    ON_BN_CLICKED(IDC_PATH4, OnBnClickedPath4)
    ON_BN_CLICKED(IDC_PATH6, OnBnClickedPath6)
    ON_BN_CLICKED(IDC_PATH7, OnBnClickedPath7)
    ON_BN_CLICKED(IDC_PATH8, OnBnClickedPath8)
    ON_BN_CLICKED(IDC_PATH9, OnBnClickedPath9)
    ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
    ON_BN_CLICKED(IDC_PATH10, OnBnClickedPath10)
    ON_CBN_KILLFOCUS(IDC_MASK, OnEnKillfocusMask)
    ON_CBN_SELCHANGE(IDC_TYPE, OnCbnSelchangeType)
    ON_BN_CLICKED(IDC_APPLY, OnBnClickedApply)
    ON_CBN_EDITCHANGE(IDC_MASK, OnCbnEditchangeMask)
    ON_BN_CLICKED(IDC_BUTTON8, OnBnClickedButton8)
    ON_BN_CLICKED(IDC_BUTTON9, OnBnClickedButton9)
    ON_CBN_SELCHANGE(IDC_MASK, OnCbnSelchangeMask)
    ON_BN_CLICKED(IDC_SPCUNDR, OnBnClickedSpcundr)
END_MESSAGE_MAP()

/*ctor*/ FilenameMaskDialog::FilenameMaskDialog(CWnd* pParent, int initial_state)
: CDialog(FilenameMaskDialog::IDD, pParent)
{
    m_Parent = pParent;
    m_State = initial_state;

    m_FromFilename = Globals::Preferences->GetTagFromFilename().c_str();
    m_ToFilename = Globals::Preferences->GetTagToFilename().c_str();
    m_SpcToUndr = Globals::Preferences->GetTagSpacesToUndr();
    m_UndrToSpc = Globals::Preferences->GetTagUndrToSpaces();

    m_CurrMask = &m_ToFilename;
}

/*dtor*/ FilenameMaskDialog::~FilenameMaskDialog()
{
}

void FilenameMaskDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TYPE, m_Type);
}

void FilenameMaskDialog::OnBnClickedOk()
{
    OnBnClickedApply();
    CDialog::OnOK();
}

void FilenameMaskDialog::OnBnClickedCancel()
{
    OnClose();
}

void FilenameMaskDialog::OnBnClickedTitle()
{
    AppendMask(_T("$TITLE"));
}

void FilenameMaskDialog::AppendMask(CString str)
{
    *m_CurrMask += str;
    GetDlgItem(IDC_MASK)->SetWindowText(*m_CurrMask);
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void FilenameMaskDialog::OnBnClickedArtist()
{
    AppendMask(_T("$ARTIST"));
}

void FilenameMaskDialog::OnBnClickedAlbum()
{
    AppendMask(_T("$ALBUM"));
}

void FilenameMaskDialog::OnBnClickedTrack()
{
    AppendMask(_T("$TRACK"));
}

void FilenameMaskDialog::OnBnClickedGenre()
{
    AppendMask(_T("$GENRE"));
}

void FilenameMaskDialog::OnBnClickedYear()
{
    AppendMask(_T("$YEAR"));
}

void FilenameMaskDialog::OnBnClickedRating()
{
    AppendMask(_T("$RATING"));
}

void FilenameMaskDialog::OnBnClickedClr()
{
    *m_CurrMask = _T("");
    GetDlgItem(IDC_MASK)->SetWindowText(*m_CurrMask);    
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

BOOL FilenameMaskDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_Type.AddString(_T("Tag to Filename"));
    m_Type.AddString(_T("Tag from Filename"));

    if (m_State == MUSIK_MASK_FROMFILENAME)
    {
        m_Type.SelectString(-1, _T("Tag f"));

        m_CurrMask = &m_FromFilename;
        *m_CurrMask = Globals::Preferences->GetTagFromFilename().c_str();

        CButton* ptrChk = (CButton*)GetDlgItem(IDC_SPCUNDR);
        ptrChk->SetCheck(m_UndrToSpc);
        ptrChk->SetWindowText(_T("underscores to spaces"));
    }
    else
    {
        m_Type.SelectString(-1, _T("Tag t"));

        m_CurrMask = &m_ToFilename;
        *m_CurrMask = Globals::Preferences->GetTagToFilename().c_str();

        CButton* ptrChk = (CButton*)GetDlgItem(IDC_SPCUNDR);
        ptrChk->SetCheck(m_SpcToUndr);
        ptrChk->SetWindowText(_T("spaces to underscores"));
    }

    PopulateMasks();
        
    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
    GetDlgItem(IDC_MASK)->SetWindowText(*m_CurrMask);

    return TRUE;
}

void FilenameMaskDialog::OnEnKillfocusMask()
{
    GetDlgItem(IDC_MASK)->GetWindowText(*m_CurrMask);
}

BOOL FilenameMaskDialog::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            OnClose();
            return true;
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void FilenameMaskDialog::OnClose()
{
    CDialog::OnCancel();
}

void FilenameMaskDialog::OnBnClickedPath2()
{
    AppendMask(_T("-"));
}

void FilenameMaskDialog::OnBnClickedPath5()
{
    AppendMask(_T("_"));
}

void FilenameMaskDialog::OnBnClickedPath3()
{
    AppendMask(_T("_-_"));
}

void FilenameMaskDialog::OnBnClickedPath4()
{
    AppendMask(_T("-_-"));
}

void FilenameMaskDialog::OnBnClickedPath6()
{
    AppendMask(_T("("));
}

void FilenameMaskDialog::OnBnClickedPath7()
{
    AppendMask(_T(")"));
}

void FilenameMaskDialog::OnBnClickedPath8()
{
    AppendMask(_T("["));
}

void FilenameMaskDialog::OnBnClickedPath9()
{
    AppendMask(_T("]"));
}

void FilenameMaskDialog::OnBnClickedButton2()
{
    AppendMask(_T(" "));
}

void FilenameMaskDialog::OnBnClickedPath10()
{
    AppendMask(_T("\\"));
}

void FilenameMaskDialog::OnCbnSelchangeType()
{
    if (m_State == MUSIK_MASK_FROMFILENAME)
    {
        m_CurrMask = &m_ToFilename;

        GetDlgItem(IDC_MASK)->SetWindowText(*m_CurrMask);    
        m_State = MUSIK_MASK_TOFILENAME;

        CButton* ptrChk = (CButton*)GetDlgItem(IDC_SPCUNDR);
        ptrChk->SetCheck(m_SpcToUndr);
        ptrChk->SetWindowText(_T("spaces to underscores"));
    }
    else
    {
        m_CurrMask = &m_FromFilename;

        GetDlgItem(IDC_MASK)->SetWindowText(*m_CurrMask);
        m_State = MUSIK_MASK_FROMFILENAME;

        CButton* ptrChk = (CButton*)GetDlgItem(IDC_SPCUNDR);
        m_SpcToUndr = ptrChk->GetCheck();
        ptrChk->SetCheck(m_UndrToSpc);
        ptrChk->SetWindowText(_T("underscores to spaces"));
    }
}

void FilenameMaskDialog::OnBnClickedApply()
{
    CString str;
    m_Type.GetWindowText(str);

    Globals::Preferences->SetTagFromFilename(m_FromFilename.GetBuffer(NULL));
    Globals::Preferences->SetTagToFilename(m_ToFilename.GetBuffer(NULL));
    Globals::Preferences->SetTagSpacesToUndr(m_SpcToUndr);
    Globals::Preferences->SetTagUndrToSpaces(m_UndrToSpc);

    GetDlgItem(IDC_APPLY)->EnableWindow(FALSE);
}

void FilenameMaskDialog::OnCbnEditchangeMask()
{
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void FilenameMaskDialog::OnBnClickedButton8()
{
    if (!m_CurrMask->IsEmpty())
    {
        Globals::Library->AddMask(m_CurrMask->GetBuffer(NULL));
        m_Masks.push_back(m_CurrMask->GetBuffer(NULL));

        PopulateMasks();
    }
}

void FilenameMaskDialog::OnBnClickedButton9()
{
    if (!m_CurrMask->IsEmpty())
    {
        Globals::Library->RemoveMask(m_CurrMask->GetBuffer(NULL));

        for (size_t i = 0; i < m_Masks.size(); i++)
        {
            if (m_Masks.at(i) == m_CurrMask->GetBuffer(NULL))
                m_Masks.erase(m_Masks.begin() + i);
        }

        *m_CurrMask = _T("");
        GetDlgItem(IDC_MASK)->SetWindowText(*m_CurrMask);

        PopulateMasks();
    }
}

void FilenameMaskDialog::PopulateMasks()
{
    Globals::Library->GetAllMasks(m_Masks);
    CComboBox* masks = (CComboBox*)GetDlgItem(IDC_MASK);

    CString str;
    masks->GetWindowText(str);

    masks->ResetContent();
    for (size_t i = 0; i < m_Masks.size(); i++)
        masks->AddString(m_Masks.at(i).c_str());

    if (!str.IsEmpty())
        masks->SetWindowText(str);
}

void FilenameMaskDialog::OnCbnSelchangeMask()
{
    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

void FilenameMaskDialog::OnBnClickedSpcundr()
{
    CButton* ptrChk = (CButton*)GetDlgItem(IDC_SPCUNDR);
    if ( m_State == MUSIK_MASK_FROMFILENAME )
        m_UndrToSpc = ptrChk->GetCheck();
    else
        m_SpcToUndr = ptrChk->GetCheck();

    GetDlgItem(IDC_APPLY)->EnableWindow(TRUE);
}

///////////////////////////////////////////////////

} } // namespace musik::Cube