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

namespace musik { namespace Cube {

///////////////////////////////////////////////////

#define LCSB_CLIENTDATA     1
#define LCSB_NCOVERRIDE     2
#define SB_HORZ             0
#define SB_VERT             1
#define SB_BOTH             3

class ListView : public CListCtrl
{
public:
    ListView();
    virtual ~ListView();

    void HideScrollBars(int Type, int Which=SB_BOTH);

protected:

    // message maps
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

    // function the custom header control
    // will use as a callback after a column
    // has been moved
    virtual void OnDragColumn(int source, int dest);

    // gets a modified color
    COLORREF GetModColor(COLORREF color, int pixel_change = 15);
    bool m_MouseTrack;

    // macros
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(ListView)

private:

    // Prevent default compiler generation of these copy constructors (no bitwise copy)
    ListView& operator=(const ListView& x);
    ListView(const ListView& x);

    // scrollbars
    BOOL NCOverride;
    int Who;
};

///////////////////////////////////////////////////

} } // namespace musik::Cube