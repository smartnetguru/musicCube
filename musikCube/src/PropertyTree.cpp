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

#include "stdafx.h"
#include "PropertyTree.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

#pragma warning (disable : 4311) // pointer truncation from 'HMENU' to 'UINT'
#pragma warning (disable : 4312) // conversion from 'DWORD' to 'HBRUSH' of greater size

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

static const CString g_OfficeFontName = _T("Tahoma");
static const CString g_DefaultFontName = _T("MS Sans Serif");

static int CALLBACK FontFamilyProcFonts(const LOGFONT FAR* lplf, const TEXTMETRIC FAR*, ULONG, LPARAM)
{
    ASSERT(lplf != NULL);
    CString strFont = lplf->lfFaceName;
    return strFont.CollateNoCase (g_OfficeFontName) == 0 ? 0 : 1;
}

///////////////////////////////////////////////////

PropertyTreeItem*   PropertyTree::m_Hovered = NULL;
UINT                PropertyTree::s_nInstanceCount;
CFont*              PropertyTree::s_pNormalFont;
CFont*              PropertyTree::s_pBoldFont;
PropertyTreeItem*   PropertyTree::s_pFound;

IMPLEMENT_DYNAMIC( PropertyTree, CWnd )

BEGIN_MESSAGE_MAP(PropertyTree, CWnd)
    ON_WM_ENABLE()
    ON_WM_SYSCOLORCHANGE()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_SETCURSOR()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_MOUSEWHEEL()
    ON_WM_KEYDOWN()
    ON_WM_GETDLGCODE()
    ON_WM_VSCROLL()
    ON_WM_RBUTTONDOWN()
    ON_WM_NCCALCSIZE()
    ON_WM_SHOWWINDOW()
    // mouse
    ON_WM_MOUSEMOVE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()

/*ctor*/ PropertyTree::PropertyTree( UINT dropid ) :
    m_ScrollPos(0),
    m_nLastUID(1),
    m_pFocus(NULL),
    m_BackBufferSize(0,0),
    m_MouseTrack(false)
{
    m_Root.Expand();

    // dropid
    m_DropID = dropid;
    m_IsWinNT = ( 0 == ( GetVersion() & 0x80000000 ) );

    // init global resources only once
    if (!s_nInstanceCount)
        InitGlobalResources();

    s_nInstanceCount++;
}

/*dtor*/ PropertyTree::~PropertyTree()
{
    DeleteAllItems(false);

    s_nInstanceCount--;

    // free global resource when ALL CmusikPropTrees are destroyed
    if (!s_nInstanceCount)
        FreeGlobalResources();
}

const int& PropertyTree::GetOrigin()
{
    return m_ScrollPos;
}

BOOL PropertyTree::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    CWnd* pWnd = this;

    LPCTSTR pszCreateClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, ::LoadCursor(NULL, IDC_ARROW));

    return pWnd->Create(pszCreateClass, _T(""), dwStyle, rect, pParentWnd, nID);
}

void PropertyTree::OnSize(UINT nType, int cx, int cy) 
{
    CWnd::OnSize(nType, cx, cy);

    RecreateBackBuffer(cx, cy);

    UpdateScrollbar();
    Invalidate();
    UpdateWindow();

    // inform all items that a resize has been made
    UpdateMoveAllItems();
}

void PropertyTree::InitGlobalResources()
{
	CFont* temp = new CFont();
    temp->CreateStockObject(DEFAULT_GUI_FONT);
    LOGFONT lf;
    temp->GetLogFont(&lf);
	delete temp;

	s_pNormalFont = new CFont();
    s_pNormalFont->CreateFontIndirect(&lf);

    lf.lfWeight = FW_BOLD;
    s_pBoldFont = new CFont;
    s_pBoldFont->CreateFontIndirect(&lf);
}

void PropertyTree::FreeGlobalResources()
{
    if (s_pNormalFont)
    {
        delete s_pNormalFont;
        s_pNormalFont = NULL;
    }

    if (s_pBoldFont)
    {
        delete s_pBoldFont;
        s_pBoldFont = NULL;
    }
}

CFont* PropertyTree::GetNormalFont()
{
    return s_pNormalFont;
}

CFont* PropertyTree::GetBoldFont()
{
    return s_pBoldFont;
}

PropertyTreeItem* PropertyTree::GetFocusedItem()
{
    return m_pFocus;
}

PropertyTreeItem* PropertyTree::GetRootItem()
{
    return &m_Root;
}

void PropertyTree::ClearVisibleList()
{
    m_pVisibleList.clear();
}

void PropertyTree::AddToVisibleList(PropertyTreeItem* pItem)
{
    m_pVisibleList.push_back( pItem );
}

BOOL PropertyTree::EnumItems(PropertyTreeItem* pItem, ENUMPROPITEMPROC proc, LPARAM lParam)
{
    if (!pItem || !proc)
        return FALSE;

    PropertyTreeItem* pNext;

    // don't count the root item in any enumerations
    if (pItem!=&m_Root && !proc(this, pItem, lParam))
        return FALSE;

    // recurse thru all child items
    pNext = pItem->GetChild();

    while (pNext)
    {
        if (!EnumItems(pNext, proc, lParam))
            return FALSE;

        pNext = pNext->GetNext();
    }

    return TRUE;
}

void PropertyTree::SetOriginOffset(LONG nOffset)
{
    m_ScrollPos = nOffset;
}

void PropertyTree::UpdatedItems()
{
    if (!IsWindow(m_hWnd))
        return;

    Invalidate();
}

void PropertyTree::DeleteAllItems( bool notify )
{
    PropertyTreeItem* pFirst = m_Root.GetChild();

    while (pFirst)
    {
        PropertyTreeItem* pNext = pFirst->GetNext();
        Delete(pFirst);
        pFirst = pNext;
    }

    m_Root.SetChild(NULL);

    if ( notify )
    {
        UpdatedItems();
    }

    m_nLastUID = 1; // reset uid counter
}

void PropertyTree::DeleteItem(PropertyTreeItem* pItem, bool notify)
{
    Delete(pItem);

    if ( notify )
    {
        if (pItem && pItem!=&m_Root && SendNotify(PTN_DELETEITEM, pItem))
            return;
        UpdatedItems();
    }
}

void PropertyTree::DetachAllItems()
{
    PropertyTreeItem* pFirst = GetRootItem()->GetChild();
    while ( pFirst )
    {
        PropertyTreeItem* pNext = pFirst->GetNext();
        //
        pFirst->Detach();
        pFirst = pNext;
    }
}

void PropertyTree::Delete(PropertyTreeItem* pItem)
{
    if (pItem)
    {
        if (pItem->GetChild())
        {
            PropertyTreeItem* pChild = pItem->GetChild();
            while (pChild)
            {
                PropertyTreeItem* pNext = pChild->GetNext();
                Delete(pChild);
                pChild = pNext;
            }
        }

        if (GetFocusedItem() == pItem)
        {
            SetFocusedItem(NULL);
        }

        pItem->Detach();
        delete pItem;
    }
}

void PropertyTree::SetFocusedItem(PropertyTreeItem* pItem)
{
    m_pFocus = pItem;

    if ( pItem )
    {
        pItem->Select(TRUE);
        EnsureVisible(m_pFocus);
    }

    if (!IsWindow(m_hWnd))
        return;

    Invalidate();
}

BOOL PropertyTree::IsItemVisible(PropertyTreeItem* pItem)
{
    for ( size_t i = 0; i < m_pVisibleList.size(); i++ )
    {
        if ( pItem == m_pVisibleList.at( i ) )
            return TRUE;
    }
    return FALSE;
}

void PropertyTree::EnsureVisible(PropertyTreeItem* pItem)
{
    if (!pItem)
        return;

    // item is not scroll visible (expand all parents)
    if (!IsItemVisible(pItem))
    {
        PropertyTreeItem* pParent;

        pParent = pItem->GetParent();
        while (pParent)
        {
            pParent->Expand();
            pParent = pParent->GetParent();
        }

        UpdatedItems();
        UpdateWindow();
    }

    CRect rc;

    GetClientRect(rc);
    rc.OffsetRect(0, m_ScrollPos);
    rc.bottom -= pItem->GetHeight();

    CPoint pt;

    pt = pItem->GetLocation();

    if (!rc.PtInRect(pt))
    {
        LONG oy;

        //if (pt.y < rc.top)
            oy = pt.y;
        //else
        //	oy = pt.y - rc.Height() + pItem->GetHeight();

        OnVScroll(SB_THUMBTRACK, oy, NULL);
    }
}

