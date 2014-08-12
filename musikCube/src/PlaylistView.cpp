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

#include "musikCubeApp.h"
#include "PlaylistView.h"
#include "PlaylistView.h"
#include "SourcesView.h"
#include "FileDropDialog.h"
#include "BatchTagDialog.h"
#include "GraphicsUtility.h"

#include "3rdparty/memdc.h"
#include "3rdparty/CShellFileOp.h"

///////////////////////////////////////////////////

#pragma warning(disable : 4244)

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// PlaylistView
///////////////////////////////////////////////////

int WM_TAGPROPERTIESDESTROY = RegisterWindowMessage(_T("TAGPROPERTIESDESTROY"));
int WM_TAGNEXT              = RegisterWindowMessage(_T("TAGNEXT"));
int WM_TAGPREV              = RegisterWindowMessage(_T("TAGPREV"));
int WM_TAGUPDATE            = RegisterWindowMessage(_T("TAGUPDATE"));

IMPLEMENT_DYNAMIC(PlaylistView, CWnd)

BEGIN_MESSAGE_MAP(PlaylistView, ListView)
    // mfc message maps
    ON_WM_CREATE()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnLvnGetdispinfo)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
    ON_NOTIFY_REFLECT(LVN_ODCACHEHINT, OnLvnOdcachehint)
    ON_NOTIFY_REFLECT(NM_CLICK, OnNMClick)
    ON_NOTIFY_REFLECT(LVN_ITEMACTIVATE, OnLvnItemActivate)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnLvnBegindrag)
    ON_NOTIFY_REFLECT(LVN_MARQUEEBEGIN, OnLvnMarqueeBegin)
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnLvnColumnclick)
    ON_NOTIFY_REFLECT(LVN_BEGINRDRAG, OnLvnBeginrdrag)
    ON_NOTIFY(HDN_ENDTRACKA, 0, OnHdnEndtrack)
    ON_NOTIFY(HDN_ENDTRACKW, 0, OnHdnEndtrack)
    ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnLvnKeydown)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnLvnItemchanged)
    // menu
    ON_COMMAND(ID_PLAYLISTCOLUMNS_ARTIST, OnPlaylistcolumnsArtist)
    ON_COMMAND(ID_PLC_DELETE_FROMPLAYLIST, OnPlcDeleteFromplaylist)
    ON_COMMAND(ID_PLC_DELETE_FROMLIBRARY, OnPlcDeleteFromlibrary)
    ON_COMMAND(ID_PLC_DELETE_FROMCOMPUTER, OnPlcDeleteFromcomputer)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, OnPlaylistcontextmenuShufflenowplaying)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_PROPERTIES, OnPlaylistcontextmenuProperties)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_ALBUM, OnPlaylistcolumnsAlbum)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_YEAR, OnPlaylistcolumnsYear)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_GENRE, OnPlaylistcolumnsGenre)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_TITLE, OnPlaylistcolumnsTitle)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_TRACKNUMBER, OnPlaylistcolumnsTracknumber)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_TIMEADDED, OnPlaylistcolumnsTimeadded)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_LASTPLAYED, OnPlaylistcolumnsLastplayed)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_FILESIZE, OnPlaylistcolumnsFilesize)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_FORMAT, OnPlaylistcolumnsFormat)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_DURATION, OnPlaylistcolumnsDuration)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_RATING, OnPlaylistcolumnsRating)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_TIMESPLAYED, OnPlaylistcolumnsTimesplayed)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_BITRATE, OnPlaylistcolumnsBitrate)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_FILENAME, OnPlaylistcolumnsFilename)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_BATCH, OnPlaylistcontextmenuBatch)
    ON_COMMAND(ID_FILENAME_TAGFROMFILENAME, OnFilenameTagfromfilename)
    ON_COMMAND(ID_FILENAME_TAGTOFILENAME, OnFilenameTagtofilename)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_TAGTOLOWERCASE, OnPlaylistcontextmenuTagtolowercase)
    ON_COMMAND(ID_TAG_CLEAN, OnPlaylistcontextmenuTagClean)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_TAGTOUPPERCASE, OnPlaylistcontextmenuTagtouppercase)
    ON_COMMAND(ID_TAG_AUTOCAPITALIZE, OnTagAutocapitalize)
    ON_COMMAND(ID_RATE_N1, OnRateN1)
    ON_COMMAND(ID_RATE_UNRATED, OnRateUn)
    ON_COMMAND(ID_RATE_1, OnRate1)
    ON_COMMAND(ID_RATE_2, OnRate2)
    ON_COMMAND(ID_RATE_3, OnRate3)
    ON_COMMAND(ID_RATE_4, OnRate4)
    ON_COMMAND(ID_RATE_5, OnRate5)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_RELOADTAGSFROMFILE, OnPlaylistcontextmenuReloadtagsfromfile)
    ON_COMMAND(ID_PLAYLISTCOLUMNS_COMMENTS, OnPlaylistcolumnsComments)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REMOVEWHITESPACE, OnPlaylistcontextmenuRemovewhitespace)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_QUEUESELECTEDINNOWPLAYING, OnPlaylistcontextmenuQueueselectedinnowplaying)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REWRITETAGSTOFILE, OnPlaylistcontextmenuRewritetagstofile)
    // custom messages
    ON_REGISTERED_MESSAGE(WM_TAGPROPERTIESDESTROY, OnTagEditDestroy)
    ON_REGISTERED_MESSAGE(WM_TAGNEXT, OnTagNext)
    ON_REGISTERED_MESSAGE(WM_TAGPREV, OnTagPrev)
    ON_REGISTERED_MESSAGE(WM_TAGUPDATE, OnTagUpdate)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_PLAYSELECTED, OnPlaylistcontextmenuPlayselected)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_PLAYNEXT, OnPlaylistcontextmenuPlaynext)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYING, OnPlaylistcontextmenuReplacenowplaying)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_SHOWINEXPLORER, OnPlaylistcontextmenuShowinexplorer)
    ON_COMMAND(ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYINGANDPLAY, OnPlaybackReplacenowplayingandplay)
END_MESSAGE_MAP()

/*ctor*/ PlaylistView::PlaylistView(CFrameWnd* mainwnd, PlaylistViewDropTarget* pDropTarget, UINT dropid_l, UINT dropid_r)
{
    Init(mainwnd, dropid_l, dropid_r);

    m_DropTarget = pDropTarget;
}

/*ctor*/ PlaylistView::PlaylistView()
{
    Init(NULL, 0, 0);
}

/*dtor*/ PlaylistView::~PlaylistView()
{
    if (m_Playlist && m_Playlist->GetType() != musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING)
        delete m_Playlist;

    if (m_TagEdit)
        delete m_TagEdit;

    if (m_MaskDlg)
        delete m_MaskDlg;

    if (m_SongInfoArray)
        delete m_SongInfoArray;

    delete m_SongInfoCache;

    if (this->GetType() == PLAYLISTCTRL_NORMAL)
    {
        if (m_DropTarget)
        {
            m_DropTarget->Revoke();
            delete m_DropTarget;
        }
    }
    
    delete m_InfoCtrl;
}

void PlaylistView::Init(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r)
{
    // core
    m_Playlist    = NULL;
    m_SongInfoArray = NULL;
    m_ViewType = PLAYLISTCTRL_NORMAL;

    // dialogs
    m_TagEdit = NULL;
    m_MaskDlg = NULL;

    // no sorting yet
    m_LastCol    = -1;

    // is a column being rearranged?
    m_DropTarget = NULL;
    m_Arranging = false;

    // dnd drop id
    m_DropID_L = dropid_l;
    m_DropID_R = dropid_r;

    // main window
    m_Parent = mainwnd;

    // info ctrl
    m_InfoCtrl = NULL;

    // misc
    m_DropArrange = false;
    m_Dragging = false;
    m_DropOverIndex = -1;
    m_PlaylistNeedsSave = false;
    m_SongInfoCache = new TrackMetadataCache(NULL);

    // dnd variables
    m_Marquee = false;

    // fonts and colors
    InitFonts();

    // get rating extent 
    GetRatingExtent();
}

void PlaylistView::GetPlaylistCols()
{
    m_Cols = Globals::Preferences->GetPlaylistOrder();
    m_Sizes = Globals::Preferences->GetPlaylistSizes();
}

void PlaylistView::ResetColumns(bool update)
{
    SetRedraw(FALSE);
    GetHeaderCtrl()->SetRedraw(FALSE);

    DeleteAllItems();

    musik::Core::Playlist* pList = GetPlaylist();
    m_Playlist = NULL;
    Refresh();

    while (DeleteColumn(0));

    GetPlaylistCols();

    bool rewrite = false;

    size_t times = m_Cols.size();
    for (size_t i = 0; i < times; i++)
    {
        if (m_Cols.at(i) > -1)
        {
            // KLUDGE: cannot figure out where or when this happens,
            // which leads me to believe its an MFC bug...
            if (m_Sizes.at(i) > 6000)
            {
                m_Sizes.at(i) = 100;
            }

            InsertColumn((int)i, Globals::Library->GetSongFieldStr(m_Cols.at(i)));
            SetColumnWidth((int)i, m_Sizes.at(i));
        }
        else
        {
            m_Cols.erase(m_Cols.begin() + i);
            m_Sizes.erase(m_Sizes.begin() + i);
            
            i--;
            times--;

            rewrite = true;
        }
    }

    if (rewrite)
    {
        Globals::Preferences->SetPlaylistOrder(m_Cols);
        Globals::Preferences->SetPlaylistSizes(m_Sizes);
    }

    SetRedraw(TRUE);
    GetHeaderCtrl()->SetRedraw(TRUE);

    m_Playlist = pList;

    if (update)
        Refresh();
}

