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
#include "ListView.h"
#include "ListViewRowEditor.h"
#include "DockableWindow.h"

#include <musikCore.h>

#include <OpenThreads/Mutex>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

class MetadataFilterView;

///////////////////////////////////////////////////
// MetadataFilterViewContainer
///////////////////////////////////////////////////

class MetadataFilterViewContainer : public DockableWindow
{
    friend class MetadataFilterView;

public:

    // construct / destruct
    MetadataFilterViewContainer(CFrameWnd* parent, int type, int ctrlid, UINT dropid_l, UINT dropid_r);
    virtual ~MetadataFilterViewContainer();

    // gets
    MetadataFilterView* GetCtrl(){ return m_wndChild; }

    // mfc message maps
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSelectionboxRename();
    afx_msg void OnSelectionboxRemove();
    afx_msg void OnSelectionboxAddnew();
    afx_msg void OnChangetypeArtist();
    afx_msg void OnChangetypeAlbum();
    afx_msg void OnChangetypeYear();
    afx_msg void OnChangetypeGenre();
    afx_msg void OnChangetypeTracknumber();
    afx_msg void OnChangetypeTimeadded();
    afx_msg void OnChangetypeLastplayed();
    afx_msg void OnChangetypeFormat();
    afx_msg void OnChangetypeRating();
    afx_msg void OnChangetypeTimesplayed();
    afx_msg void OnChangetypeBitrate();
    afx_msg void OnDeleteFromlibrary();
    afx_msg void OnDeleteFromcomputer();
    afx_msg void OnSelectionboxcontextPlayselected();
    afx_msg void OnSelectionboxcontextQueueselected();
    afx_msg void OnSelectionboxcontextPlaynext();
    afx_msg void OnSelectionboxcontextReplacenowplaying();
    afx_msg void OnSelectionboxcontextReplacenowplayingandplay();

    // misc
    void ShowOptionsMenu();
    void ShowContextMenu();

    // showing options menu
    virtual void OnOptions();

protected:

    virtual CWnd* GetMainWindow() { return (CWnd*) this->m_wndChild; }

    // child
    MetadataFilterView* m_wndChild;
    CFont m_Font;

    // macros
    DECLARE_DYNAMIC(MetadataFilterViewContainer)
    DECLARE_MESSAGE_MAP()
};

///////////////////////////////////////////////////
// MetadataFilterView
///////////////////////////////////////////////////

class MetadataFilterView : public ListView
{
    friend class MetadataFilterViewContainer;
    static bool m_Updating;
    static int m_Count;

public:

    // construct / destruct
    MetadataFilterView(CFrameWnd* parent, int type, int ctrlid, UINT dropid_l, UINT dropid_r);
    virtual ~MetadataFilterView();

    // gets
    int GetCtrlID (){ return m_ID; }
    CString GetTypeDB();
    CString GetTypeStr();
    int    GetType(){ return m_Type; }
    void GetSelItems(musik::Core::StringArray& items, bool format_query = true);
    musik::Core::String GetEditCommitStr();

    // sets
    void SetParent(bool parent = true){ m_ParentBox = parent; }
    static void SetUpdating(bool updating = true){ m_Updating = updating; }

    // misc
    void RescaleColumn();
    void RenameSel();
    void DelSel(bool from_file = false);
    void OnNewSong();

    // queries
    bool IsItemSelected(int item);
    bool IsParent(){ return m_ParentBox; }

    // mfc message maps
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
    afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);

    // custom message maps
    LRESULT OnEditCommit(WPARAM wParam, LPARAM lParam);
    LRESULT OnEditCancel(WPARAM wParam, LPARAM lParam);



    // custom message maps
    afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

protected:

    void SetType(int type);

    // misc
    void InitFonts();
    void UpdateWindowCap(bool resize_col = true);
       
    // colors
    musik::Core::String m_CurrPlayingStr;

    // drag / drop id
    UINT m_DropID_L;
    UINT m_DropID_R;
    void BeginDrag(bool right_button);
    void InsertFirstItem();

    // F2...
    ListViewRowEditor m_EditInPlace;
    musik::Core::String m_CommitStr;

    // core
    CFrameWnd* m_Parent;
    musik::Core::StringArray m_Items;
    int m_Type;
    int m_ID;
    bool m_ParentBox;

    // fonts
    CFont m_Regular;
    CFont m_Bold;
    CFont m_StarFont;

    Mutex m_ProtectingUpdate;

    // macros
    DECLARE_DYNAMIC(MetadataFilterView)
    DECLARE_MESSAGE_MAP()


public:     // NEW !!!

    // (events)
    afx_msg void                OnSetFocus(CWnd* pOldWnd);
    afx_msg void                OnKillFocus(CWnd* pNewWnd);
    // (methods)
    void                        Reset(musik::Core::String playlistFilter = L"");
    void                        Query(musik::Core::String query);
    void                        Requery();
    void                        Clear();
    musik::Core::String         GetFullFilterExpression(musik::Core::String inputQuery);
    static musik::Core::String  GetFullQueryString(musik::Core::String criteria, int targetType, int orderByType = -1);
    musik::Core::String         GetSelectedItemsQuery();
    void                        GetSelectedItems(musik::Core::StringArray& items);
    void                        SetSelectedItems(musik::Core::StringArray& items);
    void                        GetSelectedFiles(musik::Core::StringArray& files);
    void                        ChangeType(int newType);
    //
    void                        NotifyParentSelectionChanged();
    // (variables)
    musik::Core::String         m_LastQuery;
    musik::Core::String         m_SearchFilter;
    bool                        m_SelectionChangeFired;
};

///////////////////////////////////////////////////

} } // namespace musik::Cube
