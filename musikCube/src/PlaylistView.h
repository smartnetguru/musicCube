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
#include "TagEditorDialog.h"
#include "FilenameMaskDialog.h"
#include "ListView.h"
#include "TrackMetadataCache.h"

#include <musikCore.h>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

enum
{
    PLAYLISTCTRL_CDRIPPER = 1,
    PLAYLISTCTRL_NORMAL = 0
};

///////////////////////////////////////////////////
// PlaylistView
///////////////////////////////////////////////////

class PlaylistViewDropTarget;
class PlaylistInfoView;

class PlaylistView : public musik::Cube::ListView
{
    friend class PlaylistViewDropTarget;

public:

    // construct and destruct
    PlaylistView();
    PlaylistView(CFrameWnd* mainwnd, PlaylistViewDropTarget* pDropTarget, UINT dropid_l, UINT dropid_r);
    virtual ~PlaylistView();

    // update the virtual list control
    void Refresh();
    void ScrollToCurr();
    void ResyncItem(size_t songid, int item = -1);

    // querying items
    void GetSelectedItems(musik::Core::IntArray& items);
    void GetSelectedSongs(musik::Core::Playlist& songs);
    void GetSelectedSongs(musik::Core::SongInfoArray& songs);
    void GetSelectedSongs(musik::Core::SongArray& songs);
    int GetFirstSelected();

    // stuff dealing with the playlist header
    int GetColumnPos(int field);
    void AddColumn(int field);
    void RemoveColumn(int field);
    void ToggleColumn (int field);

    // call when new playlist is set becuase the
    // previous playlist's sorting should not apply
    // to the current one's...
    void HideSortArrow();
    
    // returns TRUE if user has deleted, inserted,
    // or otherwise modified a playlist that may
    // need to be saved...
    bool PlaylistNeedsSave(){ return m_PlaylistNeedsSave; }
    musik::Core::Playlist* GetPlaylist(){ return m_Playlist; }
    void SetPlaylist(musik::Core::Playlist* playlist);
    void SavePlaylist();

    // used by the main UI to trigger an "item activate"
    // event..
    virtual bool PlayItem(int n = -1);

    void SetInfoCtrl(PlaylistInfoView *ctrl);
    int GetType(){ return m_ViewType; }

    virtual void ShowContextMenu();
    virtual void ShowHeaderMenu();
    musik::Core::SongInfoArray* m_SongInfoArray;

protected:

    virtual void Init(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r);
    vector<int> m_Cols, m_Sizes;

    // is dnd active
    UINT m_DropID_L;
    UINT m_DropID_R;
    bool m_Dragging;

    // dialogs
    TagEditorDialog* m_TagEdit;
    FilenameMaskDialog* m_MaskDlg;

    // internal play routines
    bool PlayItem_AddNP(int n = -1);
    bool PlayItem_ReplaceNP(int n = -1);

    // pointer to main window
    CFrameWnd* m_Parent;

    // pointer to info ctrl
    PlaylistInfoView* m_InfoCtrl;

    // playlist stuff
    musik::Core::Playlist* m_NowPlaying;
    TrackMetadataCache* m_SongInfoCache;

    musik::Core::Playlist* m_Playlist;
    int m_ViewType;

    // drag and drop functions
    virtual void OnDragColumn(int source, int dest);
    void BeginDrag(NMHDR* pNMHDR, bool right_button);
    bool m_Arranging;

    // fonts
    void InitFonts();
    CFont m_StarFont;
    CFont m_ItemFont;
    CFont m_BoldFont;

    // rating ***** text extent
    void GetRatingExtent();
    int m_RatingExtent;
    int m_TwoSpace;

    // manipulating items..
    bool m_PlaylistNeedsSave;
    void GetItemIDs(const musik::Core::IntArray& items, musik::Core::IntArray* target);
    void DeleteItems(const musik::Core::IntArray& items, bool update = true);
    void InsertItems(musik::Core::Playlist& items, int first_sel, int at, bool update = true);
    void DeleteSelectedItems(bool from_library = false, bool from_computer = false);
    void RateSelectedItems(int rating);