void PlaylistView::SaveColumns()
{
    musik::Core::IntArray last_order;
    musik::Core::IntArray last_sizes;

    wchar_t sCol[256];

    LVCOLUMN pColumn;
    pColumn.mask = LVCF_TEXT | LVCF_WIDTH;
    pColumn.pszText = sCol;
    pColumn.cchTextMax = sizeof(sCol);

    int nColCount = GetHeaderCtrl()->GetItemCount();
    LPINT lpiCols = new int[nColCount];

    // get header order array
    GetHeaderCtrl()->GetOrderArray(lpiCols, nColCount);

    for (int i = 0; i < nColCount; i++)
    {
        GetColumn(lpiCols[i], &pColumn);
        last_order.push_back(Globals::Library->GetSongFieldID(pColumn.pszText));
        last_sizes.push_back(pColumn.cx);
    } 
    delete[] lpiCols;

    Globals::Preferences->SetPlaylistOrder(last_order);
    Globals::Preferences->SetPlaylistSizes(last_sizes);
}

void PlaylistView::UpdateItemCount()
{
    m_SongInfoCache->Set(0, 0, true);
    SetItemCountEx(m_Playlist->GetCount(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
}

void PlaylistView::Refresh()
{
    if (m_Playlist || m_SongInfoArray)
    {
        SetItemState(-1, 0, LVIS_SELECTED);

        int nPos = GetScrollPos(SB_VERT);

        SetRedraw(false);
        UpdateItemCount();

        SetScrollPos(SB_VERT, nPos, false);

        CRect rcClient;
        GetClientRect(&rcClient);
        SetRedraw(true);
        RedrawWindow(rcClient);
    }
}

int PlaylistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (ListView::OnCreate(lpCreateStruct) == -1)
        return -1;

    if (m_DropTarget)
    {
        m_DropTarget->m_pList = this;
        m_DropTarget->Register(this);
    }

    ResetColumns(false);

    return 0;
}

void PlaylistView::OnDestroy()
{
    if (m_ViewType == PLAYLISTCTRL_NORMAL)
        SaveColumns();

    ListView::OnDestroy();
}

musik::Core::String PlaylistView::GetItemAt(int nItem, int sub_type)
{
    return m_SongInfoCache->GetValue(nItem - m_SongInfoCache->GetFirst(), sub_type);
}

void PlaylistView::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

    LV_ITEM* pItem = &(pDispInfo->item);

    // only need to worry about item text
    if (pItem->mask & LVIF_TEXT)
    {
        int nItem = pItem->iItem;
        int nSub  = pItem->iSubItem;
        int nType = Globals::Preferences->GetPlaylistCol(nSub);

        pItem->mask |= LVIF_DI_SETITEM;

        int sub_type = m_Cols.at(nSub);
        musik::Core::String sValue = GetItemAt(nItem, sub_type);  

        switch(sub_type)
        {
        case musik::Core::MUSIK_LIBRARY_TYPE_RATING:
            {
                int nRating = musik::Core::StringToInt(sValue);
                sValue = GetRating(nRating);
            }
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_DURATION:
            {
                sValue = GetTimeStr(nItem);
            }
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM:
        case musik::Core::MUSIK_LIBRARY_TYPE_YEAR:
            {
                if (sValue == _T("0"))
                    sValue = _T("");
            }
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_TIMESPLAYED:
            {
                if (sValue == _T("0"))
                    sValue = _T("never");
            }
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_LASTPLAYED:
            {
                if (sValue[0] == '-')
                    sValue = _T("");
            }
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_FILESIZE:
            {
                float fval = (float) musik::Core::StringToInt(sValue);

                fval /= (1024.0 * 1024.0);
                sValue.Format(_T("%.2f MB"), fval);
            }
            break;

#if 0
        case musik::Core::MUSIK_LIBRARY_TYPE_FILENAME:
            {
                wchar_t realfn[512];
                if (GetLongPathName(sValue, realfn, 512) <= 0)
                {
                    sValue = _T("File not found");
                }
                else
                {
                    sValue = realfn;
                }
            }
#endif

        default:
            break;

        }

        // copy the buffer of the correct display string
        // to the current LV_ITEM 
        if (sValue.GetLength() > pItem->cchTextMax)
        {
            sValue = sValue.Left(pItem->cchTextMax - 1);
        }
        wcscpy(pItem->pszText, sValue.c_str());
    }

    *pResult = 0;
}

BOOL PlaylistView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

int PlaylistView::GetSongID(int at)
{
    return m_Playlist->GetSongID(at);
}

void PlaylistView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
    NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

    // Take the default processing unless we set this to something else below.
    *pResult = CDRF_DODEFAULT;

    COLORREF clrStripe = GetModColor(Globals::Preferences->GetListCtrlColor(), 10);
    
    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.
    if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;
        return;
    }
    
    // we got a paint item event, ignore it, we want to draw the 
    // sub items one by one.
    else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT )
    {
        *pResult = CDRF_NOTIFYSUBITEMDRAW;
        return;
    }    

    // draw the sub items
    else if (pLVCD->nmcd.dwDrawStage == (CDDS_ITEMPREPAINT | CDDS_SUBITEM))
    {
        if ( 
               (
                 (m_Playlist && pLVCD->nmcd.dwItemSpec < m_Playlist->GetCount()) || 
                 (m_SongInfoArray && pLVCD->nmcd.dwItemSpec < m_SongInfoArray->size()) 
              ) && !m_Arranging 
            
          )
        {
            int songid = GetSongID(pLVCD->nmcd.dwItemSpec);
            int realsongid = Globals::Player->GetPlaying()->GetID();
            CDC *pDC = CDC::FromHandle(pLVCD->nmcd.hdc);
            
            bool is_playing = false;
        
            // color for currently playing song if the current
            // playlist is NOT now playing...
            if (Globals::Player->IsPlaying())
            {
                // if we are looking at the now playing playlist
                // we may have duplicate songs... so go ahead and only
                // highlight the ones thats playing
                if (m_Playlist == Globals::Player->GetPlaylist())
                {
                    if (pLVCD->nmcd.dwItemSpec == Globals::Player->GetIndex())
                    {
                        is_playing = true;
                    }
                }
                else if 
                    (
                        (m_ViewType == PLAYLISTCTRL_NORMAL && songid == Globals::Player->GetPlaying()->GetID()) || 
                        (m_ViewType == PLAYLISTCTRL_CDRIPPER && Globals::Player->GetIndex() == pLVCD->nmcd.dwItemSpec && realsongid == -1)
                   )
                {
                    if (m_ViewType == PLAYLISTCTRL_NORMAL)
                    {
                        if (Globals::Player->GetPlaying()->GetLibrary() == Globals::Library)
                        {
                            is_playing = true;
                        }
                    }
                    else if (m_ViewType == PLAYLISTCTRL_CDRIPPER)
                    {
                        if (Globals::Player->GetDiscDrive() == dynamic_cast<CDPlaylistView*>(this)->GetDiscDrive())
                        {
                            is_playing = true;
                        }
                    }
                }
            }

            pDC->SelectObject(m_ItemFont);

            // currently playing
            if (is_playing)
            {
                pDC->SelectObject(m_BoldFont);

                pLVCD->clrText = Globals::Preferences->GetHilightTextColor();
                pLVCD->clrTextBk = GraphicsUtility::ShadeColor(Globals::Preferences->GetHilightColor(), 40);
            }
            // drag and drop over
            else if (m_DropOverIndex == pLVCD->nmcd.dwItemSpec)
            {
                pLVCD->clrText = Globals::Preferences->GetListCtrlTextColor();
                pLVCD->clrTextBk = GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 20);
            }
            // selected
            else if (GetItemState(pLVCD->nmcd.dwItemSpec, LVIS_SELECTED))
            {
                pLVCD->clrText = Globals::Preferences->GetHilightTextColor();
                pLVCD->clrTextBk = Globals::Preferences->GetHilightColor();
            }
            // regular item
            else
            {
                if (pLVCD->nmcd.dwItemSpec % 2 != 0)
                {
                    // even color
                    pLVCD->clrTextBk = clrStripe;
                }
                else
                {
                    // odd color
                    pLVCD->clrTextBk = Globals::Preferences->GetListCtrlColor();
                }
                //
                pLVCD->clrText = Globals::Preferences->GetListCtrlTextColor();
            }

            int nSubType = Globals::Preferences->GetPlaylistCol(pLVCD->iSubItem);
            if (nSubType == musik::Core::MUSIK_LIBRARY_TYPE_RATING)
            {
                pDC->SelectObject(m_StarFont);
            }        

            *pResult = CDRF_NEWFONT;
            return;
        }
    }
}

