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
//
// Information:
//
//   Class basically stolen from P.J Arends, see
//   http://www.codeproject.com/dialog/PJA_MultiSelect.asp
//
//   CFileDialog requires a static buffer size to retrieve
//   a list of files. This is not good, becuase we will
//   get an error if we select too many files. This dynamically
//   allocates memory for us so we don't have to worry about it.
//
//   Much thanks to the original author, P.J Arends.
// 
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// FileDialog
///////////////////////////////////////////////////

class FileDialog : public CFileDialog
{
    DECLARE_DYNAMIC(FileDialog)

public:
    FileDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
        LPCTSTR lpszDefExt = NULL,
        LPCTSTR lpszFileName = NULL,
        DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
        LPCTSTR lpszFilter = NULL,
        CWnd* pParentWnd = NULL);

    virtual ~FileDialog();

    // overrides
    virtual int DoModal();
    CString GetNextPathName(POSITION &pos) const;
    POSITION GetStartPosition();

protected:
    BOOL m_bParsed;
    TCHAR *m_pstrFolder;
    TCHAR *m_pstrFiles;

    virtual void OnFileNameChange();
    void CleanStrings();

    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////

} } // namespace musik::Cube
