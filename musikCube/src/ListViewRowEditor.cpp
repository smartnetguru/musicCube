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
#include "ListViewRowEditor.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

int WM_MUSIKEDITCOMMIT = RegisterWindowMessage(_T("MUSIKEDITCOMMIT"));
int WM_MUSIKEDITCANCEL = RegisterWindowMessage(_T("MUSIKEDITCANCEL"));
int WM_MUSIKEDITCHANGE = RegisterWindowMessage(_T("MUSIKEDITCHANGE"));

IMPLEMENT_DYNAMIC(ListViewRowEditor, CEdit)

BEGIN_MESSAGE_MAP(ListViewRowEditor, CEdit)
    ON_CONTROL_REFLECT(EN_KILLFOCUS, OnEnKillfocus)
    ON_WM_SHOWWINDOW()
    ON_CONTROL_REFLECT(EN_CHANGE, OnEnChange)
    ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/*ctor*/ ListViewRowEditor::ListViewRowEditor()
{
    this->m_IsVisible = false;
    this->m_Args = NULL;
    this->m_MessageTarget = NULL;
    m_brBg.CreateSolidBrush(Globals::Preferences->GetListCtrlColor());
}

/*dtor*/ ListViewRowEditor::~ListViewRowEditor()
{
}

void ListViewRowEditor::Commit()
{
    GetWindowText(m_Str);
    m_Commited = true;
    m_IsVisible = false;

    if (GetMessageTarget() && !m_MessageSent)
    {
        m_MessageSent = true;
        GetMessageTarget()->SendMessage(WM_MUSIKEDITCOMMIT, (WPARAM)this, (LPARAM)m_Args);
    }
}

void ListViewRowEditor::Cancel()
{
    GetWindowText(m_Str);
    ShowWindow(SW_HIDE);
    m_IsVisible = false;
    m_Commited = false;

    if (GetMessageTarget() && !m_MessageSent)
    {
        m_MessageSent = true;
        GetMessageTarget()->SendMessage(WM_MUSIKEDITCANCEL, (WPARAM)this, (LPARAM)m_Args);
    }
}

void ListViewRowEditor::OnEnKillfocus()
{
    GetWindowText(m_Str);
    ShowWindow(SW_HIDE);
    m_IsVisible = false;
    m_Commited = false;

    if (GetMessageTarget() && !m_MessageSent)
    {
        m_MessageSent = true;
        GetMessageTarget()->SendMessage(WM_MUSIKEDITCANCEL, (WPARAM)this, (LPARAM)m_Args);
    }
}

BOOL ListViewRowEditor::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        Commit();
        return TRUE;
    }

    else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
    {
        Cancel();
        return TRUE;
    }

    return CEdit::PreTranslateMessage(pMsg);
}

void ListViewRowEditor::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CEdit::OnShowWindow(bShow, nStatus);

    if (bShow)
    {
        m_Commited = false;
        m_MessageSent = false;

        SetWindowText(m_Str);
        SetSel(0, m_Str.GetLength());
        m_IsVisible = true;
    }
}

void ListViewRowEditor::OnEnChange()
{
    if (GetMessageTarget())
    {
        GetMessageTarget()->SendMessage(WM_MUSIKEDITCHANGE, (WPARAM)this, (LPARAM)m_Args);
    }
}

bool ListViewRowEditor::IsVisible()
{
    return m_IsVisible;
}

HBRUSH ListViewRowEditor::CtlColor(CDC* pDC, UINT nCtlColor)
{
    HBRUSH hbr;

    pDC->SetBkColor(Globals::Preferences->GetListCtrlColor());
    pDC->SetTextColor(Globals::Preferences->GetListCtrlTextColor());
    hbr = (HBRUSH)m_brBg;

    return hbr;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube
