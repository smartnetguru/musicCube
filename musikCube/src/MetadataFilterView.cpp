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

#include "stdafx.h"

#include "MetadataFilterView.h"
#include "musikCubeApp.h"
#include "MainFrame.h"
#include "GraphicsUtility.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// MetadataFilterViewContainer
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(MetadataFilterViewContainer, DockableWindow)

BEGIN_MESSAGE_MAP(MetadataFilterViewContainer, DockableWindow)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(ID_SELECTIONBOXCONTEXT_RENAME, OnSelectionboxRename)
    ON_COMMAND(ID_SELECTIONBOX_CLOSEVIEW, OnSelectionboxRemove)
    ON_COMMAND(ID_SELECTIONBOX_ADDVIEW, OnSelectionboxAddnew)
    ON_COMMAND(ID_CHANGETYPE_ARTIST, OnChangetypeArtist)
    ON_COMMAND(ID_CHANGETYPE_ALBUM, OnChangetypeAlbum)
    ON_COMMAND(ID_CHANGETYPE_YEAR, OnChangetypeYear)
    ON_COMMAND(ID_CHANGETYPE_GENRE, OnChangetypeGenre)
    ON_COMMAND(ID_CHANGETYPE_TRACKNUMBER, OnChangetypeTracknumber)
    ON_COMMAND(ID_CHANGETYPE_TIMEADDED, OnChangetypeTimeadded)
    ON_COMMAND(ID_CHANGETYPE_LASTPLAYED, OnChangetypeLastplayed)
    ON_COMMAND(ID_CHANGETYPE_FORMAT, OnChangetypeFormat)
    ON_COMMAND(ID_CHANGETYPE_RATING, OnChangetypeRating)
    ON_COMMAND(ID_CHANGETYPE_TIMESPLAYED, OnChangetypeTimesplayed)
    ON_COMMAND(ID_CHANGETYPE_BITRATE, OnChangetypeBitrate)
    ON_COMMAND(ID_DELETE_FROMLIBRARY, OnDeleteFromlibrary)
    ON_COMMAND(ID_DELETE_FROMCOMPUTER, OnDeleteFromcomputer)
    ON_COMMAND(ID_SELECTIONBOXCONTEXT_PLAYSELECTED, OnSelectionboxcontextPlayselected)
    ON_COMMAND(ID_SELECTIONBOXCONTEXT_QUEUESELECTED, OnSelectionboxcontextQueueselected)
    ON_COMMAND(ID_SELECTIONBOXCONTEXT_PLAYNEXT, OnSelectionboxcontextPlaynext)
    ON_COMMAND(ID_SELECTIONBOXCONTEXT_REPLACENOWPLAYING, OnSelectionboxcontextReplacenowplaying)
    ON_COMMAND(ID_SELECTIONBOXCONTEXT_REPLACENOWPLAYINGANDPLAY, OnSelectionboxcontextReplacenowplayingandplay)
END_MESSAGE_MAP()

/*ctor*/ MetadataFilterViewContainer::MetadataFilterViewContainer(CFrameWnd* parent, int type, int ctrlid, UINT dropid_l, UINT dropid_r)
    : DockableWindow()
{
    m_wndChild = new MetadataFilterView(parent, type, ctrlid, dropid_l, dropid_r);
}

/*dtor*/ MetadataFilterViewContainer::~MetadataFilterViewContainer()
{
    delete m_wndChild;
}

int MetadataFilterViewContainer::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (DockableWindow::OnCreate(lpCreateStruct) == -1)
        return -1;

    // dock bar
    SetBarStyle(GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    EnableDocking(CBRS_ALIGN_ANY);

    // child
    long dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_OWNERDATA | LVS_NOCOLUMNHEADER;
    long dwStyleEx = LVS_EX_FULLROWSELECT;

    if (!m_wndChild->Create(dwStyle, CRect(0, 0, 0, 0), this, 123))
        return -1;

    m_wndChild->SetExtendedStyle(dwStyleEx);
    m_wndChild->ModifyStyleEx(WS_EX_STATICEDGE, NULL);

    if (!m_Font.CreateStockObject(DEFAULT_GUI_FONT))
        return -1;

    m_wndChild->SetFont(&m_Font);

    return 0;
}

void MetadataFilterViewContainer::OnSize(UINT nType, int cx, int cy)
{
    DockableWindow::OnSize(nType, cx, cy);
    m_wndChild->RescaleColumn();
}

void MetadataFilterViewContainer::OnOptions()
{
    ShowOptionsMenu();
}

void MetadataFilterViewContainer::OnSelectionboxRename()
{
    GetCtrl()->RenameSel();
}

void MetadataFilterViewContainer::ShowContextMenu()
{
    CPoint pos;
    ::GetCursorPos(&pos);

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_SELECTION_BOX_CONTEXT_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // check / enable / disable menu items
    if (GetCtrl()->GetSelectedCount() == 0)
    {
        popup_menu->EnableMenuItem(ID_SELECTIONBOXCONTEXT_RENAME, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_DELETE_FROMLIBRARY, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_DELETE_FROMCOMPUTER, MF_DISABLED | MF_GRAYED);

        popup_menu->EnableMenuItem(ID_SELECTIONBOXCONTEXT_PLAYSELECTED, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOXCONTEXT_QUEUESELECTED, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOXCONTEXT_PLAYNEXT, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOXCONTEXT_REPLACENOWPLAYING, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOXCONTEXT_REPLACENOWPLAYINGANDPLAY, MF_DISABLED | MF_GRAYED);
    }
        
    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
}

void MetadataFilterViewContainer::ShowOptionsMenu()
{
    CPoint pos;
    ::GetCursorPos(&pos);

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_SELECTION_BOX_OPTIONS_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // check / enable / disable menu items
    if (Globals::Preferences->GetSelBoxCount() == 1)
        popup_menu->EnableMenuItem(ID_SELECTIONBOX_CLOSEVIEW, MF_DISABLED | MF_GRAYED);

    if (GetCtrl()->GetSelectedCount() == 0)
        popup_menu->EnableMenuItem(ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED);
        
    // hide these, they aren't really releveant...
    popup_menu->RemoveMenu(ID_CHANGETYPE_LASTPLAYED, MF_BYCOMMAND);
    popup_menu->RemoveMenu(ID_CHANGETYPE_TIMEADDED, MF_BYCOMMAND);

    int type = GetCtrl()->GetType();
    switch(type)
    {
    case musik::Core::MUSIK_LIBRARY_TYPE_ARTIST:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_ARTIST, MF_CHECKED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_ALBUM:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_ALBUM, MF_CHECKED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_YEAR:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_YEAR, MF_CHECKED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_GENRE:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_GENRE, MF_CHECKED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_TRACKNUMBER, MF_CHECKED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_TIMEADDED:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_TIMEADDED, MF_CHECKED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_LASTPLAYED:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_LASTPLAYED, MF_CHECKED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_FORMAT:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_FORMAT, MF_CHECKED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_RATING:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_RATING, MF_CHECKED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_TIMESPLAYED:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_TIMESPLAYED, MF_CHECKED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED);
        break;
    case musik::Core::MUSIK_LIBRARY_TYPE_BITRATE:
        popup_menu->CheckMenuItem(ID_CHANGETYPE_BITRATE, MF_CHECKED);
        popup_menu->EnableMenuItem(ID_SELECTIONBOX_RENAME, MF_DISABLED | MF_GRAYED);
        break;
    }

    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
}

void MetadataFilterViewContainer::OnSelectionboxRemove()
{
    int WM_SELBOXADDREMOVE = RegisterWindowMessage(_T("SELBOXADDREMOVE"));
    GetCtrl()->m_Parent->PostMessage(WM_SELBOXADDREMOVE, (WPARAM)FALSE, (LPARAM)this);
}

void MetadataFilterViewContainer::OnSelectionboxAddnew()
{
    int WM_SELBOXADDREMOVE = RegisterWindowMessage(_T("SELBOXADDREMOVE"));
    GetCtrl()->m_Parent->PostMessage(WM_SELBOXADDREMOVE, (WPARAM)TRUE, NULL);
}

void MetadataFilterViewContainer::OnChangetypeArtist()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST);
}

void MetadataFilterViewContainer::OnChangetypeAlbum()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_ALBUM);
}

void MetadataFilterViewContainer::OnChangetypeYear()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_YEAR);
}

void MetadataFilterViewContainer::OnChangetypeGenre()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_GENRE);
}

void MetadataFilterViewContainer::OnChangetypeTracknumber()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM);
}

void MetadataFilterViewContainer::OnChangetypeTimeadded()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_TIMEADDED);
}

void MetadataFilterViewContainer::OnChangetypeLastplayed()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_LASTPLAYED);
}

void MetadataFilterViewContainer::OnChangetypeFormat()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_FORMAT);
}

void MetadataFilterViewContainer::OnChangetypeRating()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_RATING);
}

void MetadataFilterViewContainer::OnChangetypeTimesplayed()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_TIMESPLAYED);
}

void MetadataFilterViewContainer::OnChangetypeBitrate()
{
    this->GetCtrl()->SetType(musik::Core::MUSIK_LIBRARY_TYPE_BITRATE);
}

void MetadataFilterViewContainer::OnDeleteFromlibrary()
{
    GetCtrl()->DelSel();
}

void MetadataFilterViewContainer::OnDeleteFromcomputer()
{
    GetCtrl()->DelSel(true);
}

///////////////////////////////////////////////////
// MetadataFilterView
///////////////////////////////////////////////////

bool MetadataFilterView::m_Updating = false;
int MetadataFilterView::m_Count = 1;

int WM_SELECTION_EDIT_COMMIT = RegisterWindowMessage(_T("MUSIKEDITCOMMIT"));
int WM_SELECTION_EDIT_CANCEL = RegisterWindowMessage(_T("MUSIKEDITCANCEL"));

IMPLEMENT_DYNAMIC(MetadataFilterView, ListView)

BEGIN_MESSAGE_MAP(MetadataFilterView, ListView)
    // mfc message maps
    ON_WM_CREATE()
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
    ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBegindrag)
    ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnLvnBeginrdrag)
    ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
    ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnLvnKeydown)
    ON_WM_ERASEBKGND()
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_CHAR()
    // custom message maps
    ON_REGISTERED_MESSAGE(WM_SELECTION_EDIT_COMMIT,OnEditCommit)
    ON_REGISTERED_MESSAGE(WM_SELECTION_EDIT_CANCEL,OnEditCancel)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

/*ctor*/ MetadataFilterView::MetadataFilterView(CFrameWnd* parent, int type, int ctrlid, UINT dropid_l, UINT dropid_r)
{
    m_Type = type;
    m_Parent = parent;
    m_ID = ctrlid;
    m_DropID_L = dropid_l;
    m_DropID_R = dropid_r;
    m_ParentBox = false;

    HideScrollBars(LCSB_NCOVERRIDE, SB_HORZ);

    InitFonts();
}

/*dtor*/ MetadataFilterView::~MetadataFilterView()
{
}

int MetadataFilterView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (ListView::OnCreate(lpCreateStruct) == -1)
        return -1;

    // set caption to reflect type
    InsertColumn(0, _T(""));
    UpdateWindowCap();

    // edit in place
    m_EditInPlace.Create(WS_CHILD | WS_CLIPCHILDREN, CRect(0, 0, 0, 0), this, 123);
    m_EditInPlace.EnableWindow(FALSE);

    CFont font;
    font.CreateStockObject(DEFAULT_GUI_FONT);
    m_EditInPlace.SetFont(&font);

    // set initial state
    this->RescaleColumn();

    return 0;
}

void MetadataFilterView::RescaleColumn()
{
    CRect client_size;
    GetClientRect(&client_size);

    SetColumnWidth(0, client_size.Width());
}

void MetadataFilterView::InsertFirstItem()
{
    // get Select by...
    musik::Core::String top = _T("Show All Songs");
    CString type = GetTypeStr();
    type.MakeLower();
    m_Items.insert(m_Items.begin(), top);
}

void MetadataFilterView::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
    LV_ITEM* pItem = &(pDispInfo)->item;

    // only need to worry about item text
    if (pItem->mask & LVIF_TEXT)
    {
        wchar_t* pStr = NULL;
        musik::Core::String sCurrItem;
        switch(pItem->iSubItem)
        {
        case 0:
            if (pItem->iItem >= 0)
            {

                // get current item
                bool caching = false;
                if (pItem->iItem >= (int)m_Items.size())
                {
                    pStr = _T("[musik.caching]");
                    caching = true;
                }
                else
                    sCurrItem = m_Items.at(pItem->iItem);

                // got a valid item, so go ahead and add it.
                if (!caching)
                {
                    switch (m_Type)
                    {

                    case musik::Core::MUSIK_LIBRARY_TYPE_RATING:
                        {
                            if (pItem->iItem > 0)
                            {
                                if (sCurrItem == _T("0"))
                                    pStr = _T("Unrated");

                                else if (sCurrItem == _T("1"))
                                    pStr = _T("-,,,,");

                                else if (sCurrItem == _T("2"))
                                    pStr = _T("--,,,");

                                else if (sCurrItem == _T("3"))
                                    pStr = _T("---,,");

                                else if (sCurrItem == _T("4"))
                                    pStr = _T("----,");

                                else if (sCurrItem == _T("5"))
                                    pStr = _T("-----");
                            }
                            else
                                pStr = (wchar_t*)sCurrItem.c_str();
                        }

                        break;

                    case musik::Core::MUSIK_LIBRARY_TYPE_LASTPLAYED:
                    case musik::Core::MUSIK_LIBRARY_TYPE_TIMESPLAYED:

                        if (sCurrItem.IsEmpty() || sCurrItem == _T("0"))
                        {
                            pStr = _T("(never)");
                            break;
                        }

                    case musik::Core::MUSIK_LIBRARY_TYPE_BITRATE:

                        if (sCurrItem == _T("0"))
                        {
                            pStr = _T("(unknown)");
                            break;
                        }

                    case musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM:

                        if (sCurrItem == _T("0"))
                        {
                            pStr = _T("(unknown)");
                            break;
                        }

                    default:
                        pStr = (wchar_t*)sCurrItem.c_str();
                        break;
                    }
                }

                if (pStr)
                {
                    size_t length = wcslen(pStr);
                    if (length > (size_t) pItem->cchTextMax)
                    {
                        length = pItem->cchTextMax - 1;
                    }

                    wcsncpy(pItem->pszText, pStr, length);
                    pItem->pszText[length] = 0;
                }
            }

            break;
        }
    }

    *pResult = 0;
}

void MetadataFilterView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;

    if ( ! m_Updating)
    {
        // only interested in state changes
        if (pNMLV->uChanged & LVIF_STATE)
        {
            // holding CTRL is a special case... we see if the current state
            // is different than the old state before we update the
            // selection
            if (GetKeyState(VK_CONTROL) < 0)
            {
                if ((pNMLV->uNewState & LVIS_SELECTED) != (pNMLV->uOldState & LVIS_SELECTED))
                {
                    this->NotifyParentSelectionChanged();
                    m_SelectionChangeFired = true;          // If we get focus but selection hasn't changed we will Notify, so set state var
                }
            }
            else
            {
                if (pNMLV->uNewState & LVIS_SELECTED)
                {
                    this->NotifyParentSelectionChanged();
                    m_SelectionChangeFired = true;          // ditto from directly above
                }
            }
        }
    }

}