PropertyTreeItem* PropertyTree::InsertItem(PropertyTreeItem* pItem, PropertyTreeItem* pParent)
{
    if (!pItem)
        return NULL;

    if (!pParent)
        pParent = &m_Root;

    if ( !pParent->GetChild() )
    {
        pParent->SetChild( pItem );
        pItem->SetPrev( NULL );
        pItem->SetNext( NULL );
        pItem->SetParent( pParent );
    }

    else
    {
        PropertyTreeItem* pLast = pParent->GetChild();
        while (pLast->GetNext())
            pLast = pLast->GetNext();

        pLast->SetNext( pItem );
        pItem->SetPrev( pLast );
        pItem->SetNext( NULL );
        pItem->SetParent( pParent );
    }

    pItem->SetPropOwner( this );
    pItem->SetChild( NULL );

    // auto generate a default ID
    pItem->SetCtrlID( m_nLastUID++ );

    SendNotify( PTN_INSERTITEM, pItem );
    UpdatedItems();

    return pItem;
}

LONG PropertyTree::HitTest(const POINT& pt)
{
    POINT p = pt;

    PropertyTreeItem* pItem;

    // convert screen to tree coordinates
    p.y += m_ScrollPos;

    try
    {
        if ((pItem = FindItem(pt))!=NULL)
        {
            if (pItem->IsRootLevel())
                return HTEXPAND;

            return HTLABEL;
        }
    }
    catch (...)
    {

    }

    return HTCLIENT;
}

PropertyTreeItem* PropertyTree::HitTestEx( const POINT& pt )
{
    POINT p = pt;

    PropertyTreeItem* pItem;

    // convert screen to tree coordinates
    p.y += m_ScrollPos;

    if ((pItem = FindItem(pt))!=NULL)
        return pItem;

    return NULL;
}

PropertyTreeItem* PropertyTree::FindItem(const POINT& pt)
{
    CPoint p = pt;

    // convert screen to tree coordinates
    p.y += m_ScrollPos;

    // search the visible list for the item
    CPoint curloc;
    PropertyTreeItem* pItem = NULL;
    try
    {
        for ( size_t i = 0; i < m_pVisibleList.size(); i++ )
        {
            pItem = m_pVisibleList.at( i );
            curloc = pItem->GetLocation();
            if ( p.y >= curloc.y && p.y < curloc.y + pItem->GetHeight() )   
                return pItem;
        }
    }
    catch (...)
    {
        cout << "a strange, nonharmful event happened.\n";
        return NULL;
    }

    return NULL;
}

PropertyTreeItem* PropertyTree::FindItem(UINT nCtrlID)
{
    s_pFound = NULL;

    EnumItems(&m_Root, EnumFindItem, nCtrlID);

    return s_pFound;
}

BOOL CALLBACK PropertyTree::EnumFindItem(PropertyTree*, PropertyTreeItem* pItem, LPARAM lParam)
{
    ASSERT(pItem!=NULL);

    if (pItem->GetCtrlID()==(UINT)lParam)
    {
        s_pFound = pItem;
        return FALSE;
    }

    return TRUE;
}

void PropertyTree::SelectItems(PropertyTreeItem* pItem, BOOL bSelect)
{
    if (!pItem)
        pItem = &m_Root;

    EnumItems(pItem, EnumSelectAll, (LPARAM)bSelect);
}

PropertyTreeItem* PropertyTree::FocusPrev()
{
    PropertyTreeItem* pItem = GetFocusedItem();
	// Added check to prevent a crash when pressing the ESC key 
	// following UP_ARROW key on the search box.
	// TODO: Probably should find a way to enable moving with the keyboard.
	if ( pItem && pItem->GetPrev() )
    {
        SetFocusedItem( pItem->GetPrev() );
        pItem->GetPrev()->Select();
        return pItem->GetPrev();
    }

    return NULL;
}

PropertyTreeItem* PropertyTree::FocusNext()
{
    PropertyTreeItem* pItem = GetFocusedItem();
	// Added check to prevent a crash when pressing the ESC key 
	// following UP_ARROW key on the search box.
	// TODO: Probably should find a way to enable moving with the keyboard.
	if ( pItem && pItem->GetNext() )
    {
        SetFocusedItem( pItem->GetNext() );
        pItem->GetNext()->Select();
        return pItem->GetNext();
    }

    return NULL;
}

void PropertyTree::UpdateMoveAllItems()
{
    EnumItems(&m_Root, EnumMoveAll);
}

void PropertyTree::RefreshItems(PropertyTreeItem* pItem)
{
    if (!pItem)
        pItem = &m_Root;

    EnumItems(pItem, EnumRefreshAll);

    UpdatedItems();
}

BOOL CALLBACK PropertyTree::EnumSelectAll(PropertyTree*, PropertyTreeItem* pItem, LPARAM lParam)
{
    if (!pItem)
        return FALSE;

    pItem->Select((BOOL)lParam);

    return TRUE;
}

BOOL CALLBACK PropertyTree::EnumRefreshAll(PropertyTree*, PropertyTreeItem* pItem, LPARAM)
{
    if (!pItem)
        return FALSE;

    pItem->OnRefresh();

    return TRUE;
}

BOOL CALLBACK PropertyTree::EnumMoveAll(PropertyTree*, PropertyTreeItem* pItem, LPARAM)
{
    if (!pItem)
        return FALSE;

    pItem->OnMove();

    return TRUE;
}

LRESULT PropertyTree::SendNotify(UINT nNotifyCode, PropertyTreeItem* pItem)
{
    if (!IsWindow(m_hWnd))
        return 0L;

    if (!(GetStyle() & PTS_NOTIFY))
        return 0L;

    NMPROPTREE nmmp;
    LPNMHDR lpnm;

    lpnm = NULL;

    switch (nNotifyCode)
    {
        case PTN_INSERTITEM:
        case PTN_DELETEITEM:
        case PTN_DELETEALLITEMS:
        case PTN_ITEMCHANGED:
        case PTN_ITEMBUTTONCLICK:
        case PTN_SELCHANGE:
        case PTN_ITEMEXPANDING:
        case PTN_PROPCLICK:
        case PTN_ITEMDRAG:
            lpnm = (LPNMHDR)&nmmp;
            nmmp.pItem = pItem;
            break;
    }

    if (lpnm)
    {
        UINT id = (UINT)::GetMenu(m_hWnd);
        lpnm->code = nNotifyCode;
        lpnm->hwndFrom = m_hWnd;
        lpnm->idFrom = id;

        return GetParent()->SendMessage(WM_NOTIFY, (WPARAM)id, (LPARAM)lpnm);
    }

    return 0L;
}

void PropertyTree::OnEnable(BOOL bEnable) 
{
    CWnd::OnEnable(bEnable);

    Invalidate();
}

void PropertyTree::OnSysColorChange() 
{
    CWnd::OnSysColorChange();

    Invalidate();
}

void PropertyTree::RecreateBackBuffer(int cx, int cy)
{
    if (m_BackBufferSize.cx<cx || m_BackBufferSize.cy<cy)
    {
        m_BackBufferSize = CSize(cx, cy);

        CWindowDC dc(NULL);

        int nPlanes = dc.GetDeviceCaps(PLANES);
        int nBitCount = dc.GetDeviceCaps(BITSPIXEL);

        m_BackBuffer.DeleteObject();
        m_BackBuffer.CreateBitmap(cx, cy, nPlanes, nBitCount, NULL);
    }
}

void PropertyTree::UpdateScrollbar()
{
    SCROLLINFO si;
    LONG nHeight;
    CRect rc;

    GetClientRect(rc);
    nHeight = rc.Height() + 1;

    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE|SIF_PAGE;
    si.nMin = 0;
    si.nMax = GetRootItem()->GetTotalHeight();
    si.nPage = nHeight + PROPTREEITEM_DEFHEIGHT;

    if ((int)si.nPage>si.nMax)
        SetOriginOffset(0);

    SetScrollInfo(SB_VERT, &si, TRUE);
    CheckScrollVisible();	// sets a flag thats used when drawing
}

void PropertyTree::CheckScrollVisible()
{
    CRect rc;
    GetClientRect(rc);
    if ( rc.Height() < (int)m_pVisibleList.size() * PROPTREEITEM_DEFHEIGHT )
        m_ScrollVisible = true;
    else
        m_ScrollVisible = false;
}

void PropertyTree::OnPaint() 
{
    CPaintDC dc(this);

    ClearVisibleList();

    // get client rect
    CRect rc;
    GetClientRect( rc );

    // draw control background
    CMemDC memdc( &dc );
    memdc.FillSolidRect( rc, Globals::Preferences->GetBtnFaceColor() );

    PropertyTreeItem* pItem;
    LONG nTotal = 0;

    ASSERT(GetRootItem()!=NULL);

    // create clip region
    HRGN hRgn = CreateRectRgn( rc.left, rc.top, rc.right, rc.bottom );
    SelectClipRgn( memdc.m_hDC, hRgn );

    // build a list of visible items prior to drawing. if
    // we decide to later implement trees with more than
    // one level, we will need a recursive function call
    // to do this.

    pItem = GetRootItem()->GetChild();
    while ( pItem )
    {
        m_pVisibleList.push_back( pItem );
        if ( pItem->IsExpanded() && pItem->GetChild() )
        {
            PropertyTreeItem* pNext = pItem->GetChild();
            while ( pNext )
            {
                m_pVisibleList.push_back( pNext );
                pNext = pNext->GetNext();
            }
        }
        pItem = pItem->GetNext();
    }

    // determine if the scrollbar is visible
    CheckScrollVisible();

    // actually draw the items
    for ( size_t i = 0; i < m_pVisibleList.size(); i++ )
    {
        LONG nHeight = m_pVisibleList.at( i )->DrawItem(&memdc, rc, 0, nTotal);
        nTotal += nHeight;
    }

    // remove clip region
    SelectClipRgn(memdc.m_hDC, NULL);
    DeleteObject(hRgn);
}

BOOL PropertyTree::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void PropertyTree::OnLButtonDown(UINT flags, CPoint point) 
{
    //
    // hide edit in place
    //
    if ( m_EditInPlace.IsVisible() )
        m_EditInPlace.Cancel();

    SendNotify(NM_CLICK);

    if (!IsWindowEnabled())
        return;

    SetFocus();

    LONG nHit = HitTest(point);

    PropertyTreeItem* pItem;
    CRect rc;

    switch (nHit)
    {
        case HTEXPAND:
            if ( flags != RTCLKITEM && (pItem = FindItem(point))!=NULL)
            {
                if (pItem->GetChild() && !SendNotify(PTN_ITEMEXPANDING, pItem))
                {
                    pItem->Expand(!pItem->IsExpanded());

                    UpdateScrollbar();
                    Invalidate();
                    UpdateWindow();
                    CheckVisibleFocus();
                }
            }
            break;

        default:

            if ((pItem = FindItem(point))!=NULL)
            {
                PropertyTreeItem* pOldFocus = GetFocusedItem();

                if ( pItem->IsRootLevel() )
                    return;

                SelectItems(NULL, FALSE);
                SetFocusedItem(pItem);

                pItem->Select();

                Invalidate();

                if ( flags == RTCLKITEM )
                    m_LastBtn = RTCLKITEM;
                else
                    m_LastBtn = LFCLKITEM;

                // don't send a sel change event for removable devices
                if (pItem!=pOldFocus )
                    SendNotify(PTN_SELCHANGE, pItem);

                if (nHit==HTATTRIBUTE && !pItem->IsRootLevel())
                {
                    if (!SendNotify(PTN_PROPCLICK, pItem))
                        pItem->Activate();
                }
            }
            break;
    }
}

void PropertyTree::OnLButtonDblClk(UINT, CPoint point)
{
    SendNotify(NM_DBLCLK);

    PropertyTreeItem* pItem;

    if ( (pItem = FindItem(point))!=NULL )
    {
        if ( pItem->IsRootLevel() )
        {
            if (pItem->GetChild() && !SendNotify(PTN_ITEMEXPANDING, pItem))
            {
                pItem->Expand(!pItem->IsExpanded());

                UpdateScrollbar();
                Invalidate();
                UpdateWindow();
                CheckVisibleFocus();
            }

            return;
        }

        PropertyTreeItem* pOldFocus = GetFocusedItem();
        SelectItems(NULL, FALSE);
        SetFocusedItem(pItem);

        pItem->Select();

        Invalidate();

        if ( !pItem->IsRootLevel() && pItem != pOldFocus )
            SendNotify(PTN_SELCHANGE, pItem);
    }

}

BOOL PropertyTree::OnMouseWheel(UINT, short zDelta, CPoint) 
{
    SCROLLINFO si;

    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE;

    GetScrollInfo(SB_VERT, &si);

    CRect rc;
    GetClientRect(rc);

    if (si.nMax - si.nMin < rc.Height())
        return TRUE;

    SetFocus();
    OnVScroll(zDelta < 0 ? SB_LINEDOWN : SB_LINEUP, 0, NULL);

    return TRUE;
}

void PropertyTree::OnKeyDown(UINT nChar, UINT, UINT) 
{
    PropertyTreeItem* pItem;

    switch (nChar)
    {
        case VK_RETURN:
            if ((pItem = GetFocusedItem())!=NULL && !pItem->IsRootLevel())
            {
                pItem->Activate();
            }
            break;

        case VK_LEFT:
            if ((pItem = GetFocusedItem())!=NULL)
            {
                if (!SendNotify(PTN_ITEMEXPANDING, pItem))
                {
                    if (pItem->GetChild() && pItem->IsExpanded())
                    {
                        pItem->Expand(FALSE);
                        UpdateScrollbar();
                        Invalidate();
                        UpdateWindow();
                        CheckVisibleFocus();
                        break;
                    }
                }
            }
            else
                break;
            // pass thru to next case VK_UP
        case VK_UP:
            if (FocusPrev())
                Invalidate();
            break;

        case VK_RIGHT:
            if ((pItem = GetFocusedItem())!=NULL)
            {
                if (!SendNotify(PTN_ITEMEXPANDING, pItem))
                {
                    if (pItem->GetChild() && !pItem->IsExpanded())
                    {
                        pItem->Expand();
                        UpdateScrollbar();
                        Invalidate();
                        UpdateWindow();
                        CheckVisibleFocus();
                        break;
                    }
                }
            }
            else
                break;
            // pass thru to next case VK_DOWN
        case VK_DOWN:
            if (FocusNext())
                Invalidate();
            break;
    }
}

UINT PropertyTree::OnGetDlgCode() 
{
    return DLGC_WANTARROWS|DLGC_WANTCHARS|DLGC_WANTALLKEYS;
}

void PropertyTree::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar*) 
{
    SCROLLINFO si;
    CRect rc;
    LONG nHeight;

    SetFocus();

    GetClientRect(rc);
    nHeight = rc.Height() + 1;

    ZeroMemory(&si, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_RANGE;

    GetScrollInfo(SB_VERT, &si);

    LONG ny = GetOrigin();

    switch (nSBCode)
    {
        case SB_LINEDOWN:
            ny += PROPTREEITEM_DEFHEIGHT;
            break;

        case SB_LINEUP:
            ny -= PROPTREEITEM_DEFHEIGHT;
            break;

        case SB_PAGEDOWN:
            ny += nHeight;
            break;

        case SB_PAGEUP:
            ny -= nHeight;
            break;

        case SB_THUMBTRACK:
            ny = nPos;
            break;
    }

    ny = __min(__max(ny, si.nMin), si.nMax - nHeight);

    SetOriginOffset(ny);
    si.fMask = SIF_POS;
    si.nPos = ny;

    SetScrollInfo(SB_VERT, &si, TRUE);
    CheckScrollVisible();	// sets a flag thats used when drawing
    Invalidate();
}

void PropertyTree::CheckVisibleFocus()
{
    PropertyTreeItem* pItem;
    
    if ((pItem = GetFocusedItem())==NULL)
        return;
}

void PropertyTree::OnRButtonDown(UINT nFlags, CPoint point)
{
    PropertyTree::OnLButtonDown(RTCLKITEM, point);
}

void PropertyTree::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
    CWnd::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void PropertyTree::OnMouseMove(UINT nFlags, CPoint point)
{
    if (!m_MouseTrack && !(nFlags & MK_LBUTTON))
    {
        TRACKMOUSEEVENT tme;
        tme.cbSize = sizeof(tme);
        tme.dwFlags = TME_LEAVE;
        tme.hwndTrack = m_hWnd;
        tme.dwHoverTime = HOVER_DEFAULT;
        ::_TrackMouseEvent(&tme);

        m_MouseTrack = true;
    }

    // test for drag and drop
    if (m_MouseTrack && (nFlags & MK_LBUTTON))
    {
        PropertyTreeItem* pItem = this->GetFocusedItem();
        //
        if (pItem)
        {
            this->SendNotify(PTN_ITEMDRAG, pItem);
        }
    }

    PropertyTreeItem* pItem = this->FindItem(point);
    if (pItem != PropertyTree::m_Hovered)
    {
        PropertyTree::m_Hovered = pItem;
        RedrawWindow();
    }
}

LRESULT PropertyTree::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    m_MouseTrack = false;
    PropertyTree::m_Hovered = NULL;
    Invalidate();
    return 1L;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube