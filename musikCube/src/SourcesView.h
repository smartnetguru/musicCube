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

#pragma once

///////////////////////////////////////////////////

#include "ListViewRowEditor.h"
#include "DockableWindow.h"
#include "SourcesViewDropTarget.h"
#include "PropertyTree.h"
#include "DiscRipDialog.h"

#include <musikCore.h>
#include <vector>

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// BEGIN NEW STUFF!!
///////////////////////////////////////////////////
// This stuff doesn't do anything yet, but lays out what we should be
// thinking about in the not too distant future.
///////////////////////////////////////////////////

class SourcesView;      // forward decl

enum SourcesItemType
{
    SourcesItemLibrary,
    SourcesItemCreateUserOrderedPlaylist,
    SourcesItemUserOrderedPlaylist,
    SourcesItemCreateDynamicPlaylist,
    SourcesItemDynamicPlaylist,
    SourcesItemNowPlaying,
    SourcesItemNetRadio,
    SourcesItemCollected,
    SourcesItemCdrom,
    SourcesItemMassStorageDevice,
    SourcesItemMainFramePlugin
};

enum DragDropEffect
{
    DropEffectNone,
    DropEffectCopy,
    DropEffectMove,
    DropEffectLink,
    DropEffectScroll
};

class ISourcesTreeItem
{
public:     virtual bool IsDragSource() = 0;

        // legacy stuff
public:     virtual SourcesItemType Type() = 0;
public:     virtual int DatabaseID() = 0;
public:     virtual const std::wstring& DatabaseName() = 0;
public:     virtual const std::wstring& DatabaseQuery() = 0;
public:     virtual bool CanRename() = 0;
public:     virtual bool RenameTo(const std::wstring& newName) = 0;
public:     virtual bool CanDelete() = 0;
public:     virtual bool Delete() = 0;

        // events
public:     virtual void OnLeftClick() = 0;
public:     virtual void OnRightClick() = 0;
public:     virtual DragDropEffect OnFilesDragOver() = 0;
public:     virtual void OnDropFiles(const musik::Core::StringArray& filenames) = 0;
public:     virtual musik::Core::StringArray OnBeginDrag() = 0;
};

typedef std::vector<ISourcesTreeItem*> SourcesTreeGroup;

#define BEGIN_DECLARE_LEGACY_SOURCES_TREE_ITEM(itemName, itemType)                      \
    class itemName                                                                      \
    {                                                                                   \
    public:     /*ctor*/ itemName(                                                      \
                    SourcesView& sourcesView,                                           \
                    int databaseID,                                                     \
                    const std::wstring& databaseName,                                   \
                    const std::wstring& databaseQuery)                                  \
                : sourcesView(sourcesView)                                              \
                , databaseID(databaseID)                                                \
                , databaseName(databaseName)                                            \
                , databaseQuery(databaseQuery)                                          \
                {                                                                       \
                }                                                                       \
                                                                                        \
    public:     virtual SourcesItemType Type() { return itemType; }                     \
    public:     virtual const int DatabaseID() { return databaseID; }                   \
    public:     virtual const std::wstring& DatabaseName() { return databaseName; }     \
    public:     virtual const std::wstring& DatabaseQuery() { return databaseQuery; }   \
    public:     virtual bool CanRename();                                               \
    public:     virtual bool Rename(const std::wstring& newName);                       \
    public:     virtual bool CanDelete();                                               \
    public:     virtual bool Delete();                                                  \
                                                                                        \
    public:     virtual bool IsDragSource();                                            \
    public:     virtual void OnDropFiles(const musik::Core::StringArray& filenames);    \
    public:     virtual void OnLeftClick();                                             \
    public:     virtual void OnRightClick() ;                                           \
    public:     virtual musik::Core::StringArray OnBeginDrag();                         \
                                                                                        \
    private:    SourcesView& sourcesView;                                               \
    private:    int databaseID;                                                         \
    private:    std::wstring databaseName, databaseQuery;                               \

#define END_DECLARE_LEGACY_SOURCES_TREE_ITEM                                            \
    };                                                                                  \

BEGIN_DECLARE_LEGACY_SOURCES_TREE_ITEM(UserOrderedPlaylistTreeItem, SourcesItemUserOrderedPlaylist)
END_DECLARE_LEGACY_SOURCES_TREE_ITEM

BEGIN_DECLARE_LEGACY_SOURCES_TREE_ITEM(DynamicPlaylistTreeItem, SourcesItemDynamicPlaylist)
END_DECLARE_LEGACY_SOURCES_TREE_ITEM
// ...
// ...
// ...

///////////////////////////////////////////////////
// END NEW STUFF!!
///////////////////////////////////////////////////

///////////////////////////////////////////////////

#define IDC_SOURCES 1000
#define IDC_EDITINPLACE 1538



typedef std::vector<PropertyTreeItem*> PropertyTreeItemArray;

enum
{
    MUSIK_SOURCES_EDITINPLACE_RENAME = 493,
    MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH,
    MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST,
    MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST
};

class SourcesView : public DockableWindow
{
public:     friend class                SourcesViewDropTarget;
public:     friend class                MainFrame;
public:     static void                 LoadBitmaps();


    // ctor, dtor
public:     /*ctor*/                    SourcesView(CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r);
public:     /*dtor*/                    virtual ~SourcesView();


    // DockableWindow overrides
protected:  virtual void                OnOptions(){}
protected:  CWnd*                       GetMainWindow() { return (CWnd*) this->m_PropTree; }


    // Control's Instance Variables
private:    CFrameWnd*                  m_Parent;
private:    PropertyTreeItem*         m_RightClick;
private:    PropertyTree*             m_PropTree;
private:    CFont                       m_DefaultFont;
private:    UINT                        m_DropID;
private:    ListViewRowEditor           m_EditInPlace;


    // Browse
    // (methods)
private:    void                        LoadBrowseItems();
    // (variables)
private:    PropertyTreeItem*         m_BrowseRootItem;
private:    PropertyTreeItemArray   m_BrowseItems;


    // Library
    // (events)
private:    afx_msg void                OnMenuLibraryShowAllSongs();
    // (methods)
private:    bool                        DropAddToLibrary(musik::Core::Library* pLibrary, musik::Core::StringArray* pFiles);
public:     void                        FocusLibrary();


    // Now Playing
    // (events)
private:    afx_msg                     void OnSourcesNowPlayingClear();
private:    afx_msg                     void OnMenuShuffleNowPlaying();
    // (methods)
public:     void                        FocusNowPlaying();


    // Standard Playlists
    // (methods)
private:    void                        CreateNewStdPlaylist();
private:    void                        RemoveStdPlaylist(PropertyTreeItem* pItem);
private:    void                        LoadStdPlaylistItems();
public:     void                        ToggleStdPlaylistsVisible();
    // (variables)
private:    PropertyTreeItem*         m_StdPlaylistRootItem;
private:    PropertyTreeItemArray   m_StdPlaylistItems;


    // Dynamic Playlists
    // (events)
private:    afx_msg void                OnMenuEditQuery();
private:    afx_msg void                OnMenuCreateStockDynamicPlaylists();
    // (methods)
private:    void                        CreateNewDynPlaylist();
private:    void                        RemoveDynPlaylist(PropertyTreeItem* pItem);
private:    void                        LoadDynPlaylistItems();
public:     void                        ToggleDynPlaylistsVisible();
    // (variables)
private:    PropertyTreeItem*         m_DynPlaylistRootItem;
private:    PropertyTreeItemArray   m_DynPlaylistItems;


    // Collected
    // (events)
public:     void                        LoadCollectedLibrary();
public:     void                        TellUserAboutCollected();
    // (variables)
private:    PropertyTreeItem*         m_CollectedItem;


    // Devices
    // (events)
private:    void                        OnTimer(UINT nIDEvent);
private:    afx_msg void                OnMenuQueryFreeDB();
private:    afx_msg void                OnMenuRefreshCD();
private:    afx_msg void                OnMenuDeviceUnloadLibrary();
private:    afx_msg void                OnMenuRipSelectedCD();
private:    afx_msg void                OnMenuSynchronizeDevice();
public:     afx_msg void                OnMenuSynchronizedPaths();
            // (methods)
private:    bool                        DropOnRemovableDevice(musik::Core::StringArray* pFiles, PropertyTreeItem* pItem);
private:    bool                        LoadRemovableLibrary(PropertyTreeItem* ptrItem);
private:    int                         GetDriveCount();
private:    void                        DeviceTimerTick();
public:     void                        RefreshDevices();
public:     static void                 FindAttachedDevices(musik::Core::StringArray& target);
public:     musik::Core::String           GetDeviceVolumeDescription(const musik::Core::String& drive_path);
public:     void                        ToggleDevicesVisible();
    // (variables)
private:    PropertyTreeItem*         m_DevicesRootItem;
private:    PropertyTreeItemArray   m_DeviceItems;
private:    musik::Core::StringArray      m_VisibleDrives;
private:    UINT                        m_DevicesTimerID;


    // Drag and Drop
    // (events)
private:    void                        OnDropFiles(HDROP hDropInfo, bool right_button = false);
private:    DROPEFFECT                  OnDragOver(CPoint point);
private:    void                        OnDragLeave();
private:    void                        DoDrag(PropertyTreeItem* pItem);
    // (methods)
private:    bool                        DropCopyFilesToDevice(musik::Core::StringArray* pFiles, PropertyTreeItem* pItem);
private:    bool                        DropAddNowPlaying(musik::Core::StringArray* pFiles);
private:    bool                        DropAddToDevice(musik::Core::StringArray* pFiles, PropertyTreeItem* pItem);
private:    bool                        DropAddToCollected(musik::Core::StringArray* pFiles);
    // (variables)
private:    musik::Core::PlaylistInfo*    m_DragAndDropInfo;
private:    SourcesViewDropTarget*    m_DropTarget;


    // Item Editing
    //  (events)
private:    afx_msg LRESULT             OnItemEditCommit(WPARAM wParam, LPARAM lParam);
private:    afx_msg LRESULT             OnItemEditCancel(WPARAM wParam, LPARAM lParam);
private:    afx_msg LRESULT             OnItemEditChange(WPARAM wParam, LPARAM lParam);
    //  (methods)
private:    void                        EditItemInPlace(PropertyTreeItem* pItem, int edit_type, const CString text);
public:     void                        RenameItem(PropertyTreeItem* pItem = NULL, int mode = MUSIK_SOURCES_EDITINPLACE_RENAME, CPoint loc = CPoint(-1, -1));
    //  (variables)
private:    PropertyTreeItem*         m_EditInPlaceItem;
private:    musik::Core::String           m_PreEditInPlaceText;

    // Item Manipulation
    //  (events)
private:    afx_msg void                OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult);
private:    afx_msg void                OnItemDragged(NMHDR* pNotifyStruct, LRESULT* plResult);

private:    afx_msg void                OnMenuMoveItemUp();
private:    afx_msg void                OnMenuMoveItemDown();
private:    afx_msg void                OnMenuRenameItem();
private:    afx_msg void                OnMenuDeleteItem();
    //  (methods)
private:    void                        MoveSelectedItemUp();
private:    void                        MoveSelectedItemDown();
public:     void                        DeleteSelectedItem();
public:     void                        FocusItem(PropertyTreeItem* pItem);       // WHY IS THIS USED ONLY ONCE?!
public:     void                        DeselectAllItems(bool redraw = true);
public:     void                        ReloadAllItems();
public:     PropertyTreeItem*         GetFocusedItem() { return this->m_PropTree->GetFocusedItem(); }
private:    void                        NotifyParentItemChanged(PropertyTreeItem* pItem);

    // Searching
    // (methods)
private:    void                        FinishQuickSearch();
public:     void                        FocusQuickSearch();
public:     void                        EnableQuickSearch();
private:    void                        SearchTimerTick();
    // (variables)
private:    CString                     m_QuickStr;
private:    PropertyTreeItem*         m_SearchItem;
private:    UINT                        m_SearchTimerID;

    // Plugins
    // (events)
private:    afx_msg void                OnMenuPluginExecute();
private:    afx_msg void                OnMenuPluginConfigure();
private:    afx_msg void                OnMenuPluginStop();
private:    afx_msg void                OnMenuPluginAbout();
    // (methods)
private:    void                        LoadPluginItems();
public:     void                        TogglePluginsVisible();
    // (variables)
private:    PropertyTreeItemArray   m_PluginItems;
private:    PropertyTreeItem*         m_PluginsRootItem;


    // Keyboard Events
private:    afx_msg void                OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);


    // misc menu events
private:    afx_msg void                OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
private:    afx_msg                     void OnMenuCopyFilesTo();
private:    afx_msg                     void OnMenuMoveFilesTo();


    // window events
private:    afx_msg int                 OnCreate(LPCREATESTRUCT lpCreateStruct);


    // misc
private:    void                        DeleteLastCreatedPlaylist(int type);
private:    musik::Core::String           SourcesFileOp(DWORD mode, const musik::Core::StringArray& files, musik::Core::String default_path = _T(""));
public:     void                        GetSelFilenames(PropertyTreeItem* pItem, musik::Core::String& files);
public:     void                        GetSelFilenames(PropertyTreeItem* pItem, musik::Core::StringArray& files);
private:    void                        ShowDockBarMenu();

    // macros
private:    DECLARE_DYNAMIC(SourcesView)
private:    DECLARE_MESSAGE_MAP()
public:
};

///////////////////////////////////////////////////

} } // namespace musik::Cube