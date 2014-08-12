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

///////////////////////////////////////////////////

#include "musikCubeApp.h"
#include "3rdparty/BtnST.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

class FlatToggleButton : public CButtonST
{
public:

    FlatToggleButton(Gdiplus::Bitmap* inUnchecked = NULL, Gdiplus::Bitmap* inChecked = NULL);
    void SetBitmaps(Gdiplus::Bitmap* inUnchecked, Gdiplus::Bitmap* inChecked);

protected:

    Gdiplus::Bitmap* m_UncheckedBmp;
    Gdiplus::Bitmap* m_CheckedBmp;

    COLORREF m_Checked;

    virtual DWORD OnDrawBorder(CDC* pDC, CRect* rect);
    virtual DWORD OnDrawBackground(CDC* pDC, CRect* rect);
};  

///////////////////////////////////////////////////

} } // namespace musik::Cube