void PlaylistView::InitFonts()
{
    m_ItemFont.CreateStockObject(DEFAULT_GUI_FONT);
    m_StarFont.CreatePointFont(100, _T("Musik"));

    LOGFONT pBoldFont;
    m_ItemFont.GetLogFont(&pBoldFont);
    pBoldFont.lfWeight = FW_BOLD;
    m_BoldFont.CreateFontIndirect(&pBoldFont);  
}

void PlaylistView::OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVCACHEHINT pCacheHint = reinterpret_cast<LPNMLVCACHEHINT>(pNMHDR);

    if (m_ViewType == PLAYLISTCTRL_NORMAL)
        m_SongInfoCache->Set(pCacheHint->iFrom, pCacheHint->iTo, false);

    *pResult = 0;
}

void PlaylistView::OnPaint()
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
    memDC.FillSolidRect(clip, Globals::Preferences->GetListCtrlColor());
       
    DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

CString PlaylistView::GetTimeStr(int item)
{
    int time_ms = musik::Core::StringToInt(
        m_SongInfoCache->GetValue(
            item - m_SongInfoCache->GetFirst(),
            musik::Core::MUSIK_LIBRARY_TYPE_DURATION));

    return (CString)Globals::Player->GetTimeStr(time_ms);
}

CString PlaylistView::GetRating(int nRating)
{
    CString sRating;
    switch (nRating)
    {
    case -1:
        sRating = _T("");
        break;
    case 1:
        sRating = _T("-,,,,");
        break;
    case 2:
        sRating = _T("--,,,");
        break;
    case 3:
        sRating = _T("---,,");
        break;
    case 4:
        sRating = _T("----,");
        break;
    case 5:
        sRating = _T("-----");
        break;
    default:
        sRating = _T(",,,,,");
        break;
    }

    return sRating;
}

void PlaylistView::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
    // get cursor pos at time of click in relation
    // to the client
    CPoint ptCurr;
    GetCursorPos(&ptCurr);
    ScreenToClient(&ptCurr);

    // see which item was clicked
    LVHITTESTINFO hit_test;
    hit_test.pt.y = ptCurr.y;
    hit_test.pt.x = 0;
    SubItemHitTest(&hit_test);
    int nItem = hit_test.iItem;

    if (nItem > -1)
    {
        int rating_pos = -1;
        for (size_t i = 0; i < Globals::Preferences->GetPlaylistColCount(); i++)
        {
            if (Globals::Preferences->GetPlaylistCol(i) == musik::Core::MUSIK_LIBRARY_TYPE_RATING)
                rating_pos = (int)i;
        }

        if (rating_pos == -1)
            return;

        CRect rcRating;
        GetSubItemRect(nItem, rating_pos, LVIR_LABEL, rcRating);

        if (rcRating.PtInRect(ptCurr))
        {
            // if an item is a sub item, there are two
            // spaces in front of the item text -- so
            // shift point that far in the X axis
            int nOffset;
            if (rating_pos > 0)
                nOffset = m_TwoSpace;
            else
                nOffset = 2;

            // width of the column and the
            // recalculated click locations...
            int col_width = GetColumnWidth(rating_pos);
            int clk_loc = (ptCurr.x - rcRating.left);

            int nRating;
            if (clk_loc <= nOffset)
                nRating = 0;
            else if (clk_loc >= m_RatingExtent + nOffset)
            {
                if (clk_loc < m_RatingExtent + nOffset + 12)
                {
                    *pResult = 0;
                    return;
                }
                    nRating = 5;
            }
                
            else 
            {
                nRating = (clk_loc / (m_RatingExtent / 5));

                if (rating_pos == 0)
                    nRating++;
            }

            musik::Core::Song& song = m_Playlist->at(hit_test.iItem);
            song.GetLibrary()->SetSongRating(m_Playlist->GetSongID(hit_test.iItem), nRating);    
            ResyncItem(m_Playlist->GetSongID(hit_test.iItem), hit_test.iItem);

            if (Globals::Player->GetPlaying())
                Globals::Player->GetPlaying()->SetRating(ItoS(nRating));

        }
    }

    *pResult = 0;
}

void PlaylistView::ResyncItem(size_t songid, int item)
{
    if (m_SongInfoCache->ResyncItem(songid))
    {
        if (item != -1)
        {
            RedrawItems(item, item);
        }
        else
        {
            RedrawWindow();
        }
    }
}

void PlaylistView::SavePlaylist()
{
    // if the user wants to be prompted...
    if (m_PlaylistNeedsSave && m_Playlist->GetType() == musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD)
    {
        if (m_Playlist->GetID() == -1)
        {
            return;
        }

        Globals::Library->RewriteStdPlaylist(*m_Playlist);
        m_PlaylistNeedsSave = false;
    }
}

void PlaylistView::SetPlaylist(musik::Core::Playlist* playlist)
{
    HideSortArrow();

    if (playlist == NULL)
    {
        return;
    }

    SetItemState(-1, 0, LVIS_SELECTED);
    m_ViewType = PLAYLISTCTRL_NORMAL;

    if ((this->m_Playlist) && (playlist->GetType() == -1))
    {
        playlist->SetPlaylistInfo(this->m_Playlist->GetPlaylistInfo());
    }

    // delete any old playlist that is not a now playing type
    if ((m_Playlist) && (m_Playlist != Globals::Player->GetPlaylist()))
    {
        delete m_Playlist;
    }

    // set the playlist and cache it
    m_Playlist = playlist;

    if (m_SongInfoCache)
        m_SongInfoCache->SetPlaylist(playlist);

    if (m_InfoCtrl)
        m_InfoCtrl->UpdateInfo();    

    if (m_TagEdit)
        m_TagEdit->UpdatePlaylist(m_Playlist);

    // we want all the plugins to see the active playlist
    for(size_t i = 0; i < Globals::Plugins.size(); i++)
        Globals::Plugins.at(i).SetVisiblePlaylist(m_Playlist);
}

void PlaylistView::SetInfoCtrl(PlaylistInfoView *ctrl)
{
    m_InfoCtrl = ctrl;
}

bool PlaylistView::PlayItem(int n)
{
    if (Globals::Preferences->AddEntireToNP())
        return PlayItem_ReplaceNP(n);

    return PlayItem_AddNP(n);
}

bool PlaylistView::PlayItem_ReplaceNP(int n)
{
    if (!m_Playlist || m_Playlist->size() < 1)
        return false;

    if (n == -1)
    {
        POSITION pos = GetFirstSelectedItemPosition();
        n = GetNextSelectedItem (pos);

        if (n == -1)
            n = 0;
    }    

    if (Globals::Player->GetPlaylist() != m_Playlist)
    {
        Globals::Player->SetPlaylist(m_Playlist);
    }

    Globals::Player->Play(n, 0, musik::Core::MUSIK_PLAYER_SONG_USER_ADVANCE, true);

    SetItemState(-1, 0, LVIS_SELECTED);

    int WM_SOURCESREFRESH = RegisterWindowMessage(_T("SOURCESREFRESH"));
    AfxGetApp()->GetMainWnd()->SendMessage(WM_SOURCESREFRESH, NULL);

    return true;
}

bool PlaylistView::PlayItem_AddNP(int n)
{
    if (!m_Playlist)
        return false;

    // current view IS now playing, just play that song
    if (m_Playlist == Globals::Player->GetPlaylist())
    {
        return Globals::Player->Play(n);
    }

    if (n == -1)
    {
        POSITION pos = GetFirstSelectedItemPosition();
        n = GetNextSelectedItem (pos);

        // no selection, so just start playing the first item
        // in the list. if no first item exists, return false.
        if (n == -1 && m_Playlist->size())
            n = 0;
        else
            return false;
    }    

    int insert_at = -1;

    // get activated item's songid
    musik::Core::Song song = m_Playlist->GetSong(n);

    // add it and play it
    Globals::Player->GetPlaylist()->Add(song);

    if (!Globals::Player->IsPlaying())
        Globals::Player->Play(
            Globals::Player->GetPlaylist()->size() - 1);

    SetItemState(-1, 0, LVIS_SELECTED);

    return true;
}

void PlaylistView::OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    *pResult = NULL;

    int item = pNMIA->iItem;

    PlayItem(item);
}

