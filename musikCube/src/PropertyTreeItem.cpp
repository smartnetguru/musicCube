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
// I've lost record of where this code came from. Sorry for defacing it so
// badly. If its yours and you want credit let me know. -Casey
///////////////////////////////////////////////////

#include "stdafx.h"

#include <Wingdi.h>

#include "PropertyTree.h"
#include "PropertyTreeItem.h"
#include "GraphicsUtility.h"

//////////////////////////////////////////////////

namespace musik { namespace Cube {

//////////////////////////////////////////////////

PropertyTreeItem::PropertyTreeItem() :
    m_pProp(NULL),
    m_sLabel(_T("")),
    m_loc(0,0),
    m_rc(0,0,0,0),
    m_lParam(0),
    m_nCtrlID(0),
    m_dwState(0),
    m_bActivated(FALSE),
    m_bCommitOnce(FALSE),
    m_rcExpand(0,0,0,0),
    m_pParent(NULL),
    m_pChild(NULL),
    m_pNext(NULL),
    m_pPrev(NULL),
    m_pBmpOff(NULL),
    m_pBmpOn(NULL)
{
    m_Library = NULL;
}

PropertyTreeItem::~PropertyTreeItem()
{
    if ( m_Library )
    {
        UnloadLibrary();
    }
}

void PropertyTreeItem::SetBitmaps( Gdiplus::Bitmap* off, Gdiplus::Bitmap* on )
{
    m_pBmpOff = off;
    m_pBmpOn = on;
}

void PropertyTreeItem::SetPlaylistInfo(const musik::Core::PlaylistInfo& info)
{
    this->m_PlaylistInfo = info;
}

int PropertyTreeItem::GetPlaylistType()
{
    return m_PlaylistInfo.GetType();
}

int PropertyTreeItem::GetPlaylistID()
{
    return m_PlaylistInfo.GetID();
}

BOOL PropertyTreeItem::IsExpanded()
{
    return (m_dwState & TreeItemExpanded) ? TRUE : FALSE;
}

BOOL PropertyTreeItem::IsSelected()
{
    return (m_dwState & TreeItemSelected) ? TRUE : FALSE;
}

BOOL PropertyTreeItem::IsActivated()
{
    return (m_dwState & TreeItemActivated) ? TRUE : FALSE;
}

void PropertyTreeItem::Select(BOOL bSelect)
{
    if (bSelect)
        m_dwState |= TreeItemSelected;
    else
        m_dwState &= ~TreeItemSelected;
}

void PropertyTreeItem::Expand(BOOL bExpand)
{
    if (bExpand)
        m_dwState |= TreeItemExpanded;
    else
        m_dwState &= ~TreeItemExpanded;
}

BOOL PropertyTreeItem::HitExpand(const POINT& pt)
{
    return m_rcExpand.PtInRect(pt);
}

BOOL PropertyTreeItem::IsRootLevel()
{
    return GetParent() == m_pProp->GetRootItem();
}

LONG PropertyTreeItem::GetTotalHeight()
{
    PropertyTreeItem* pItem;
    LONG nHeight;

    nHeight = GetHeight();

    if (IsExpanded())
    {
        pItem = GetChild();
        while ( pItem )
        {
            nHeight += pItem->GetTotalHeight();
            pItem = pItem->GetNext();
        }
    }

    return nHeight;
}

LPCTSTR PropertyTreeItem::GetLabelText()
{
    return m_sLabel;
}

void PropertyTreeItem::SetCtrlID(UINT nCtrlID)
{
    m_nCtrlID = nCtrlID;
}

UINT PropertyTreeItem::GetCtrlID()
{
    return m_nCtrlID;
}

LONG PropertyTreeItem::GetHeight()
{
    return PROPTREEITEM_DEFHEIGHT;
}

LPARAM PropertyTreeItem::GetItemValue()
{
    // no items are assocatied with this type
    return 0L;
}

void PropertyTreeItem::SetItemValue(LPARAM)
{
    // no items are assocatied with this type
}

void PropertyTreeItem::OnMove()
{
    // no attributes, do nothing
}

void PropertyTreeItem::OnRefresh()
{
    // no attributes, do nothing
}

void PropertyTreeItem::OnCommit()
{
    // no attributes, do nothing
}

void PropertyTreeItem::Activate()
{
    m_bActivated = TRUE;
    m_bCommitOnce = FALSE;

    OnActivate();
}

void PropertyTreeItem::Deactivate()
{
    CommitChanges();
}

void PropertyTreeItem::CommitChanges()
{
    m_bActivated = FALSE;

    if (m_bCommitOnce)
        return;

    m_bCommitOnce = TRUE;

    ASSERT(m_pProp!=NULL);

    OnCommit();

    m_pProp->SendNotify(PTN_ITEMCHANGED, this);
    m_pProp->RefreshItems(this);
}

void PropertyTreeItem::OnActivate()
{
    // no attributes, do nothing
}

void PropertyTreeItem::SetPropOwner(PropertyTree* pProp)
{
    m_pProp = pProp;
}

const POINT& PropertyTreeItem::GetLocation()
{
    return m_loc;
}

PropertyTreeItem* PropertyTreeItem::GetParent()
{
    return m_pParent;
}

PropertyTreeItem* PropertyTreeItem::GetChild()
{
    return m_pChild;
}

PropertyTreeItem* PropertyTreeItem::GetNext()
{
    return m_pNext;
}

void PropertyTreeItem::SetParent(PropertyTreeItem* pParent)
{
    m_pParent = pParent;
}

void PropertyTreeItem::SetChild(PropertyTreeItem* pChild)
{
    m_pChild = pChild;
}

void PropertyTreeItem::SetNext(PropertyTreeItem* pNext)
{
    m_pNext = pNext;
}

PropertyTreeItem* PropertyTreeItem::Detach()
{
    PropertyTreeItem* pPrev = GetPrev();
    PropertyTreeItem* pNext = GetNext();

    // we are in the middle of the list
    if ( pPrev && pNext )
    {
        pPrev->SetNext( pNext );
        pNext->SetPrev( pPrev );
    }

    // no previous, link to our parent to the
    // next node in the list
    else if ( !pPrev && pNext )
    {
        GetParent()->SetChild( pNext );
        pNext->SetPrev( NULL );
    }

    // deleted the last item in the list
    else if ( pPrev && !pNext )
    {
        pPrev->SetNext( NULL );
    }

    // we were the only item (no left or right node)
    else if ( !pPrev && !pNext )
    {
        GetParent()->SetChild( NULL );
    }

    SetPrev( NULL );
    SetNext( NULL );

    return this;
}

void PropertyTreeItem::SetPrev(PropertyTreeItem* pPrev)
{
    m_pPrev = pPrev;
}

LONG PropertyTreeItem::DrawItem( CDC* pDC, const RECT& rc, LONG x, LONG y )
{
    CPoint pt;
    LONG nTotal;
    CRect drc, ir;

    ASSERT( m_pProp!=NULL );

    int right = rc.right;
    if ( m_pProp->IsScrollVisible() )
    {
        right -= 1;
    }

    bool isActiveLibrary = (this->m_Library) && (this->m_Library == Globals::Library);

    // store the item's location
    m_loc = CPoint(x, y);

    // store the items rectangle position
    m_rc.SetRect(PROPTREEITEM_SPACE, m_loc.y, right, m_loc.y + GetHeight()-1);
    m_rc.OffsetRect(0, -m_pProp->GetOrigin());

    // init temp drawing variables
    nTotal = GetHeight();

    // convert item coordinates to screen coordinates
    pt = m_loc;
    pt.y -= m_pProp->GetOrigin();

    // draws the label the whole size
    drc.SetRect(pt.x + 16, pt.y, right, pt.y + nTotal);

        HGDIOBJ hOld;

    // SH 2004-01-03 GDI leak cleanup : was previously plain old Win32 CreateSolidBrush without any variable
    // assignment and cleanup. Changed to CBrush, as its destructor handles cleanup properly.

    CBrush listBrush, gutterBrush;

    //the object
    if ( IsSelected() )
    {
        listBrush.CreateSolidBrush( Globals::Preferences->GetListCtrlColor() );
    }
    else if ( PropertyTree::m_Hovered == this || 
              GetPlaylistType() == MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST || 
              GetPlaylistType() == MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST )
    {
        COLORREF backColor;

        if ( IsRootLevel() )
        {
            backColor = GraphicsUtility::ShadeColor( Globals::Preferences->GetBtnFaceColor(), -40 );
        }
        else
        {
            backColor = GraphicsUtility::ShadeColor( Globals::Preferences->GetBtnFaceColor(), 20 );
        }

        listBrush.CreateSolidBrush( backColor ); 
    }
    else
    {
        COLORREF backColor;

        if ( IsRootLevel() )
        {
            backColor = GraphicsUtility::ShadeColor( Globals::Preferences->GetBtnFaceColor(), -20 );
        }
        else if (isActiveLibrary)
        {
            backColor = GraphicsUtility::ShadeColor( Globals::Preferences->GetBtnFaceColor(), 40 );
        }
        else
        {
            backColor = GraphicsUtility::ShadeColor( Globals::Preferences->GetBtnFaceColor(), 10 );
        }

        listBrush.CreateSolidBrush( backColor );
    }

    hOld = pDC->SelectObject( listBrush );
    pDC->PatBlt(rc.left, drc.top, right - rc.left + 1, drc.Height(), PATCOPY);

    pDC->SelectObject(hOld);

    //
    // calc label position
    //

    ir = drc;
    if ( IsRootLevel() )
    {
        ir.left -= 8;
    }
    else
    {
        if ( Globals::DrawGraphics && !IsRootLevel() )
        {
            ir.left += PROPTREEITEM_SPACE + 12;
        }
        else
        {
            ir.left -= 4;
        }
    }

    //
    // draw label
    //
    if ( !this->GetPlaylistInfo().GetName().IsEmpty() )
    {

        if ( IsSelected() || IsRootLevel() )
            pDC->SelectObject( PropertyTree::GetBoldFont() );
        else
            pDC->SelectObject( PropertyTree::GetNormalFont() );

        //
        // color
        //
        if ( IsSelected() )
        {
            pDC->SetTextColor( Globals::Preferences->GetListCtrlTextColor() );
        }
        else
        {
            pDC->SetTextColor( Globals::Preferences->GetBtnTextColor() );
        }

        pDC->SetBkMode(TRANSPARENT);
        pDC->DrawText(
            this->GetPlaylistInfo().GetName().c_str(),
            &ir, 
            DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS
        );
    }

    // remove clip region
    pDC->SelectObject(hOld);	
    HRGN hRgn = NULL;
    if (hRgn)
    {
        SelectClipRgn(pDC->m_hDC, NULL);
        DeleteObject(hRgn);
    }	

    // border around selected object
    COLORREF bord;
    if ( IsSelected() || this == PropertyTree::m_Hovered )
    {
        bord = Globals::Preferences->GetBtnShadowColor();
        
        int width = right;
        if ( !IsRootLevel() )
            width -= 6;
        if ( m_pProp->IsScrollVisible() )
            width += 1;

        pDC->Draw3dRect(
            IsRootLevel() ? 0 : 6,
            pt.y,
            width,
            PROPTREEITEM_DEFHEIGHT,
            bord, bord );
    }

    //
    // draw icon
    //

    if ( Globals::DrawGraphics )
    {
        Gdiplus::Bitmap* bmp = NULL;
        if ( IsSelected() || this == PropertyTree::m_Hovered )
            bmp = m_pBmpOn;
        else
            bmp = m_pBmpOff;

        if ( bmp )
        {
            Gdiplus::Graphics graphics( pDC->GetSafeHdc() );
            graphics.DrawImage( bmp, 10, pt.y + 2, 18, 18 );
        }
    }

    //
    // draw the gutter
    //

    if ( !IsRootLevel() )
    {
        if ( IsSelected() )
        {
            gutterBrush.CreateSolidBrush(Globals::Preferences->GetActiveCaptionColor());
            hOld = pDC->SelectObject( gutterBrush );
            pDC->PatBlt( rc.left, drc.top, 6, drc.Height(), PATCOPY);
        }
        else if (isActiveLibrary)
        {
            gutterBrush.CreateSolidBrush(
                GraphicsUtility::ShadeColor(Globals::Preferences->GetActiveCaptionColor(), 80));

            hOld = pDC->SelectObject( gutterBrush );
            pDC->PatBlt( rc.left, drc.top, 6, drc.Height(), PATCOPY);
        }
        else
        {
            gutterBrush.CreateSolidBrush( Globals::Preferences->GetBtnFaceColor() );
            hOld = pDC->SelectObject( gutterBrush );
            pDC->PatBlt( rc.left, drc.top, 6, drc.Height(), PATCOPY);
        }
    }

    return nTotal;
}

void PropertyTreeItem::UnloadLibrary()
{
    if ((m_Library) && (m_Library != Globals::MainLibrary))
    {
        Globals::Player->LibraryDetach(m_Library);  // let the player know we've detached, as it
                                                        // may need to remove some songs from this library

        if (Globals::Library == m_Library)
        {
            Globals::SetMainLibrary(Globals::MainLibrary);
        }

        Globals::Player->LibraryDetach(m_Library);

        delete m_Library;
        m_Library = NULL;
    }
}

///////////////////////////////////////////////////

} } // namespace musik::Cube