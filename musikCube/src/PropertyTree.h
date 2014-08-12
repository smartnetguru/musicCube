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

#include "PropertyTreeItem.h"
#include "ListViewRowEditor.h"
#include "Preferences.h"

#include <musikCore.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

#define PROPTREEITEM_SPACE      5
#define PROPTREEITEM_DEFHEIGHT  22          // default heigt of an item

#define PTS_NOTIFY              0x00000001

#define HTPROPFIRST             50
#define HTLABEL                 (HTPROPFIRST + 0)
#define HTEXPAND                (HTPROPFIRST + 2)
#define HTATTRIBUTE             (HTPROPFIRST + 3)
#define RTCLKITEM               778997
#define LFCLKITEM               778998

#define PTN_FIRST               (0U-1100U)
#define PTN_INSERTITEM          (PTN_FIRST-1)
#define PTN_DELETEITEM          (PTN_FIRST-2)
#define PTN_DELETEALLITEMS      (PTN_FIRST-3)
#define PTN_ITEMCHANGED         (PTN_FIRST-4)
#define PTN_ITEMBUTTONCLICK     (PTN_FIRST-5)
#define PTN_SELCHANGE           (PTN_FIRST-6)
#define PTN_ITEMEXPANDING       (PTN_FIRST-7)
#define PTN_PROPCLICK           (PTN_FIRST-8)
#define PTN_MOUSEOVERNEW        (PTN_FIRST-9)
#define PTN_ITEMDRAG            (PTN_FIRST-10)

enum
{
    MUSIK_SOURCES_TYPE_QUICKSEARCH = musik::Core::MUSIK_PLAYLIST_TYPE_COUNT,
    MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST,
    MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST,
    MUSIK_SOURCES_TYPE_NETSTREAMS,
    MUSIK_SOURCES_TYPE_PLUGIN,
    MUSIK_SOURCES_TYPE_COLLECTED
};

typedef BOOL (CALLBACK* ENUMPROPITEMPROC)(PropertyTree*, PropertyTreeItem*, LPARAM);

class PropertyTree;
class PropertyTreeItem;

///////////////////////////////////////////////////
// NMPROPTREE, PNMPROPTREE, FAR LPNMPROPTREE
///////////////////////////////////////////////////

typedef struct _NMPROPTREE
{
    NMHDR           hdr;
    PropertyTreeItem* pItem;
} NMPROPTREE, *PNMPROPTREE, FAR *LPNMPROPTREE;

///////////////////////////////////////////////////
// PropertyTree
///////////////////////////////////////////////////

class PropertyTree : public CWnd
{
    // so they can access the musik stuff
    friend class PropertyTreeItem;

public:
    // construct / destruct
    PropertyTree( UINT dropid );
    virtual ~PropertyTree();

    // overrides
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

    // fonts
    static CFont* GetNormalFont();
    static CFont* GetBoldFont();

    // item gets
    PropertyTreeItem* GetRootItem();
    PropertyTreeItem* GetFocusedItem();
    static PropertyTreeItem* m_Hovered;

    // item sets
    void SetFocusedItem(PropertyTreeItem* pItem);

    // items actions
    PropertyTreeItem* InsertItem(PropertyTreeItem* pItem, PropertyTreeItem* pParent = NULL);
    void DeleteItem(PropertyTreeItem* pItem, bool notify = true);
    void DeleteAllItems( bool notify = true );
    void EnsureVisible(PropertyTreeItem* pItem);
    virtual PropertyTreeItem* FindItem(const POINT& pt);
    PropertyTreeItem* FindItem(UINT nCtrlID);
    void DetachAllItems();

    // item checks
    BOOL IsItemVisible(PropertyTreeItem* pItem);

    // functions used by PropertyTreeItem...
    // should not call these directly
    void AddToVisibleList(PropertyTreeItem* pItem);
    void ClearVisibleList();
    void SetOriginOffset(LONG nOffset);
    void UpdatedItems();
    void UpdateMoveAllItems();
    void RefreshItems(PropertyTreeItem* pItem = NULL);
    void SelectItems(PropertyTreeItem* pItem, BOOL bSelect = TRUE);

    // misc
    const int& GetOrigin();
    void UpdateScrollbar();

    // item focus controls
    PropertyTreeItem *FocusPrev();
    PropertyTreeItem *FocusNext();
    vector<PropertyTreeItem*> m_pVisibleList;
    void CheckScrollVisible();
    bool IsScrollVisible(){ return m_ScrollVisible; }

    // Enumerates an item and all its child items
    BOOL EnumItems(PropertyTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam = 0L);

    // do a hit test on the control 
    // HitTest returns a HTxxxx code
    // HitTextEx returns the item
    LONG HitTest(const POINT& pt);
    PropertyTreeItem* HitTestEx(const POINT& pt);

    // non-mfc or custom messages
    LRESULT SendNotify(UINT nNotifyCode, PropertyTreeItem* pItem = NULL);

    // mfc message maps
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

    // F2...
    ListViewRowEditor m_EditInPlace;
    int m_LastBtn;

    int GetScrollPosition() { return this->m_ScrollPos; }

protected:

    // drag and drop related stuff
    UINT m_DropID;
    bool m_IsWinNT;
    bool m_ScrollVisible;

    // bitmap back buffer for flicker free drawing
    CBitmap m_BackBuffer;
    CSize m_BackBufferSize;

    // Initialize global resources, brushes, 
    // fonts, etc.
    void InitGlobalResources();

    // Free global resources, brushes, fonts, etc.
    void FreeGlobalResources();

    // Recursive version of DeleteItem
    void Delete(PropertyTreeItem* pItem);

    // Root level tree item
    PropertyTreeItem m_Root;

    // Pointer to the focused item (selected)
    PropertyTreeItem* m_pFocus;

    // PropTree scroll position
    int m_ScrollPos;

    // auto generated last created ID
    UINT m_nLastUID;

    // Number of PropertyTree controls in the current application
    static UINT s_nInstanceCount;

    // fonts
    static CFont* s_pNormalFont;
    static CFont* s_pBoldFont;

    // Used for enumeration
    static PropertyTreeItem*  s_pFound;

    // misc internal functions
    void RecreateBackBuffer(int cx, int cy);
    void CheckVisibleFocus();

    // protected mfc message maps
    afx_msg void OnEnable(BOOL bEnable);
    afx_msg void OnSysColorChange();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);

    // macros
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(PropertyTree)

    // mouse
    // (events)
private:    afx_msg void                OnMouseMove(UINT nFlags, CPoint point);
private:    afx_msg LRESULT             OnMouseLeave(WPARAM wParam, LPARAM lParam);
    // (variables)
private:    bool                        m_MouseTrack;

    // (callbacks)
 private:   static BOOL CALLBACK    EnumFindItem(PropertyTree* pProp, PropertyTreeItem* pItem, LPARAM lParam);
 private:   static BOOL CALLBACK    EnumSelectAll(PropertyTree*, PropertyTreeItem* pItem, LPARAM lParam);
 private:   static BOOL CALLBACK    EnumMoveAll(PropertyTree*, PropertyTreeItem* pItem, LPARAM);
 private:   static BOOL CALLBACK    EnumRefreshAll(PropertyTree*, PropertyTreeItem* pItem, LPARAM);

};

///////////////////////////////////////////////////

} } // namespace musik::Cube