void PlaylistView::BeginDrag(NMHDR* pNMHDR, bool right_button)
{
    // set cursor back to an arrow
    SetCursor(LoadCursor(NULL, IDC_ARROW));

    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    COleDataSource datasrc;
    HGLOBAL        hgDrop;
    DROPFILES*     pDrop;
    CStringList    lsDraggedFiles;
    POSITION       pos;
    int            nSelItem;
    CString        sFile;
    UINT           uBuffSize = 0;
    TCHAR*         pszBuff;
    FORMATETC      etc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

    // For every selected item in the list, put the filename into lsDraggedFiles.
    pos = GetFirstSelectedItemPosition();
    while (pos)
    {
        nSelItem = GetNextSelectedItem (pos);
        sFile = m_Playlist->GetField(nSelItem, musik::Core::MUSIK_LIBRARY_TYPE_FILENAME);
        
        lsDraggedFiles.AddTail (sFile);

        // Calculate the # of chars required to hold this string.
        uBuffSize += lstrlen (sFile) + 1;
    }

    if (!lsDraggedFiles.GetCount())
        return;

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
        wcscpy(pszBuff, (LPCTSTR) lsDraggedFiles.GetNext (pos));
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

    // Put the data in the data coosource.
    if (right_button)
        etc.cfFormat = m_DropID_R;
    else 
        etc.cfFormat = m_DropID_L;

    datasrc.CacheGlobalData (right_button ? m_DropID_R : m_DropID_L, hgBool, &etc);

    // post a message to the main frame, letting
    // it know that drag and drop has started
    int WM_DRAGSTART = RegisterWindowMessage(_T("DRAGSTART"));
    m_Parent->SendMessage(WM_DRAGSTART, NULL);

    // Start the drag 'n' drop!
    DROPEFFECT dwEffect = datasrc.DoDragDrop (DROPEFFECT_COPY);

    int WM_DRAGEND = RegisterWindowMessage(_T("DRAGEND" ));
    m_Parent->SendMessage(WM_DRAGEND, NULL);

    // If the DnD completed OK, we remove all of the dragged items from our
    // list.
    switch (dwEffect)
    {
        case DROPEFFECT_COPY:
        case DROPEFFECT_MOVE:
            break;

        case DROPEFFECT_NONE:
        {
            // the copy completed successfully
            // on a windows nt machine.
            // do whatever we need to do here

            bool bDeletedAnything = false;
            for (size_t i = GetNextItem (-1, LVNI_SELECTED);    i != -1; i = GetNextItem (i, LVNI_SELECTED))
            {
                CString sFilename = GetItemText (nSelItem, 0);

                if (GetFileAttributes (sFile) == 0xFFFFFFFF  &&
                        GetLastError() == ERROR_FILE_NOT_FOUND)
                {
                    // We couldn't read the file's attributes, and GetLastError()
                    // says the file doesn't exist, so remove the corresponding 
                    // item from the list.
                    DeleteItem (nSelItem);
                
                    nSelItem--;
                    bDeletedAnything = true;
                }
            }

            if (! bDeletedAnything)
            {
                // The DnD operation wasn't accepted, or was canceled, so we 
                // should call GlobalFree() to clean up.
                GlobalFree (hgDrop);
                GlobalFree (hgBool);
            }
        }

        break;
    }
}

void PlaylistView::OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (m_ViewType == PLAYLISTCTRL_NORMAL)
        BeginDrag(pNMHDR, false);

    *pResult = 0;
}

void PlaylistView::OnDragLeave()
{
    m_DropOverIndex = -1;
    this->Invalidate();
}

DROPEFFECT PlaylistView::OnDragOver(CPoint point)
{    
    if (m_DropArrange)
    {
        int intDropIndex = this->HitTest(point);

        if ((intDropIndex > -1) && (intDropIndex != m_DropOverIndex)) 
        {
            m_DropOverIndex = intDropIndex;
            this->Invalidate();
        }

        return DROPEFFECT_MOVE;
    }

    return DROPEFFECT_COPY;
}

void PlaylistView::OnDropFiles(HDROP hDropInfo)
{
    m_DropOverIndex = -1;

    // if the drag originated from ourself, then
    // we want to rearrange items, and nothing more.
    if (m_ViewType != PLAYLISTCTRL_NORMAL)
    {
        return;
    }

    if (m_DropArrange)
    {
        CPoint ptCursor;
        ::GetCursorPos(&ptCursor);
        ScreenToClient(&ptCursor);
        
        int nPos = HitTest(ptCursor);

        // get first selected item.. if we can't
        // find one, return.
        int nFirstSel = GetFirstSelected();
        if (nFirstSel == -1)
            return;

        // item dragged onto itself
        if (nPos == nFirstSel)
            return;

        // get selected
        musik::Core::IntArray sel;
        GetSelectedItems(sel);
        
        musik::Core::Playlist sel_songs;
        GetSelectedSongs(sel_songs);

        // remove selected items from their
        // current position
        DeleteItems(sel, false);
        sel.clear();

        // get the id of the currently playing
        // song, we'll find it once the drag is done
        // to assure the correct song is bold.
        int songid = Globals::Player->GetPlaying()->GetID();

        // insert the items back to their new
        // position...
        InsertItems(sel_songs, nFirstSel, nPos);
        sel_songs.Clear();

        // grab the new index..
        Globals::Player->FindNewIndex(songid);

        if (m_Playlist->GetType() == musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD)
            m_PlaylistNeedsSave = true;

        m_DropArrange = false;
        RedrawWindow();
        return;
    }

    // see if we need to prompt the user for
    // which playlist to insert files to...
    int nRet = Globals::Preferences->GetFileDropPrompt();

    if (nRet == -1)
    {
        FileDropDialog* pDlg = new FileDropDialog(this);
        nRet = pDlg->DoModal();
        delete pDlg;

        if (nRet == MUSIK_FILEDROP_CANCEL)
        {
            return;
        }
    }

    // if we get here, the drag originated from 
    // somewhere else, such as explorer...
    size_t nNumFiles;
    TCHAR szNextFile [MAX_PATH];
    SHFILEINFO  rFileInfo;
    
    nNumFiles = DragQueryFile (hDropInfo, -1, NULL, 0);
    musik::Core::StringArray* files = NULL;

    files = new musik::Core::StringArray();

    bool is_dir = false;

    musik::Core::String sTemp;
    for (size_t i = 0; i < nNumFiles; i++)
    {
        if (DragQueryFile(hDropInfo, i, szNextFile, MAX_PATH) > 0)
        {
            // get the filetype. if its a directory
            // that was dropped, we'll want to 
            // recurse it and add all the supported 
            // media files...
            SHGetFileInfo(szNextFile, 0, &rFileInfo, sizeof(rFileInfo), SHGFI_ATTRIBUTES);
            if (rFileInfo.dwAttributes & SFGAO_FOLDER)
            {
                sTemp += szNextFile;
                sTemp += _T("\\*.*||");
                is_dir = true;
            }        

            // otherwise it was just a file... add it...
            else
                files->push_back(szNextFile);
        }
    }

    bool join_thread = false;
    if (is_dir || files->size() > 0)
    {
        musik::Core::BatchAdd* params;

        if (nRet == MUSIK_FILEDROP_ADDNOWPLAYING)
        {
            params = new musik::Core::BatchAdd(
                files, 
                is_dir ? sTemp : "",
                Globals::Player->GetPlaylist(), 
                Globals::Library, 
                Globals::Player, 
                NULL, 
                0, 
                1, 
                1 
           );

            join_thread = true;
        }

        else if (nRet == MUSIK_FILEDROP_ADDPLAYLIST)
        {
            params = new musik::Core::BatchAdd(
                files, 
                is_dir ? sTemp : _T(""),
                m_Playlist, 
                Globals::Library, 
                NULL, 
                NULL, 
                1, 
                0, 
                1 
           );

            m_PlaylistNeedsSave = true;
            join_thread = true;
        }

        else if (nRet == MUSIK_FILEDROP_ADDLIBRARY)
        {
            params = new musik::Core::BatchAdd(
                files, 
                is_dir ? sTemp : _T(""),
                Globals::Player->GetPlaylist(), 
                Globals::Library, 
                NULL, 
                NULL,
                1,
                0,
                1 
           );

            join_thread = false;
        }

        params->m_Functor = Globals::Functor;
        musik::Core::BatchAddTask* task = new musik::Core::BatchAddTask;
        task->m_Params = params;

        if (join_thread)
        {
            task->Start();
        }
        else
        {
            Globals::Library->QueueTask(task);
        }
    }
}

void PlaylistView::GetSelectedItems(musik::Core::IntArray& items)
{
    items.clear();

    POSITION pos = GetFirstSelectedItemPosition();
    while (pos)
        items.push_back(GetNextSelectedItem (pos));
}

void PlaylistView::GetSelectedSongs(musik::Core::Playlist& items)
{
    items.Clear();
    
    POSITION pos = GetFirstSelectedItemPosition();
    while (pos)
        items.Add(m_Playlist->GetSong(GetNextSelectedItem(pos)));
}

void PlaylistView::GetSelectedSongs(musik::Core::SongInfoArray& items)
{
    items.clear();

    musik::Core::Song curr_song;
    musik::Core::SongInfo curr_info;
    
    POSITION pos = GetFirstSelectedItemPosition();
    Globals::Library->BeginTransaction();
    while (pos)
    {
        curr_song = m_Playlist->GetSong(GetNextSelectedItem(pos));
        Globals::Library->GetSongInfoFromID(curr_song.GetID(), curr_info);
        items.push_back(curr_info);
    }
    Globals::Library->EndTransaction();
}

void PlaylistView::GetSelectedSongs(musik::Core::SongArray& items)
{
    items.clear();

    musik::Core::Song curr_song;

    POSITION pos = GetFirstSelectedItemPosition();
    Globals::Library->BeginTransaction();
    while (pos)
    {
        curr_song = m_Playlist->GetSong(GetNextSelectedItem(pos));
        items.push_back(curr_song);
    }
    Globals::Library->EndTransaction();
}

