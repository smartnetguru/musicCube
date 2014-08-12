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

#pragma once
#include "afxwin.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// PluginManagerDialog
///////////////////////////////////////////////////

class PluginManagerDialog : public CDialog
{

public:

    PluginManagerDialog(CWnd* pParent = NULL);

    virtual ~PluginManagerDialog();
    virtual BOOL OnInitDialog();
    afx_msg void OnLbnSelchangePlugins();
    afx_msg void OnBnClickedExecute();
    afx_msg void OnBnClickedAbout();

    CEdit m_Description;
    CListBox m_PluginNames;

    void Populate();

    enum { IDD = IDD_PLUGIN_MANAGER };

protected:

    int GetIndex();

    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_DYNAMIC(PluginManagerDialog)    
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnBnClickedClose();
    afx_msg void OnBnClickedConfigure();
    afx_msg void OnBnClickedStop();
    CButton m_ConfigureBtn;
    CButton m_ExecuteBtn;
    CButton m_StopBtn;
    CButton m_AboutBtn;
};

///////////////////////////////////////////////////

} } // namespace musik::Cube