CString MetadataFilterView::GetTypeDB()
{
    musik::Core::String ret = Globals::Library->GetSongFieldDBStr(m_Type);
    CString sRet = (CString)ret.c_str();
    return sRet;
}

CString MetadataFilterView::GetTypeStr()
{
    return (CString)Globals::Library->GetSongFieldStr(m_Type).c_str();
}

void MetadataFilterView::GetSelItems(musik::Core::StringArray& items, bool format_query)
{
    items.clear();
    
    int item;
    int count = 0;

    musik::Core::String item_str;

    for (int i = -1 ; ;)
    {
        item = GetNextItem(i, LVNI_SELECTED);

        if (item == -1)
            break;

        else
        {
            items.push_back(m_Items.at(item));
            if (format_query)
                items.at(count).Replace(_T("'"), _T("''"));
        }

        i = item;
        count++;
    }
}

musik::Core::String MetadataFilterView::GetSelectedItemsQuery()
{
    musik::Core::String sQuery;
    
    musik::Core::StringArray selected_items;
    GetSelItems(selected_items);

    sQuery += Globals::Library->GetSongFieldDBStr(m_Type);

    if (selected_items.size())
    {
        sQuery += _T(" IN (");    

        musik::Core::String sSel;
        for (size_t i = 0; i < selected_items.size(); i++)
        {
            sQuery += _T("'");
            sSel = selected_items.at(i);
            sQuery += sSel;
            sQuery += _T("'");    

            if (i < selected_items.size() - 1)
                sQuery += _T(",");
        }

        sQuery += _T(") ");
    }
    else
    {
        sQuery += _T(" LIKE '%' ");    
    }

    return this->GetFullFilterExpression(sQuery);
}

bool MetadataFilterView::IsItemSelected(int item)
{
    if (GetItemState(item, LVIS_SELECTED))
        return true;
    return false;
}

void MetadataFilterView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

    COLORREF clrChild = GetModColor(Globals::Preferences->GetListCtrlColor(), 10);

    *pResult = CDRF_DODEFAULT;
    
    // this is the first notifcation we'll receive. tell
    // the handler we want to recieve notifactions for 
    // each item in the list
    if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
        return;
    }

    // draw each sub item... we set colors here.
    if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
        CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);

        musik::Core::String item_str;

        if (pLVCD->nmcd.dwItemSpec < m_Items.size())
        {
            item_str = m_Items.at(pLVCD->nmcd.dwItemSpec);
        }

        // back ground color
        if (IsParent())
        {
            pLVCD->clrTextBk = Globals::Preferences->GetListCtrlColor();
        }
        else
        {
            pLVCD->clrTextBk = clrChild;
        }

        // text color
        pLVCD->clrText = Globals::Preferences->GetListCtrlTextColor();

        // this is the "Show All..."
        if (pLVCD->nmcd.dwItemSpec == 0)
        {
             pDC->SelectObject(m_Bold);
        }

        // items that are after "Show All..."
        else
        {
            pDC->SelectObject(m_Regular);
        }

        if    (
                 (Globals::Player->GetPlaying()->GetLibrary() == Globals::Library) 
              && (Globals::Player->IsPlaying() && m_CurrPlayingStr == m_Items.at(pLVCD->nmcd.dwItemSpec))
           )
        {
            pLVCD->clrText = Globals::Preferences->GetHilightTextColor();
            pLVCD->clrTextBk = GraphicsUtility::ShadeColor(Globals::Preferences->GetHilightColor(), 40);

            pDC->SelectObject(m_Bold);
        }
        else if (GetItemState(pLVCD->nmcd.dwItemSpec, LVIS_SELECTED))
        {
            pLVCD->clrText = Globals::Preferences->GetHilightTextColor();
            pLVCD->clrTextBk = Globals::Preferences->GetHilightColor();
        }

        if (m_Type == musik::Core::MUSIK_LIBRARY_TYPE_RATING)
        {
            if ((item_str != _T("0")) && (item_str != _T("Unrated")) && (pLVCD->nmcd.dwItemSpec > 0))
            {
                pDC->SelectObject(m_StarFont);
            }
        }

        *pResult = CDRF_NEWFONT;

        return;
    }
}

void MetadataFilterView::InitFonts()
{
    m_StarFont.CreatePointFont(100, _T("musik"));

    m_Regular.CreateStockObject(DEFAULT_GUI_FONT);

    LOGFONT pBoldFont;
    m_Regular.GetLogFont(&pBoldFont);
    pBoldFont.lfWeight = FW_BOLD;

    m_Bold.CreateFontIndirect(&pBoldFont);
}

BOOL MetadataFilterView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

void MetadataFilterView::OnPaint()
{
    CPaintDC dc(this);
    CRect rect;
    GetClientRect(&rect);
    CMemDC memDC(&dc, &rect);
    
    CRect headerRect;
    GetDlgItem(0)->GetWindowRect(&headerRect);
    ScreenToClient(&headerRect);
    dc.ExcludeClipRect(&headerRect);
       
    CRect clip;
    memDC.GetClipBox(&clip);
    
    if (IsParent())
        memDC.FillSolidRect(clip, Globals::Preferences->GetListCtrlColor());
    else
    {
        memDC.FillSolidRect(
            clip, 
            GetModColor(Globals::Preferences->GetListCtrlColor(), 10)
       );
    }
     
    DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

void MetadataFilterView::OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    // returning non-zero ignores the message,
    // and marquee never appears.
    *pResult = 1;
}

void MetadataFilterView::RenameSel()
{
    musik::Core::StringArray items;
    GetSelItems(items);

    // make sure somethign is selected
    if (!items.size())
        return;

    // first selected item rect
    CRect rcItem;
    GetItemRect(GetSelectionMark(), rcItem, LVIR_BOUNDS);
    rcItem.left += 2;

    if (rcItem.top < 0)
    {
        int height = rcItem.bottom - rcItem.top;
        rcItem.top = 0 + 2;
        rcItem.bottom = height + 2;
    }

    musik::Core::String item_str;

    if (GetSelectionMark() < (int)m_Items.size())
        item_str = m_Items.at(GetSelectionMark());

    m_EditInPlace.EnableWindow(TRUE);
    m_EditInPlace.MoveWindow(rcItem);
    m_EditInPlace.SetFocus();
    m_EditInPlace.SetString(item_str.c_str());
    m_EditInPlace.ShowWindow(SW_SHOWDEFAULT);    
}

LRESULT MetadataFilterView::OnEditCancel(WPARAM wParam, LPARAM lParam)
{
    m_EditInPlace.EnableWindow(FALSE);
    SetFocus();

    return 0L;
}

LRESULT MetadataFilterView::OnEditCommit(WPARAM wParam, LPARAM lParam)
{
    m_EditInPlace.EnableWindow(FALSE);
    SetFocus();

    m_CommitStr = m_EditInPlace.GetString();

    bool failed = false;

    switch(m_Type)
    {
    case musik::Core::MUSIK_LIBRARY_TYPE_YEAR:
        {
            int year = musik::Core::StringToInt(m_CommitStr.c_str());
            if (year == 0 || year < 1000 || year > 9999)
            {
                MessageBox(
                    _T("Invalid year entered. Please enter a four digit number, such as 2004."), 
                    _T(MUSIK_VERSION_STR), 
                    MB_ICONINFORMATION);

                failed = true;
                break;
            }
        }

        break;

    case musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM:
        if (!m_CommitStr.IsEmpty())
        {
            int track = musik::Core::StringToInt(m_CommitStr.c_str());
            if (track == 0)
            {
                MessageBox(
                    _T("Invalid track number entered. Please enter any number greater than 0, or leave the field blank for no track number."), 
                    _T(MUSIK_VERSION_STR),
                    MB_ICONINFORMATION);
                failed = true;
                break;
            }
        }
        else
            m_CommitStr = "0";

        break;

    }

    if (GetSelectedCount() == 1)
    {
        musik::Core::String item_str;


        if (GetSelectionMark() < (int)m_Items.size())
            item_str = m_Items.at(GetSelectionMark());

        if (item_str == m_CommitStr)
            failed = true;
    }

    if (!m_CommitStr.IsEmpty() && !failed)
    {
        int WM_SELBOXEDITCOMMIT = RegisterWindowMessage(_T("SELBOXEDITCOMMIT"));
        m_Parent->PostMessage(WM_SELBOXEDITCOMMIT, (WPARAM)this, (LPARAM)GetType());
    }

    return 1L;
}

musik::Core::String MetadataFilterView::GetEditCommitStr()
{
    return m_CommitStr;
}

void MetadataFilterView::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
    MetadataFilterViewContainer* parent = (MetadataFilterViewContainer*)GetParent();
    parent->ShowContextMenu();
}

void MetadataFilterView::UpdateWindowCap(bool resize_column)
{
    CString sType = (CString)Globals::Library->GetSongFieldStr(m_Type);

    if (m_Type != musik::Core::MUSIK_LIBRARY_TYPE_TIMEADDED && m_Type != musik::Core::MUSIK_LIBRARY_TYPE_LASTPLAYED && m_Type != musik::Core::MUSIK_LIBRARY_TYPE_TIMESPLAYED)
        sType += _T("s");

    if (resize_column)
    {
        LV_COLUMN column;
        column.mask = LVCF_TEXT;
        column.cchTextMax = 255;
        column.pszText = sType.GetBuffer(NULL);
        
        SetColumn(0, &column);
    }

    CString sVis;
    sVis.Format(
        _T(" (%d visible)"),
        m_Items.size() - 1);

    sType += sVis;

    if (m_SearchFilter.size())
    {
        CString sTitle;
        sTitle.Format(
            _T("%s (filter: '%s')"),
            sType,
            m_SearchFilter.c_str());

        sType = sTitle;
    }

    GetParent()->SetWindowText(sType);
}

void MetadataFilterView::SetType(int type)
{
    m_Type = type;
    this->Query(this->m_LastQuery);
    this->NotifyParentSelectionChanged();
}

void MetadataFilterView::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    BeginDrag(false);
    *pResult = 0;
}

void MetadataFilterView::OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    BeginDrag(true);
    *pResult = 0;
}

void MetadataFilterView::BeginDrag(bool right_button)
{
    COleDataSource datasrc;
    HGLOBAL        hgDrop;
    DROPFILES*     pDrop;
    CStringList    lsDraggedFiles;
    POSITION       pos;
    CString        sFile;
    UINT           uBuffSize = 0;
    TCHAR*         pszBuff;
    FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    // get a list of filenames with the currently
    // selected items...
    musik::Core::StringArray files;
    musik::Core::StringArray sel;
    GetSelItems(sel, false);

    //
    // our drag query will be the last thing we selected!
    //
    this->GetSelectedFiles(files);
    if (!files.size())
    {
        return;
    }

    // CStringList containing files
    for (size_t i = 0; i < files.size(); i++)
    {
        lsDraggedFiles.AddTail(files.at(i));
        uBuffSize += files.at(i).GetLength() + 1;
    }

    sel.clear();
    files.clear();

    // Add 1 extra for the final null char, and the size of the DROPFILES struct.
    uBuffSize = sizeof(DROPFILES) + sizeof(TCHAR) * (uBuffSize + 1);

    // Allocate memory from the heap for the DROPFILES struct.
    hgDrop = GlobalAlloc (GHND | GMEM_SHARE, uBuffSize);

    if (!hgDrop)
        return;

    pDrop = (DROPFILES*) GlobalLock (hgDrop);

    if (!pDrop)
    {
        GlobalFree (hgDrop);
        return;
    }

    // Fill in the DROPFILES struct.
    pDrop->pFiles = sizeof(DROPFILES);

    // If we're compiling for Unicode, set the Unicode flag in the struct to
    // indicate it contains Unicode strings.
    #ifdef _UNICODE
        pDrop->fWide = TRUE;
    #endif;

    // Copy all the filenames into memory after the end of the DROPFILES struct.
    pos = lsDraggedFiles.GetHeadPosition();
    pszBuff = (TCHAR*) (LPBYTE(pDrop) + sizeof(DROPFILES));

    while (pos)
    {
        wcscpy (pszBuff, (LPCTSTR) lsDraggedFiles.GetNext (pos));
        pszBuff = 1 + _tcschr (pszBuff, '\0');
    }

    GlobalUnlock (hgDrop);

    // Put the data in the data source.
    datasrc.CacheGlobalData (CF_HDROP, hgDrop, &etc);

    // Add in our own custom data, so we know that the drag originated from our 
    // window.  CMyDropTarget::DragEnter() checks for this custom format, and
    // doesn't allow the drop if it's present.  This is how we prevent the user
    // from dragging and then dropping in our own window.
    // The data will just be a dummy bool.
    HGLOBAL hgBool;

    hgBool = GlobalAlloc (GHND | GMEM_SHARE, sizeof(bool));

    if (NULL == hgBool)
    {
        GlobalFree (hgDrop);
        return;
    }

    // Put the data in the data source.
    if (right_button)
        etc.cfFormat = m_DropID_R;
    else
        etc.cfFormat = m_DropID_L;

    datasrc.CacheGlobalData (right_button ? m_DropID_R : m_DropID_L, hgBool, &etc);

    // Start the drag 'n' drop!
    DROPEFFECT dwEffect = datasrc.DoDragDrop (DROPEFFECT_COPY);

    // If the DnD completed OK, we remove all of the dragged items from our
    // list.
    switch (dwEffect)
    {
        case DROPEFFECT_COPY:
        case DROPEFFECT_MOVE:
        {
            // the copy completed successfully
            // do whatever we need to do here
            cout << _T("DND from playlist completed successfully. The data has a new owner.\n");
        }
        break;

        case DROPEFFECT_NONE:
        {
            // the copy completed successfully
            // on a windows nt machine.
            // do whatever we need to do here
            bool bDeletedAnything = false;
            if (! bDeletedAnything)
            {
                // The DnD operation wasn't accepted, or was canceled, so we 
                // should call GlobalFree() to clean up.
                GlobalFree (hgDrop);
                GlobalFree (hgBool);

                cout << _T("DND had a problem. We had to manually free the data.\n");
            }
        }

        break;
    }        

}

void MetadataFilterView::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    if (GetKeyState(VK_SHIFT) < 0)
    {
        if (pLVKeyDow->wVKey == VK_END || pLVKeyDow->wVKey == VK_HOME || pLVKeyDow->wVKey == VK_DOWN || pLVKeyDow->wVKey == VK_UP || pLVKeyDow->wVKey == VK_PRIOR || pLVKeyDow->wVKey == VK_NEXT)
        {
            int WM_SELBOXUPDATE = RegisterWindowMessage(_T("SELBOXUPDATE"));
            m_Parent->PostMessage(WM_SELBOXUPDATE, (WPARAM) this);
        }
    }
    else if (GetKeyState(VK_MENU) < 0)
    {
        if (pLVKeyDow->wVKey == VK_DELETE)
        {
            DelSel();
        }
    }
    else if (GetKeyState(VK_CONTROL) < 0)
    {
        if (pLVKeyDow->wVKey == VK_DELETE)
            DelSel(true);
    }
    else
    {
        if (pLVKeyDow->wVKey == VK_F2)
            RenameSel();
    }

    *pResult = 0;
}

void MetadataFilterView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (nChar == 8)   // backspace
    {
        if (m_SearchFilter != _T(""))
        {
            m_SearchFilter.resize(m_SearchFilter.size() - 1);
        }
    }
    else
    {
        wchar_t chr = (UINT)nChar;
        m_SearchFilter += chr;
    }

    this->Query(this->m_LastQuery);
    this->NotifyParentSelectionChanged();
}

void MetadataFilterView::DelSel(bool from_file)
{
    int WM_SELECTIONCTRLDELETESELECTED = RegisterWindowMessage(_T("SELECTIONCTRLDELETESELECTED"));
    m_Parent->PostMessage(WM_SELECTIONCTRLDELETESELECTED, (WPARAM)this, (LPARAM)from_file);
}

void MetadataFilterView::OnNewSong()
{
    if (Globals::Player && Globals::Player->IsPlaying())
        m_CurrPlayingStr = Globals::Player->GetPlaying()->GetField(m_Type);
}

BOOL MetadataFilterView::PreTranslateMessage(MSG* pMsg)
{
    if (m_SearchFilter != _T(""))
    {
        if (pMsg->message == WM_KEYDOWN)
        {
            if (pMsg->wParam == VK_ESCAPE)
            {
                m_SearchFilter = _T("");
                UpdateWindowCap(false);

                if (m_Items.size() > 0)
                {
                    SetUpdating(true);
                    SetSelectionMark(1);
                    SetUpdating(false);
                }

                return TRUE;
            }
        }
    }

    return ListView::PreTranslateMessage(pMsg);
}

void MetadataFilterView::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    // whenever an item is activated, the correct playlist
    // will already be displayed.
    int msg = RegisterWindowMessage(_T("SELBOXACTIVATE"));
    m_Parent->SendMessage(msg);

    *pResult = 0;
}

void MetadataFilterViewContainer::OnSelectionboxcontextPlayselected()
{
    musik::Core::String query = GetCtrl()->GetSelectedItemsQuery();
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    Globals::Library->QuerySongs(query, *playlist, this->GetCtrl()->m_Type);

    int idx = Globals::Player->GetIndex();
    Globals::Player->GetPlaylist()->InsertAfter(*playlist, idx);

    Globals::Player->Play(++idx);

    delete playlist;
}

void MetadataFilterViewContainer::OnSelectionboxcontextQueueselected()
{
    musik::Core::String query = GetCtrl()->GetSelectedItemsQuery();
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    Globals::Library->QuerySongs(query, *playlist, GetCtrl()->m_Type);
    
    musik::Core::Playlist* pNowPlaying = Globals::Player->GetPlaylist();
    if (pNowPlaying)
    {
        for(size_t i = 0; i < playlist->size(); i ++)
        {
            pNowPlaying->Add(playlist->at(i));
        }

        delete playlist;
    }
    else
    {
        Globals::Player->SetPlaylist(playlist);
    }
}

void MetadataFilterViewContainer::OnSelectionboxcontextPlaynext()
{
    musik::Core::String query = GetCtrl()->GetSelectedItemsQuery();
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    Globals::Library->QuerySongs(query, *playlist, this->GetCtrl()->m_Type);

    int idx = Globals::Player->GetIndex();
    Globals::Player->GetPlaylist()->InsertAfter(*playlist, idx);

    delete playlist;
}

void MetadataFilterViewContainer::OnSelectionboxcontextReplacenowplaying()
{
    musik::Core::String query = GetCtrl()->GetSelectedItemsQuery();
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    Globals::Library->QuerySongs(query, *playlist, this->GetCtrl()->m_Type);
    Globals::Player->SetPlaylist(playlist);
}

void MetadataFilterViewContainer::OnSelectionboxcontextReplacenowplayingandplay()
{
    musik::Core::String query = GetCtrl()->GetSelectedItemsQuery();
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    Globals::Library->QuerySongs(query, *playlist, this->GetCtrl()->m_Type);
    Globals::Player->SetPlaylist(playlist);
    Globals::Player->Play(0);
}

void MetadataFilterView::Reset(musik::Core::String playlistFilter)
{
    this->m_SearchFilter = L"";
    this->Query(playlistFilter);
}

void MetadataFilterView::Requery()
{
    musik::Core::StringArray selectedItems;
    this->GetSelectedItems(selectedItems);

    musik::Core::String queryString = this->m_LastQuery;

    this->Query(queryString);

    this->SetSelectedItems(selectedItems);
}

musik::Core::String MetadataFilterView::GetFullQueryString(musik::Core::String criteria, int targetType, int orderByType)
{
    // Builds the "SELECT [FIELD] FROM SONGS WHERE [CRITERIA];" part of the query

    musik::Core::String query, orderByString;
    //
    if (orderByType == -1)
    {
        orderByType = targetType;
    }

    orderByString = musik::Core::Library::GetOrder(orderByType, false, true);

    query += L"SELECT DISTINCT ";
    query += Globals::Library->GetSongFieldDBStr(targetType);
    query += L" FROM SONGS ";
    //
    if (criteria != L"")
    {
        query += L" WHERE ";
        query += criteria;
        query += L" ";
    }

    query += orderByString.c_str();

    return query;
}

musik::Core::String MetadataFilterView::GetFullFilterExpression(musik::Core::String inputQuery)
{
    // Gets the FULL query for the current state (selected items, filter) that
    // can be passed off to RawQueryStringArray

    musik::Core::String filterExpresion;
 
    if (this->m_SearchFilter.size())
    {
        filterExpresion.Format(
            L"LOWER(%ls) LIKE '%%%ls%%'",
            this->GetTypeDB().GetBuffer(),
            musik::Core::q_str(this->m_SearchFilter).c_str());
    }

    if (filterExpresion == L"")
    {
        return inputQuery;
    }
    else
    {
        if (inputQuery == L"")
        {
            return filterExpresion;
        }
        else
        {
            return inputQuery + L" AND " + filterExpresion;
        }
    }
}

void MetadataFilterView::Query(musik::Core::String query)
{
    m_ProtectingUpdate.lock();
    {
        this->SetUpdating(true);
        int nPos = GetScrollPos(SB_VERT);

        this->m_LastQuery = query;

        // ratings are special cased.
        m_Items.clear();
        if (m_Type == musik::Core::MUSIK_LIBRARY_TYPE_RATING)
        {
            m_Items.push_back(_T("0"));
            m_Items.push_back(_T("5"));
            m_Items.push_back(_T("4"));
            m_Items.push_back(_T("3"));
            m_Items.push_back(_T("2"));
            m_Items.push_back(_T("1"));
        }
        else
        {
            musik::Core::String fullFilter = MetadataFilterView::GetFullFilterExpression(query);
            //
            musik::Core::String fullQuery = MetadataFilterView::GetFullQueryString(fullFilter, this->GetType());
            //
            Globals::Library->RawQueryStringArray(fullQuery, this->m_Items);
        }
        
        this->InsertFirstItem();
        this->SetItemState(-1, 0, LVIS_SELECTED);
        this->SetItemCountEx(m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);

        this->SetScrollPos(SB_VERT, nPos, 0);

        this->UpdateWindowCap(false);
        this->RedrawWindow();
    }
    this->SetUpdating(false);
    m_ProtectingUpdate.unlock();
}

void MetadataFilterView::GetSelectedFiles(musik::Core::StringArray& files)
{
    musik::Core::String filterExpression = this->GetFullFilterExpression(this->m_LastQuery);
    {
        if (filterExpression != L"")
        {
            filterExpression += " AND ";
        }
    }

    filterExpression += this->GetSelectedItemsQuery();

    musik::Core::String filenameQuery = MetadataFilterView::GetFullQueryString(
        filterExpression,
        musik::Core::MUSIK_LIBRARY_TYPE_FILENAME,
        musik::Core::MUSIK_LIBRARY_TYPE_ARTIST);
    //
    Globals::Library->RawQueryStringArray(filenameQuery, files);
}

void MetadataFilterView::SetSelectedItems(musik::Core::StringArray &items)
{
    this->SetUpdating(true);

    musik::Core::String prevSelectedItem, currentItem;
    for (size_t i = 0; i < items.size(); i++)
    {
        prevSelectedItem = items[i];
        prevSelectedItem.ToLower();

        for (size_t j = 1; j < m_Items.size(); j++)     // start at 1 because of Show All Songs...
        {
            currentItem = m_Items[j];
            currentItem.ToLower();

            if (currentItem == prevSelectedItem)
            {
                SetItemState(j, LVIS_SELECTED, LVIS_SELECTED);
            }
            else if (currentItem > prevSelectedItem)
            {
                break;
            }
        }
    }

    this->SetUpdating(false);
}

void MetadataFilterView::GetSelectedItems(musik::Core::StringArray &items)
{
    items.clear();

    POSITION pos = GetFirstSelectedItemPosition();
    while (pos)
    {
        items.push_back(m_Items[GetNextSelectedItem(pos)]);
    }
}

void MetadataFilterView::NotifyParentSelectionChanged()
{
    int WM_SELBOXUPDATE = RegisterWindowMessage(_T("SELBOXUPDATE"));
    m_Parent->SendMessage(WM_SELBOXUPDATE, (WPARAM) this);
}

void MetadataFilterView::OnSetFocus(CWnd* pOldWnd)
{
    ListView::OnSetFocus(pOldWnd);

    if ( ! this->m_SelectionChangeFired)
    {
        this->NotifyParentSelectionChanged();
    }
    //
    this->m_SelectionChangeFired = false;
}

void MetadataFilterView::OnKillFocus(CWnd* pNewWnd)
{
    ListView::OnKillFocus(pNewWnd);

    this->m_SelectionChangeFired = false;
}

void MetadataFilterView::Clear()
{
    this->m_Items.clear();
    this->InsertFirstItem();
    this->SetItemCountEx(m_Items.size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
}

///////////////////////////////////////////////////

} } // namespace musik::Cube