    // mfc message maps
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnOdcachehint(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPaint();
    afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemActivate(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnMarqueeBegin(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPlaylistcolumnsArtist();
    afx_msg void OnLvnBeginrdrag(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPlaylistcolumnsAlbum();
    afx_msg void OnPlaylistcolumnsYear();
    afx_msg void OnPlaylistcolumnsGenre();
    afx_msg void OnPlaylistcolumnsTitle();
    afx_msg void OnPlaylistcolumnsTracknumber();
    afx_msg void OnPlaylistcolumnsTimeadded();
    afx_msg void OnPlaylistcolumnsLastplayed();
    afx_msg void OnPlaylistcolumnsFilesize();
    afx_msg void OnPlaylistcolumnsFormat();
    afx_msg void OnPlaylistcolumnsDuration();
    afx_msg void OnPlaylistcolumnsRating();
    afx_msg void OnPlaylistcolumnsTimesplayed();
    afx_msg void OnPlaylistcolumnsBitrate();
    afx_msg void OnPlaylistcolumnsFilename();
    afx_msg void OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPlcDeleteFromplaylist();
    afx_msg void OnPlcDeleteFromlibrary();
    afx_msg void OnPlcDeleteFromcomputer();
    afx_msg void OnPlaylistcontextmenuShufflenowplaying();
    afx_msg void OnPlaylistcontextmenuProperties();
    afx_msg void OnPlaylistcontextmenuTagClean();
    afx_msg LRESULT OnTagEditDestroy(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnTagNext(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnTagPrev(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg LRESULT OnTagUpdate(WPARAM wParam = NULL, LPARAM lParam = NULL);
    afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnPlaylistcontextmenuBatch();
    afx_msg void OnFilenameTagfromfilename();
    afx_msg void OnFilenameTagtofilename();
    afx_msg void OnPlaylistcontextmenuTagtolowercase();
    afx_msg void OnPlaylistcontextmenuTagtouppercase();
    afx_msg void OnTagAutocapitalize();
    afx_msg void OnRateN1();
    afx_msg void OnRateUn();
    afx_msg void OnRate1();
    afx_msg void OnRate2();
    afx_msg void OnRate3();
    afx_msg void OnRate4();
    afx_msg void OnRate5();
    afx_msg void OnPlaylistcontextmenuReloadtagsfromfile();
    afx_msg void OnPlaylistcolumnsComments();
    afx_msg void OnPlaylistcontextmenuRemovewhitespace();
    afx_msg void OnPlaylistcontextmenuQueueselectedinnowplaying();
    afx_msg void OnPlaylistcontextmenuRewritetagstofile();
    afx_msg void OnPlaylistcontextmenuPlayselected();
    afx_msg void OnPlaylistcontextmenuPlaynext();
    afx_msg void OnPlaylistcontextmenuReplacenowplaying();
    afx_msg void OnPlaylistcontextmenuShowinexplorer();
    afx_msg void OnPlaybackReplacenowplayingandplay();

    // drag and drop
    bool m_DropArrange;
    void OnDropFiles(HDROP hDropInfo);
    DROPEFFECT OnDragOver(CPoint point);
    void OnDragLeave();
    int m_DropOverIndex;

    virtual void UpdateItemCount();
    virtual musik::Core::String GetItemAt(int at, int sub);
    virtual int GetSongID(int at);
    virtual CString GetTimeStr(int item);
    virtual void GetPlaylistCols();
    virtual void KeyPress(NMHDR *pNMHDR, LRESULT *pResult);
    virtual int GetSongCount(){ if (m_Playlist) return m_Playlist->size(); return 0; }
    virtual void RefreshTag(int no);

    // macros
    DECLARE_DYNAMIC(PlaylistView)
    DECLARE_MESSAGE_MAP()

private:

    // misc
    int ShowMaskDlg(int state = MUSIK_MASK_TOFILENAME);

    // playlist column stuff
    void ResetColumns(bool update = true);
    void SaveColumns();
    CString GetRating(int item);

    // dnd stuff
    bool m_Marquee;
    size_t m_ClipboardFormat;

    // current sorted column
    void DrawSortArrow(int col);
    int m_LastCol;
    bool m_Ascend;

    PlaylistViewDropTarget* m_DropTarget;

};

///////////////////////////////////////////////////
// CDPlaylistView
///////////////////////////////////////////////////

class CDPlaylistView : public PlaylistView
{
public:

    CDPlaylistView(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r);

    virtual void Init(CFrameWnd* mainwnd, UINT dropid_l, UINT dropid_r);
    void SetSongInfoArray(musik::Core::SongInfoArray* info);

    afx_msg void OnEditTrackInfo();
    afx_msg void OnEditAlbumInfo();
    void SetDiscDrive(int drive){ m_Drive = drive; }
    int GetDiscDrive(){ return m_Drive; }

    bool m_CanPlay;

protected:

    virtual void UpdateItemCount();
    virtual musik::Core::String GetItemAt(int at, int subitem);
    virtual int GetSongID(int at);
    virtual CString GetTimeStr(int item);
    virtual void GetPlaylistCols();
    virtual void ShowContextMenu();
    virtual void ShowHeaderMenu(){}
    virtual void KeyPress(NMHDR *pNMHDR, LRESULT *pResult){ pResult = 0; }
    virtual int GetSongCount(){ if (m_SongInfoArray) return m_SongInfoArray->size(); return 0; }
    virtual bool PlayItem(int n = -1);
    virtual void RefreshTag(int no);

    int m_Drive;

    // macros
    DECLARE_MESSAGE_MAP()
    DECLARE_DYNAMIC(CDPlaylistView)
};

///////////////////////////////////////////////////

} } // namespace musik::Cube

