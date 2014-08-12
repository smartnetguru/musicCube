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

struct IDropTargetHelper;

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

class PlaylistView;

class PlaylistViewDropTarget : public COleDropTarget
{
    friend class PlaylistView;

public:
    DROPEFFECT OnDragEnter (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    DROPEFFECT OnDragOver (CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
    BOOL OnDrop (CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

    DROPEFFECT OnDragScroll(CWnd* pWnd, DWORD dwKeyState, CPoint point);

    void OnDragLeave (CWnd* pWnd);

    PlaylistViewDropTarget (PlaylistView* pList, UINT uDropID_L, UINT uDropID_R, UINT uSourcesID, UINT uSelectionID_L, UINT uSelectionID_R);
    virtual ~PlaylistViewDropTarget();

protected:

    PlaylistView*    m_pList;
    IDropTargetHelper*    m_piDropHelper;
    UINT                m_uDropID_L;
    UINT                m_uDropID_R;
    UINT                m_uSourcesID;
    UINT                m_uSelectionID_L;
    UINT                m_uSelectionID_R;

    BOOL ReadHdropData (COleDataObject* pDataObject);
};

///////////////////////////////////////////////////

} } // namespace musik::Cube