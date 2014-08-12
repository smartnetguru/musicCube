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
#include "PreferencesPropertyPage.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube{

///////////////////////////////////////////////////
// PreferencesPropertyPage
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(PreferencesPropertyPage, CPropertyPage)

/*ctor*/ PreferencesPropertyPage::PreferencesPropertyPage(UINT nIDTemplate)
: CPropertyPage(nIDTemplate)
{
    m_Modified = false;
}

BOOL PreferencesPropertyPage::OnInitDialog()
{
    CPropertyPage::OnInitDialog();
    LoadPrefs();
    return TRUE;
}

BOOL PreferencesPropertyPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    SetModified(TRUE);
    m_Modified = true;
    return CPropertyPage::OnCommand(wParam, lParam);
}

///////////////////////////////////////////////////
// PreferencesPropertySheet
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(PreferencesPropertySheet, CPropertySheet)

/*ctor*/ PreferencesPropertySheet::PreferencesPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
: CTreePropSheet(pszCaption, pParentWnd, iSelectPage)
{

}

BOOL PreferencesPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case IDCANCEL:

        return CTreePropSheet::OnCommand(IDCANCEL, lParam);
        break;

    case IDOK:

        CommitChanges();
        return CTreePropSheet::OnCommand(IDCANCEL, lParam);

        return true;
        break;

    case ID_APPLY_NOW:

        CommitChanges();

        return true;
        break;
    }
    
    return CTreePropSheet::OnCommand(wParam, lParam);
}

void PreferencesPropertySheet::CommitChanges()
{
    PreferencesPropertyPage* ptrPage = NULL;
    for (int i = 0; i < GetPageCount(); i++)
    {
        ptrPage = (PreferencesPropertyPage*)GetPage(i);
        if (ptrPage->IsModified())
        {
            ptrPage->CommitChanges();
            ptrPage->SetModified(FALSE);
        }
    }
}

///////////////////////////////////////////////////

} } // namespace musik::Cube