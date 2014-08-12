///////////////////////////////////////////////////
//
// License Agreement:
//
// The following are Copyright � 2002-2006, Casey Langen
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
#include "FileDropDialog.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// FileDropDialog
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(FileDropDialog, CDialog)

BEGIN_MESSAGE_MAP(FileDropDialog, CDialog)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_BN_CLICKED(IDC_RADIO_ADD_PLAYLIST, OnBnClickedRadioAddPlaylist)
    ON_BN_CLICKED(IDC_RADIO_ADD_NOW_PLAYING, OnBnClickedRadioAddNowPlaying)
    ON_BN_CLICKED(IDC_RADIO_ADD_LIBRARY, OnBnClickedRadioAddLibrary)
    ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/*ctor*/ FileDropDialog::FileDropDialog(CWnd* pParent)
: CDialog(IDD_FILEDROP, pParent)
{
    m_Ret = MUSIK_FILEDROP_ADDNOWPLAYING;
    m_FirstRun = true;
}

/*dtor*/ FileDropDialog::~FileDropDialog()
{
}

void FileDropDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_REMEMBER, m_Remember);
}

INT_PTR FileDropDialog::DoModal()
{
    CDialog::DoModal();

    return m_Ret;
}

void FileDropDialog::OnBnClickedCancel()
{
    m_Ret = MUSIK_FILEDROP_CANCEL;
    OnCancel();
}

void FileDropDialog::OnBnClickedRadioAddPlaylist()
{
    m_Ret = MUSIK_FILEDROP_ADDPLAYLIST;
}

void FileDropDialog::OnBnClickedRadioAddNowPlaying()
{
    m_Ret = MUSIK_FILEDROP_ADDNOWPLAYING;
}

void FileDropDialog::OnBnClickedRadioAddLibrary()
{
    m_Ret = MUSIK_FILEDROP_ADDLIBRARY;
}

void FileDropDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialog::OnShowWindow(bShow, nStatus);

    if (m_FirstRun)
    {
        CButton* pBtn = (CButton*)GetDlgItem(IDC_RADIO_ADD_LIBRARY);
        pBtn->SetCheck(true);
        m_FirstRun = false;

        ::SetWindowPos(GetSafeHwnd(), wndTopMost, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    }

    this->BringWindowToTop();
}

void FileDropDialog::OnBnClickedOk()
{
    if (m_Remember.GetCheck() && Globals::Preferences)
        Globals::Preferences->SetFileDropPrompt(m_Ret);

    OnOK();
}

///////////////////////////////////////////////////

} } // namespace musik::Cube