int PlaylistView::GetFirstSelected()
{
    POSITION pos = GetFirstSelectedItemPosition();
    int nRet = GetNextSelectedItem (pos);
    
    return nRet;
}

void PlaylistView::GetItemIDs(const musik::Core::IntArray& items, musik::Core::IntArray* target)
{
    target->clear();

    for (size_t i = 0; i < items.size(); i++)
        target->push_back(m_Playlist->GetSongID(items.at(i)));
}

void PlaylistView::DeleteItems(const musik::Core::IntArray& items, bool update)
{
    if (items.size() <= 0)
        return;

    int nScrollPos = GetScrollPos(SB_VERT);

    musik::Core::SongArray songs;
    musik::Core::Song song;

    bool from_playlist = m_Playlist->GetType() == musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD ? true : false;
    for (size_t i = 0; i < items.size(); i++)
    {
        if (Globals::Player->GetPlaylist() == m_Playlist)
        {
            if (Globals::Player->m_Index >= (int)items.at(i) - (int)i)
                Globals::Player->m_Index--;
        }

        if (from_playlist)
        {
            song.SetID(m_Playlist->at(items.at(i) - i).GetID());
            songs.push_back(song);
        }

        m_Playlist->DeleteAt(items.at(i) - i);
    }

    if (from_playlist)
    {
        Globals::Library->RemoveFromStdPlaylist(songs, this->m_Playlist->GetID());
    }

    if (update)
    {
        if (from_playlist)
        {
            int WM_SELBOXREQUESTUPDATE    = RegisterWindowMessage(_T("SELBOXREQUESTUPDATE"));
            m_Parent->PostMessage(WM_SELBOXREQUESTUPDATE);
        }

        Refresh();

        // deselect all, then select first...
        SetItemState(-1, 0, LVIS_SELECTED);
        if (GetItemCount() > 0)
        {
            if (items.at(0) > GetItemCount())
                SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);
            else
                SetItemState(items.at(0), LVIS_SELECTED, LVIS_SELECTED);
        }

        SetScrollPos(SB_VERT, nScrollPos);
    }

    if (m_InfoCtrl)
        m_InfoCtrl->UpdateInfo();    
}

void PlaylistView::InsertItems(musik::Core::Playlist& items, int firstsel, int at, bool update)
{
    if (items.GetCount() <= 0)
        return;

    int nScrollPos = GetScrollPos(SB_VERT);

    // see if we need to insert items above
    // or below the target... depends if we
    // are dragging up or down.
    if (firstsel < at)
    {
        at -= (items.GetCount() - 1);
        at--;
        if (at < -1)
            at = -1;
    }

    m_Playlist->InsertBefore(items, at);

    if (update)
    {
        Refresh();

        // deselect all, then select the items that
        // were just rearranged...
        SetItemState(-1, 0, LVIS_SELECTED);
        if (at == -1)
            at = GetItemCount() - items.GetCount();
        for (size_t i = at; i < at + items.GetCount(); i++)
            SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);

        SetScrollPos(SB_VERT, nScrollPos);
    }

    if (m_InfoCtrl)
        m_InfoCtrl->UpdateInfo();    
}

void PlaylistView::OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    // returning non-zero ignores the message, and marquee never appears.
    *pResult = 0;
}

void PlaylistView::OnDragColumn(int source, int dest)
{
    if (source == dest)
        return;

    m_Arranging = true;

    musik::Core::IntArray order = Globals::Preferences->GetPlaylistOrder();
    musik::Core::IntArray sizes = Globals::Preferences->GetPlaylistSizes();

    order.insert(order.begin() + dest, order.at(source));
    sizes.insert(sizes.begin() + dest, sizes.at(source));

    if (source > dest)
        source++;

    order.erase(order.begin() + source);
    sizes.erase(sizes.begin() + source);

    Globals::Preferences->SetPlaylistOrder(order);
    Globals::Preferences->SetPlaylistSizes(sizes);

    m_Arranging = false;
}

void PlaylistView::GetRatingExtent()
{
    int spc = 0;
    CSize szText;

    HDC    hMemDC    = NULL;
    hMemDC = CreateCompatibleDC(NULL);
    if (hMemDC)
    {
        CDC* pDC = CDC::FromHandle(hMemDC);
        if (pDC)
        {
            CFont* pOldFont = pDC->SelectObject(&m_StarFont);

            // list control always adds two spaces after
            // each string in a column, this must be
            // subtracted.
            szText = pDC->GetTextExtent(_T("  "));
            m_TwoSpace = szText.cx;

            // supposed extent of the five stars
            szText = pDC->GetTextExtent(_T(",,,,,"));

            pOldFont = pDC->SelectObject(pOldFont);
        }
    }
    DeleteDC(hMemDC);

    m_RatingExtent = szText.cx;
}

void PlaylistView::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    
    if (pNMLV->iSubItem >= 0 && m_ViewType == PLAYLISTCTRL_NORMAL)
    {
        // draw sort arrow
        int nCurrCol = pNMLV->iSubItem;
        DrawSortArrow(nCurrCol);

        // sort
        int songid = -1;
        if (Globals::Player->IsPlaying())
            songid = Globals::Player->GetPlaying()->GetID();

        int type = Globals::Preferences->GetPlaylistCol(pNMLV->iSubItem);
        Globals::Library->SortPlaylist(m_Playlist, type, m_Ascend ? false : true);

        if (Globals::Player->IsPlaying() && songid != -1)
            Globals::Player->FindNewIndex(songid);

        Refresh();
        
        if (m_Playlist->GetType() == musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD)
            m_PlaylistNeedsSave = true;      
    }

    *pResult = 0;
}

void PlaylistView::DrawSortArrow(int nCurrCol)
{
    if (nCurrCol == m_LastCol)
        m_Ascend = (m_Ascend == true) ? false : true;
    else
        m_Ascend = true;

    // hide old arrow
    HideSortArrow();

    // set new arrow
    HDITEM header_item;
    CHeaderCtrl* header_ctrl = GetHeaderCtrl();

    header_item.mask = HDI_FORMAT | HDI_BITMAP;
    header_ctrl->GetItem(nCurrCol, &header_item);
    header_item.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
    header_item.hbm  = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(m_Ascend ? IDB_UP : IDB_DOWN), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
    header_ctrl->SetItem(nCurrCol, &header_item);

    m_LastCol = nCurrCol;
}

void PlaylistView::HideSortArrow()
{
    HDITEM header_item;
    CHeaderCtrl* header_ctrl = GetHeaderCtrl();

    if (m_LastCol > -1 && m_LastCol < (int)Globals::Preferences->GetPlaylistColCount())
    {
        header_item.mask = HDI_FORMAT | HDI_BITMAP;
        header_ctrl->GetItem(m_LastCol, &header_item);
        header_item.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT);

        if (header_item.hbm != 0) 
        {
            DeleteObject(header_item.hbm);
            header_item.hbm = 0;
        }

        header_ctrl->SetItem(m_LastCol, &header_item);
    }
}

void PlaylistView::OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult)
{
    if (m_ViewType == PLAYLISTCTRL_NORMAL)
        BeginDrag(pNMHDR, true);

    *pResult = 0;
}

void PlaylistView::ScrollToCurr()
{
    if (Globals::Player->IsPlaying())
        EnsureVisible(Globals::Player->GetIndex(), FALSE);
}

void PlaylistView::ShowContextMenu()
{
    CPoint pos;
    ::GetCursorPos(&pos);

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_PLAYLIST_CONTEXT_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // check / enable / disable menu items
    if (GetSelectedCount() == 0)
    {
        popup_menu->EnableMenuItem(ID_PLC_DELETE_FROMPLAYLIST, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLC_DELETE_FROMLIBRARY, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLC_DELETE_FROMCOMPUTER, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_PROPERTIES, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_FILENAME_TAGFROMFILENAME, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_FILENAME_TAGTOFILENAME, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_TAGTOLOWERCASE, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_TAGTOUPPERCASE, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_TAG_AUTOCAPITALIZE, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_TAG_CLEAN, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_QUEUESELECTEDINNOWPLAYING, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_REMOVEWHITESPACE, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_RELOADTAGSFROMFILE, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_REWRITETAGSTOFILE, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_RATE_1, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_RATE_2, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_RATE_3, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_RATE_4, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_RATE_5, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_RATE_UNRATED, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_RATE_N1, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_PLAYSELECTED, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYING, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_REPLACENOWPLAYINGANDPLAY, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_PLAYNEXT, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_SHOWINEXPLORER, MF_DISABLED | MF_GRAYED);
    }    
    if (GetSelectedCount() <= 1)
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_BATCH, MF_DISABLED | MF_GRAYED);

    if (m_Playlist != Globals::Player->GetPlaylist())
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, MF_DISABLED | MF_GRAYED);

    if (!Globals::Player->GetPlaylist()->GetCount())
        popup_menu->EnableMenuItem(ID_PLAYLISTCONTEXTMENU_SHUFFLENOWPLAYING, MF_DISABLED | MF_GRAYED);

    if (m_TagEdit)
        popup_menu->CheckMenuItem(ID_PLAYLISTCONTEXTMENU_PROPERTIES, MF_CHECKED);

    if (m_MaskDlg)
        popup_menu->CheckMenuItem(ID_FILENAME_CONFIGUREMASKS, MF_CHECKED);

    if (GetSelectedCount() == 1)
    {
        int sel_rating = musik::Core::StringToInt(
            m_Playlist->GetField(
                GetFirstSelected(),
                musik::Core::MUSIK_LIBRARY_TYPE_RATING));

        switch(sel_rating)
        {    
        case -1:
            popup_menu->CheckMenuItem(ID_RATE_N1, MF_CHECKED);
            break;
        case 0:
            popup_menu->CheckMenuItem(ID_RATE_UNRATED, MF_CHECKED);
            break;
        case 1:
            popup_menu->CheckMenuItem(ID_RATE_1, MF_CHECKED);
            break;
        case 2:
            popup_menu->CheckMenuItem(ID_RATE_2, MF_CHECKED);
            break;
        case 3:
            popup_menu->CheckMenuItem(ID_RATE_3, MF_CHECKED);
            break;
        case 4:
            popup_menu->CheckMenuItem(ID_RATE_4, MF_CHECKED);
            break;
        case 5:
            popup_menu->CheckMenuItem(ID_RATE_5, MF_CHECKED);
            break;
        }
    }

    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
}

int PlaylistView::GetColumnPos(int field)
{
    for (size_t i = 0; i < Globals::Preferences->GetPlaylistColCount(); i++)
    {
        if (Globals::Preferences->GetPlaylistCol(i) == field)
            return i;
    }

    return -1;
}

void PlaylistView::ShowHeaderMenu()
{
    CPoint pos;
    ::GetCursorPos(&pos);

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_PLAYLIST_COLUMNS_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // check / enable / disable menu items
    musik::Core::IntArray items = Globals::Preferences->GetPlaylistOrder();

    for (size_t i = 0; i < items.size(); i++)
    {
        switch(items.at(i))
        {
        case musik::Core::MUSIK_LIBRARY_TYPE_ARTIST:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_ARTIST, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_ALBUM:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_ALBUM, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_YEAR:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_YEAR, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_GENRE:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_GENRE, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_TITLE:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_TITLE, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_TRACKNUMBER, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_TIMEADDED:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_TIMEADDED, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_LASTPLAYED:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_LASTPLAYED, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_FILESIZE:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_FILESIZE, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_FORMAT:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_FORMAT, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_DURATION:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_DURATION, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_RATING:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_RATING, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_TIMESPLAYED:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_TIMESPLAYED, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_BITRATE:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_BITRATE, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_FILENAME:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_FILENAME, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_COMMENTS:
            popup_menu->CheckMenuItem(ID_PLAYLISTCOLUMNS_COMMENTS, MF_CHECKED);
            break;

        case musik::Core::MUSIK_LIBRARY_TYPE_EQUALIZER:
            popup_menu->CheckMenuItem(ID_EQUALIZER_LOCKCHANNELS, MF_CHECKED);
            break;
        }
    }
        
    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
}

void PlaylistView::AddColumn(int field)
{
    SaveColumns();
    Globals::Preferences->AppendPlaylistColOrder(field);
    Globals::Preferences->AppendPlaylistColSize(50);
    ResetColumns();
}

void PlaylistView::RemoveColumn(int field)
{
    SaveColumns();
    musik::Core::IntArray cols = Globals::Preferences->GetPlaylistOrder();
    int at = -1;
    for (size_t i = 0; i < cols.size(); i++)
    {
        if (cols.at (i) == field)
        {
            at = i;
            break;
        }
    }

    if (at != -1)
    {
        Globals::Preferences->RemovePlaylistColOrder(at);
        Globals::Preferences->RemovePlaylistColSize(at);
        ResetColumns();
    }
}

void PlaylistView::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
    *pResult = 0;
}

void PlaylistView::ToggleColumn(int field)
{
    int pos = GetColumnPos(field);
    if (pos == -1)
        AddColumn(field);
    else
        RemoveColumn(field);
}

void PlaylistView::OnPlaylistcolumnsArtist()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST);
}

void PlaylistView::OnPlaylistcolumnsAlbum()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_ALBUM);
}

void PlaylistView::OnPlaylistcolumnsYear()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_YEAR);
}

void PlaylistView::OnPlaylistcolumnsGenre()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_GENRE);
}

void PlaylistView::OnPlaylistcolumnsTitle()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_TITLE);
}

void PlaylistView::OnPlaylistcolumnsTracknumber()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM);
}

void PlaylistView::OnPlaylistcolumnsTimeadded()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_TIMEADDED);
}

void PlaylistView::OnPlaylistcolumnsLastplayed()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_LASTPLAYED);
}

void PlaylistView::OnPlaylistcolumnsFilesize()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_FILESIZE);
}

void PlaylistView::OnPlaylistcolumnsFormat()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_FORMAT);
}

void PlaylistView::OnPlaylistcolumnsDuration()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_DURATION);
}

void PlaylistView::OnPlaylistcolumnsRating()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_RATING);
}

void PlaylistView::OnPlaylistcolumnsTimesplayed()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_TIMESPLAYED);
}

void PlaylistView::OnPlaylistcolumnsBitrate()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_BITRATE);
}

void PlaylistView::OnPlaylistcolumnsFilename()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_FILENAME);
}

void PlaylistView::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
    KeyPress(pNMHDR, pResult);
}

void PlaylistView::KeyPress(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
    *pResult = 0;

    WORD nChar = pLVKeyDow->wVKey;

    switch (nChar)
    {
    case VK_DELETE:
        {
            bool delete_from_lib = false;
            bool delete_from_comp = false;

            // delete from library
            if (GetKeyState(VK_MENU) < 0)
                delete_from_lib = true;

            // delete from computer
            else if (GetKeyState(VK_CONTROL) < 0)
            {
                delete_from_lib = true;
                delete_from_comp = true;
            }

            // delete from playlist
            DeleteSelectedItems(delete_from_lib, delete_from_comp);
            return;
        }
        break;

    case 'A':
    case 'a':
        if (GetKeyState(VK_CONTROL) < 0)
            SetItemState(-1, LVIS_SELECTED, LVIS_SELECTED);
        break;

    case 'D':
    case 'd':
        if (GetKeyState(VK_CONTROL) < 0)
            SetItemState(-1, 0, LVIS_SELECTED);
        break;

    case 'B':
    case 'b':
        if (GetKeyState(VK_CONTROL) < 0)
            OnPlaylistcontextmenuBatch();
        break;

    case 'L':
    case 'l':
        if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0)
            OnPlaylistcontextmenuTagtolowercase();
        break;

    case 'U':
    case 'u':
        if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0)
            OnPlaylistcontextmenuTagtouppercase();
        break;

    case 'C':
    case 'c':
        if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0)
            OnTagAutocapitalize();
        break;

    case 'W':
    case 'w':
        if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_SHIFT) < 0)
            OnPlaylistcontextmenuRemovewhitespace();
        else if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_MENU) < 0)
            OnPlaylistcontextmenuTagClean();
        break;

    case 'T':
    case 't':
        if (GetKeyState(VK_CONTROL) < 0)
            OnPlaylistcontextmenuProperties();
    }

}

void PlaylistView::DeleteSelectedItems(bool fromLibrary, bool fromComputer)
{
    if (fromLibrary)
    {
        musik::Core::Playlist selectedSongs;
        GetSelectedSongs(selectedSongs);

        if (fromComputer)       // TODO: deleting from selection boxes uses the same code. move it somewhere common.
        {
            CShellFileOp deleteFileOp;
            deleteFileOp.SetOperationFlags(FO_DELETE, this, FOF_ALLOWUNDO);
            //
            musik::Core::String filename;
            for (size_t i = 0; i < selectedSongs.GetCount(); i++)
            {
                Globals::Library->GetFieldFromID(
                    selectedSongs.GetSong(i).GetID(), 
                    musik::Core::MUSIK_LIBRARY_TYPE_FILENAME, 
                    filename);
                //
                if (musik::Core::Filename::FileExists(filename))
                {
                    deleteFileOp.AddSourceFile(filename.c_str());
                }
            }

            BOOL notUsed, apiResult, aborted;
            if (deleteFileOp.GetSourceFileList().GetCount())
            {
                deleteFileOp.Go(&notUsed, &apiResult, &aborted);
                if (aborted || apiResult)
                {
                    return;
                }
            }
        }

        musik::Core::Library::DeleteSongs(selectedSongs);

        int WM_SELBOXREQUESTUPDATE = RegisterWindowMessage(_T("SELBOXREQUESTUPDATE"));
        m_Parent->SendMessage(WM_SELBOXREQUESTUPDATE);
    }

    musik::Core::IntArray selectedIndexes;
    GetSelectedItems(selectedIndexes);
    DeleteItems(selectedIndexes, true);

    if (this->m_Playlist == Globals::Player->GetPlaylist())
    {
        Globals::Player->UpdatePlaylist();
        Globals::Player->FindNewIndex();
    }
}

void PlaylistView::OnPlcDeleteFromplaylist()
{
    DeleteSelectedItems();
}

void PlaylistView::OnPlcDeleteFromlibrary()
{
    DeleteSelectedItems(true, false);
}

void PlaylistView::OnPlcDeleteFromcomputer()
{
    DeleteSelectedItems(true, true);
}

void PlaylistView::OnPlaylistcontextmenuShufflenowplaying()
{
    Globals::Player->Shuffle();

    if (GetPlaylist() == Globals::Player->GetPlaylist())
    {
        Refresh();
        ScrollToCurr();
    }
}

void PlaylistView::OnPlaylistcontextmenuProperties()
{
    int sel = GetSelectedCount();

    if (sel < 2)
    {
        if (m_TagEdit)
            OnTagEditDestroy();
        else
        {
            if (!GetSelectedCount())
                return;

            m_TagEdit = new TagEditorDialog(this, m_Playlist);
            m_TagEdit->Create(IDD_TAG_PROPERTIES, this);
            m_TagEdit->ShowWindow(SW_SHOWNORMAL);

            m_TagEdit->UpdateSel(GetFirstSelected());
        }
    }
    else
        OnPlaylistcontextmenuBatch();
}   

LRESULT PlaylistView::OnTagEditDestroy(WPARAM wParam, LPARAM lParam)
{
    m_TagEdit->DestroyWindow();
    delete m_TagEdit;
    m_TagEdit = NULL;

    return 0L;
}

void PlaylistView::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    if (m_TagEdit && pNMLV->iItem >= 0 && pNMLV->uNewState & LVIS_SELECTED)
        m_TagEdit->UpdateSel(pNMLV->iItem);
    
    *pResult = 0;
}

LRESULT PlaylistView::OnTagNext(WPARAM wParam, LPARAM lParam)
{
    int sel;
    if (wParam >= 0)
        sel = (int)wParam;
    else
        sel = GetFirstSelected();

    if (sel + 1 == GetSongCount())
        sel = 0;
    else
        sel++;

    SetSelectionMark(sel);
    SetItemState(-1, 0, LVIS_SELECTED);
    SetItemState(sel, LVIS_SELECTED, LVIS_SELECTED);
    EnsureVisible(sel, FALSE);

    return 0L;
}

LRESULT PlaylistView::OnTagPrev(WPARAM wParam, LPARAM lParam)
{
    int sel;
    if (wParam)
        sel = (int)wParam;
    else
        sel = GetFirstSelected();

    if (sel - 1 == -1)
        sel = GetSongCount() - 1;
    else
        sel--;

    SetSelectionMark(sel);
    SetItemState(-1, 0, LVIS_SELECTED);
    SetItemState(sel, LVIS_SELECTED, LVIS_SELECTED);
    EnsureVisible(sel, FALSE);

    return 0L;
}

void PlaylistView::RefreshTag(int songid)
{
    ResyncItem(songid);
}

LRESULT PlaylistView::OnTagUpdate(WPARAM wParam, LPARAM lParam)
{
    if (wParam)
        RefreshTag((int)wParam);

    return 0L;
}

void PlaylistView::OnPlaylistcontextmenuBatch()
{
    musik::Core::SongInfoArray* selected = new musik::Core::SongInfoArray();
    GetSelectedSongs(*selected);

    BatchTagDialog batch(this, selected);
    
    if (selected->size() > 1 && batch.DoModal() == IDOK)
    {
        musik::Core::BatchRetag* params;

        params = new musik::Core::BatchRetag(
            Globals::Library, 
            NULL, 
            selected);

        params->m_DeleteUpdatedTags = true;
        params->m_WriteToFile = false;
        params->m_Functor = Globals::Functor;

        musik::Core::BatchRetagTask* task = new musik::Core::BatchRetagTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete selected;
}

void PlaylistView::OnFilenameTagfromfilename()
{
    if (ShowMaskDlg(MUSIK_MASK_FROMFILENAME) != IDOK)
        return;

    musik::Core::String mask = Globals::Preferences->GetTagFromFilename();
    if (mask.IsEmpty())
    {
        MessageBox(
            _T("No Tag to Filename mask has been configured. To read tag information from a filename you must configure a mask."), 
            _T(MUSIK_VERSION_STR),
            MB_ICONWARNING);

        return;
    }

    musik::Core::SongArray* songs = new musik::Core::SongArray();
    GetSelectedSongs(*songs);

    musik::Core::TagFromFn* params = new musik::Core::TagFromFn(
        Globals::Library, 
        songs, 
        Globals::Functor,
        mask,
        Globals::Preferences->GetTagUndrToSpaces());

    musik::Core::TagFromFnTask* task = new musik::Core::TagFromFnTask();
    task->m_Params = params;
    Globals::Library->QueueTask(task);
}

void PlaylistView::OnFilenameTagtofilename()
{
    if (ShowMaskDlg(MUSIK_MASK_TOFILENAME) != IDOK)
        return;

    musik::Core::String mask = Globals::Preferences->GetTagToFilename();

    if (mask.IsEmpty())
    {
        MessageBox(
            _T("No Tag from Filename mask has been configured. To rename your files based on database tag information you must configure a mask."), 
            _T(MUSIK_VERSION_STR), 
            MB_ICONWARNING);

        return;
    }

    musik::Core::SongArray* songs = new musik::Core::SongArray();
    GetSelectedSongs(*songs);

    musik::Core::TagToFn* params = new musik::Core::TagToFn(
        Globals::Library,
        songs, 
        Globals::Functor,
        mask,
        Globals::Preferences->GetTagSpacesToUndr());

    musik::Core::TagToFnTask* task = new musik::Core::TagToFnTask();
    task->m_Params = params;
    Globals::Library->QueueTask(task);    
}

int PlaylistView::ShowMaskDlg(int state)
{
    m_MaskDlg = new FilenameMaskDialog(this, state);
    int ret = m_MaskDlg->DoModal();
    delete m_MaskDlg;
    m_MaskDlg = NULL;

    return ret;
}

void PlaylistView::OnPlaylistcontextmenuTagtolowercase()
{
    musik::Core::SongInfoArray* selected = new musik::Core::SongInfoArray();
    GetSelectedSongs(*selected);

    if (selected->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            selected, 
            musik::Core::MUSIK_CHGCASE_TOLOWER);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete selected;
}

void PlaylistView::OnPlaylistcontextmenuTagtouppercase()
{
    musik::Core::SongInfoArray* selected = new musik::Core::SongInfoArray();
    GetSelectedSongs(*selected);

    if (selected->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            selected, 
            musik::Core::MUSIK_CHGCASE_TOUPPER);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete selected;
}

void PlaylistView::OnPlaylistcontextmenuTagClean()
{
    musik::Core::SongInfoArray* selected = new musik::Core::SongInfoArray();
    GetSelectedSongs(*selected);

    if (selected->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            selected, 
            musik::Core::MUSIK_CHGCASE_CLEAN);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete selected;
}

void PlaylistView::OnTagAutocapitalize()
{
    musik::Core::SongInfoArray* selected = new musik::Core::SongInfoArray();
    GetSelectedSongs(*selected);

    if (selected->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            selected, 
            musik::Core::MUSIK_CHGCASE_CAPITALIZE);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete selected;
}

void PlaylistView::RateSelectedItems(int rating)
{
    musik::Core::SongArray songs;
    GetSelectedSongs(songs);

    Globals::Library->BeginTransaction();
    for (size_t i = 0; i < songs.size(); i++)
        Globals::Library->SetSongRating(songs.at(i).GetID(), rating);
    Globals::Library->EndTransaction();

    if (Globals::Player->GetPlaying())
        Globals::Player->GetPlaying()->SetRating(ItoS(rating));

    Refresh();
}

void PlaylistView::OnRateN1()
{
    RateSelectedItems(-1);
}

void PlaylistView::OnRateUn()
{
    RateSelectedItems(0);
}

void PlaylistView::OnRate1()
{
    RateSelectedItems(1);
}

void PlaylistView::OnRate2()
{
    RateSelectedItems(2);
}

void PlaylistView::OnRate3()
{
    RateSelectedItems(3);
}

void PlaylistView::OnRate4()
{
    RateSelectedItems(4);
}

void PlaylistView::OnRate5()
{
    RateSelectedItems(5);
}

void PlaylistView::OnPlaylistcontextmenuReloadtagsfromfile()
{
    musik::Core::Playlist* pPlaylist = new musik::Core::Playlist();
    GetSelectedSongs(*pPlaylist);

    if (pPlaylist->size())
    {
        musik::Core::ReloadTags* pParams = new musik::Core::ReloadTags(
            Globals::Functor,
            pPlaylist,
            Globals::Library);

        musik::Core::ReloadTagsTask* pTask = new musik::Core::ReloadTagsTask();
        pTask->m_Params = pParams;

        Globals::Library->QueueTask(pTask);
    }
    else
        delete pPlaylist;
}

void PlaylistView::OnPlaylistcolumnsComments()
{
    ToggleColumn(musik::Core::MUSIK_LIBRARY_TYPE_COMMENTS);
}

void PlaylistView::OnPlaylistcontextmenuRemovewhitespace()
{
    musik::Core::SongInfoArray* selected = new musik::Core::SongInfoArray();
    GetSelectedSongs(*selected);

    if (selected->size())
    {
        musik::Core::BatchChgCase* params;

        params = new musik::Core::BatchChgCase(
            Globals::Library, 
            Globals::Functor,
            selected, 
            musik::Core::MUSIK_CHGCASE_REMOVEWHITESPACE);

        params->m_DeleteUpdatedTags = true;

        musik::Core::BatchChgCaseTask* task = new musik::Core::BatchChgCaseTask;
        task->m_Params = params;
        Globals::Library->QueueTask(task);
    }
    else
        delete selected;
}

void PlaylistView::OnPlaylistcontextmenuRewritetagstofile()
{
    musik::Core::Playlist playlist;
    GetSelectedSongs(playlist);

    if (playlist.size())
    {
        musik::Core::SongInfoArray* pSongInfoArray = new musik::Core::SongInfoArray();
        Globals::Library->GetInfoArrayFromPlaylist(playlist, *pSongInfoArray);    

        musik::Core::BatchRetag* params = new musik::Core::BatchRetag(
            Globals::Library,
            Globals::Functor,
            pSongInfoArray);
        params->m_WriteToFile = true;        // set explicitly to avoid accidental writing to file

        musik::Core::BatchRetagTask* pTask = new musik::Core::BatchRetagTask();
        pTask->m_Params = params;

        Globals::Library->QueueTask(pTask);
    }
}

void PlaylistView::OnPlaylistcontextmenuPlayselected()
{
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    GetSelectedSongs(*playlist);
    int selected = Globals::Player->GetIndex();
    Globals::Player->GetPlaylist()->InsertAfter(*playlist, selected);
    Globals::Player->Play(++selected);

    if (GetPlaylist() == Globals::Player->GetPlaylist())
        Refresh();

    delete playlist;
}

void PlaylistView::OnPlaylistcontextmenuPlaynext()
{
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    GetSelectedSongs(*playlist);
    int selected = Globals::Player->GetIndex();
    Globals::Player->GetPlaylist()->InsertAfter(*playlist, selected);

    if (GetPlaylist() == Globals::Player->GetPlaylist())
        Refresh();

    delete playlist;
}

void PlaylistView::OnPlaylistcontextmenuReplacenowplaying()
{
    bool is_np = (GetPlaylist() == Globals::Player->GetPlaylist());

    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    GetSelectedSongs(*playlist);

    if (is_np)
        SetPlaylist(playlist);

    Globals::Player->SetPlaylist(playlist);
    Globals::Player->FindNewIndex();

    if (is_np)
        Refresh();
}

void PlaylistView::OnPlaylistcontextmenuQueueselectedinnowplaying()
{
    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    GetSelectedSongs(*playlist);

    bool is_np = (Globals::Player->GetPlaylist() == GetPlaylist());

    for (size_t i = 0; i < playlist->size(); i++)
        Globals::Player->GetPlaylist()->Add(playlist->at(i));

    if (is_np)
        Refresh();
}

void PlaylistView::OnPlaybackReplacenowplayingandplay()
{
    bool is_np = (GetPlaylist() == Globals::Player->GetPlaylist());

    musik::Core::Playlist* playlist = new musik::Core::Playlist();
    GetSelectedSongs(*playlist);

    if (is_np)
        SetPlaylist(playlist);

    Globals::Player->SetPlaylist(playlist);
    Globals::Player->Play(0);

    if (is_np)
        Refresh();
}

void PlaylistView::OnPlaylistcontextmenuShowinexplorer()
{
    musik::Core::Filename fn(m_Playlist->at(GetFirstSelected()).GetField(musik::Core::MUSIK_LIBRARY_TYPE_FILENAME));
    
    wchar_t windowsDir[MAX_PATH];
    GetEnvironmentVariable(_T("WINDIR"), windowsDir, MAX_PATH);

    musik::Core::String commandLine;
    commandLine.Format(
        _T("/n, /e, /select, %s"),
        fn.GetFullFilename().c_str()
   );
    
    ShellExecute(NULL, _T("open"), _T("explorer"), commandLine.c_str(), NULL, SW_SHOWNORMAL);
}

///////////////////////////////////////////////////
// CDPlaylistView
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDPlaylistView, PlaylistView)

