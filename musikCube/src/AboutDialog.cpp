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
#include "AboutDialog.h"
#include "Version.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(AboutDialog, CDialog)
END_MESSAGE_MAP()

/*ctor*/ AboutDialog::AboutDialog() : CDialog(AboutDialog::IDD)
{
}

void AboutDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}

BOOL AboutDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    CWnd* ptrWnd = GetDlgItem(IDC_EDITBOX);

    CString wndTxt =
        
        _T("-- Project Manager, Lead Programmer --\r\n\r\n")
        _T("Casey Langen\r\n")
        _T("casey\100musikcube\056com\r\n\r\n")

        _T("-- Webmaster --\r\n\r\n")
        _T("Rajiv Makhijani\r\n")
        _T("rajiv\100musikcube\056com\r\n\r\n")

        _T("-- Documentation --\r\n\r\n")
        _T("Erik Carpenter (Urban)\r\n\r\n")

        _T("-- Installer --\r\n\r\n")
        _T("insomnia\r\n\r\n")

        _T("-- Artwork --\r\n\r\n")
        _T("derek985\r\n")
        _T("tango! icon set\r\n")
        _T("phlewp\r\n")
        _T("Mateo\r\n\r\n")

        _T("-- Contributors --\r\n\r\n")
        _T("Otto\r\n")
        _T("martind\r\n\r\n")
        _T("Yann Le Moigne\r\n")
        _T("#musik\r\n")
        _T("#coolpowers\r\n\r\n")

        _T("-- Translations --\r\n\r\n")
        _T("insomnia [German]\r\n")
        _T("Antoine Turmel [French]\r\n\r\n")

        _T("-- Quality Assurance --\r\n\r\n")
        _T("Dustin Carter\r\n")
        _T("Colby Langen\r\n")
        _T("Simon Windmill\r\n\r\n")

        _T("-- Donors --\r\n\r\n")
        _T("Sven Van Bouchout\r\n")
        _T("Dmitri Seals\r\n")
        _T("Jared Westgate\r\n")
        _T("DonTomaso\r\n\r\n")

        _T("-- Props --\r\n\r\n")
        _T("Malnilion\r\n")
        _T("DiGiTaLFX\r\n")
        _T("DocTriv\r\n")
        _T("melz\r\n")
        _T("willT\r\n")
        _T("Garmund Chan\r\n")
        _T("Jared Dickinson\r\n")
        _T("aliumalik\r\n")
        _T("Mondo\r\n")
        _T("PegaS\r\n")
        _T("Duncan Lock\r\n")
        _T("totaldarkness\r\n");

    ptrWnd->SetWindowText(wndTxt);

    CString version;

    ptrWnd = GetDlgItem(IDC_CUBEVER);
    version.Format(_T("musikCube: %s"), MUSIKCUBE_VERSION);
    ptrWnd->SetWindowText(version);

    ptrWnd = GetDlgItem(IDC_SQLITEVER);
    version.Format(_T("sqlite3: %s"), SQLITE_VERSION);
    ptrWnd->SetWindowText(version);

    ptrWnd = GetDlgItem(IDC_OPENTHREADSVER);
    version.Format(_T("OpenThreads: %s"), OPENTHREADS_VERSION);
    ptrWnd->SetWindowText(version);

    ptrWnd = GetDlgItem(IDC_BASSVER);
    version.Format(_T("BASS: %s"), BASS_VERSION);
    ptrWnd->SetWindowText(version);

    return TRUE;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube