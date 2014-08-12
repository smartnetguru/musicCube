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
// I've lost record of where this code came from. Sorry for defacing it so
// badly. If its yours and you want credit let me know. -Casey
///////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////

#include "musikCubeApp.h"
#include "Preferences.h"

#include <musikCore.h>
#include <atlimage.h>
#include <Gdiplus.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

class PropertyTree;

///////////////////////////////////////////////////

class PropertyTreeItem
{
public:

    // construct / destruct
    PropertyTreeItem();
    virtual ~PropertyTreeItem();

    // each item has playlist info, including id and type
    void SetPlaylistInfo( const musik::Core::PlaylistInfo& info );
    musik::Core::PlaylistInfo& GetPlaylistInfo() { return m_PlaylistInfo; }
    musik::Core::Library* m_Library;
    int GetPlaylistType();
    int GetPlaylistID();
    void SetBitmaps( Gdiplus::Bitmap* off, Gdiplus::Bitmap* on );

    // treeItem states
    BOOL IsExpanded();
    BOOL IsSelected();
    BOOL IsActivated();

    // set states
    void Select(BOOL bSelect = TRUE);
    void Expand(BOOL bExpand = TRUE);
    void UnloadLibrary();

    BOOL IsRootLevel();
    BOOL HitExpand(const POINT& pt);    // returns TRUE if the point is on the expand button
    LONG GetTotalHeight();              // returns the total height of the item and all its children
    LPCTSTR GetLabelText();             // Return the items label text

    void SetCtrlID(UINT nCtrlID);       // Set the item's ID
    UINT GetCtrlID();                   // Return the item's ID

    LONG DrawItem(CDC* pDC, const RECT& rc, LONG x, LONG y);

    void Activate();
    void Deactivate();
    void CommitChanges();

    // overrideables
    virtual LONG GetHeight();                   // Return the height of the item
    virtual LPARAM GetItemValue();              // Retrieve the item's attribute value
    virtual void SetItemValue(LPARAM lParam);   // Set the item's attribute value
    virtual void OnMove();                      // Called when attribute area has changed size
    virtual void OnRefresh();                   // Called when the item needs to refresh its data  
    virtual void OnCommit();                    // Called when the item needs to commit its changes
    virtual void OnActivate();                  // Called to activate the item

    //
    // Usually only PropertyTree should calls these
    //

    void SetPropOwner(PropertyTree* pProp);

    // Return the location of the PropItem
    const POINT& GetLocation();

    // TreeItem link pointer access
    PropertyTreeItem* GetParent();
    PropertyTreeItem* GetChild();
    PropertyTreeItem* GetNext();
    PropertyTreeItem* GetPrev(){ return m_pPrev; }

    // misc
    void SetParent(PropertyTreeItem* pParent);
    void SetChild(PropertyTreeItem* pChild);
    void SetNext(PropertyTreeItem* pNext );
    void SetPrev(PropertyTreeItem* pPrev );
    PropertyTreeItem* Detach();

protected:

    PropertyTree* m_pProp;          // PropertyTree class that this class belongs
    CString m_sLabel;               // TreeItem label name
    CPoint m_loc;                   // TreeItem location
    CRect m_rc;                     // TreeItem attribute size
    LPARAM m_lParam;                // user defined LPARAM value
    UINT m_nCtrlID;                 // ID of control item (should be unique)

    // MUSIK SPECIFIC: item's playlist information. ***THIS SHOULD NOT BE HERE***
    musik::Core::PlaylistInfo m_PlaylistInfo;
    Gdiplus::Bitmap* m_pBmpOff;
    Gdiplus::Bitmap* m_pBmpOn;

private:
    enum TreeItemStates
    {
        TreeItemSelected =  0x00000001,
        TreeItemExpanded =  0x00000002,
        TreeItemActivated = 0x00000010 // 0x00000020 = next
    };

    
    DWORD m_dwState;        // TreeItem state
    BOOL m_bActivated;      // TRUE if item is activated
    BOOL m_bCommitOnce;     // TRUE if item has been commited once (activation)
    CRect m_rcExpand;       // Rectangle position of the expand button (if contains one)

    // link pointers
    PropertyTreeItem* m_pParent;
    PropertyTreeItem* m_pChild;
    PropertyTreeItem* m_pNext;
    PropertyTreeItem* m_pPrev;
};

///////////////////////////////////////////////////

} } // namespace musik::Cube