BEGIN_MESSAGE_MAP(CDPlaylistView, PlaylistView)
    ON_COMMAND(ID_CDRIP_EDITALBUMINFO, OnEditAlbumInfo)
    ON_COMMAND(ID_CDRIP_EDITTRACKINFO, OnEditTrackInfo)
END_MESSAGE_MAP()

/*ctor*/ CDPlaylistView::CDPlaylistView(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r)
: PlaylistView(mainwnd, NULL, dropid_l, dropid_r)
{
    m_CanPlay = true;
    m_Drive = 0;
    m_ViewType = PLAYLISTCTRL_CDRIPPER;
}

void CDPlaylistView::Init(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r)
{
    PlaylistView::Init(mainwnd, dropid_l, dropid_r);
    m_ViewType = PLAYLISTCTRL_CDRIPPER;
}

void CDPlaylistView::RefreshTag(int no)
{
    Refresh();
}

CString CDPlaylistView::GetTimeStr(int item)
{
    if (m_SongInfoArray)
    {
        int duration = musik::Core::StringToInt(m_SongInfoArray->at(item).GetDuration());
        return (CString)Globals::Player->GetTimeStr(duration);
    }

    return _T("");
}

void CDPlaylistView::SetSongInfoArray(musik::Core::SongInfoArray* info)
{
    HideSortArrow();

    if (info == NULL)
    {
        return;
    }

    SetItemState(-1, 0, LVIS_SELECTED);

    if (m_SongInfoArray)
        delete m_SongInfoArray;

    // set the playlist and cache it
    m_SongInfoArray = info;
    m_ViewType = PLAYLISTCTRL_CDRIPPER;

    if (m_InfoCtrl)
        m_InfoCtrl->UpdateInfo();    

    if (m_TagEdit)
        m_TagEdit->UpdatePlaylist(m_Playlist);
}

void CDPlaylistView::UpdateItemCount()
{
    SetItemCountEx(m_SongInfoArray->size(), LVSICF_NOINVALIDATEALL | LVSICF_NOSCROLL);
}

musik::Core::String CDPlaylistView::GetItemAt(int nItem, int sub_type)
{
    if (m_SongInfoArray->size() < 0)
        return _T("");

    return m_SongInfoArray->at(nItem).GetField(sub_type);
}

int CDPlaylistView::GetSongID(int at)
{
    return -1;
}

void CDPlaylistView::GetPlaylistCols()
{
    m_Cols.push_back(musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM);
    m_Cols.push_back(musik::Core::MUSIK_LIBRARY_TYPE_TITLE);
    m_Cols.push_back(musik::Core::MUSIK_LIBRARY_TYPE_DURATION);
    m_Sizes.push_back(52);
    m_Sizes.push_back(210);
    m_Sizes.push_back(60);
}

void CDPlaylistView::ShowContextMenu()
{
    CPoint pos;
    ::GetCursorPos(&pos);

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_CDRIP_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // check / enable / disable menu items
    if (!GetItemCount())
    {
        popup_menu->EnableMenuItem(ID_CDRIP_EDITALBUMINFO, MF_DISABLED | MF_GRAYED);
        popup_menu->EnableMenuItem(ID_CDRIP_EDITTRACKINFO, MF_DISABLED | MF_GRAYED);
    }    
    else
    {
        if (m_TagEdit)
            popup_menu->EnableMenuItem(ID_CDRIP_EDITTRACKINFO, MF_DISABLED | MF_GRAYED);
    }

    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
}

void CDPlaylistView::OnEditAlbumInfo()
{
    BatchTagDialog batch(this, m_SongInfoArray);
    batch.DoModal();

    Refresh();
}

void CDPlaylistView::OnEditTrackInfo()
{
    if (!m_TagEdit)
    {
        m_TagEdit = new TagEditorDialog(this, m_SongInfoArray);
        m_TagEdit->Create(IDD_TAG_PROPERTIES, this);
        m_TagEdit->ShowWindow(SW_SHOWNORMAL);

        int sel = GetFirstSelected();
        if (sel == -1) sel = 0;
        m_TagEdit->UpdateSel(sel);
    }
}

bool CDPlaylistView::PlayItem(int n)
{
    if (m_CanPlay)
    {
        Globals::Player->SetCDInfo(*m_SongInfoArray, GetDiscDrive());
        Globals::Player->PlayCD(n, 0);  
        SetItemState(-1, 0, LVIS_SELECTED);
        RedrawWindow();
        int WM_SOURCESREFRESH = RegisterWindowMessage(_T("SOURCESREFRESH"));
        AfxGetApp()->GetMainWnd()->SendMessage(WM_SOURCESREFRESH, NULL);
        return true;
    }

    return false;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube
