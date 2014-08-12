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
#include "MainFrame.h"
#include "SourcesView.h"
#include "MainFramePlugin.h"
#include "DynamicPlaylistDialog.h"
#include "3rdparty/CShellFileOp.h"

#include <Winbase.h>
#include <basscd.h>
#include <musikCore/Filename.h>

///////////////////////////////////////////////////

#pragma warning (disable : 4312)    // conversion from int to void* of greater size

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

int WM_SOURCES_EDIT_COMMIT = RegisterWindowMessage(_T("MUSIKEDITCOMMIT"));
int WM_SOURCES_EDIT_CANCEL = RegisterWindowMessage(_T("MUSIKEDITCANCEL"));
int WM_SOURCES_EDIT_CHANGE = RegisterWindowMessage(_T("MUSIKEDITCHANGE"));

const unsigned int SOURCES_DEVICE_TIMER_ID = WM_USER + 25;
const unsigned int SOURCES_SEARCH_TIMER_ID = WM_USER + 26;

IMPLEMENT_DYNAMIC(SourcesView, DockableWindow)

BEGIN_MESSAGE_MAP(SourcesView, DockableWindow)
    // mfc message maps
    ON_WM_CREATE()
    ON_WM_SHOWWINDOW()
    ON_WM_KEYDOWN()
    ON_WM_CONTEXTMENU()
    ON_WM_TIMER()
    // custom message maps
    ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_COMMIT, OnItemEditCommit)
    ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CANCEL, OnItemEditCancel)
    ON_REGISTERED_MESSAGE(WM_SOURCES_EDIT_CHANGE, OnItemEditChange)
    // menu
    ON_COMMAND(ID_SOURCES_RENAME, OnMenuRenameItem)
    ON_COMMAND(ID_SOURCES_DELETE, OnMenuDeleteItem)
    ON_COMMAND(ID_SOURCES_SHUFFLEPLAYLIST, OnMenuShuffleNowPlaying)
    ON_COMMAND(ID_SOURCES_COPYTO, OnMenuCopyFilesTo)
    ON_COMMAND(ID_SOURCES_EDITQUERY, OnMenuEditQuery)
    ON_COMMAND(ID_SOURCES_QUERYFREEDB, OnMenuQueryFreeDB)
    ON_COMMAND(ID_SOURCES_RIPCD, OnMenuRipSelectedCD)
    ON_COMMAND(ID_SOURCES_REFRESHCD, OnMenuRefreshCD)
    ON_COMMAND(ID_SOURCES_UNMOUNT, OnMenuDeviceUnloadLibrary)
    ON_COMMAND(ID_SOURCES_CREATESTOCK, OnMenuCreateStockDynamicPlaylists)
    ON_COMMAND(ID_SOURCES_MOVEUP, OnMenuMoveItemUp)
    ON_COMMAND(ID_SOURCES_MOVEDOWN, OnMenuMoveItemDown)
    ON_COMMAND(ID_SOURCES_CLEAR, OnSourcesNowPlayingClear)
    ON_COMMAND(ID_SOURCES_MOVEFILESTO, OnMenuMoveFilesTo)
    ON_COMMAND(ID_SOURCES_PLUGINEXECUTE, OnMenuPluginExecute)
    ON_COMMAND(ID_SOURCES_PLUGINCONFIGURE, OnMenuPluginConfigure)
    ON_COMMAND(ID_SOURCES_PLUGINSTOP, OnMenuPluginStop)
    ON_COMMAND(ID_SOURCES_PLUGINABOUT, OnMenuPluginAbout)
    ON_COMMAND(ID_SOURCES_SHOWALLSONGS, OnMenuLibraryShowAllSongs)
    ON_COMMAND(ID_SOURCES_SYNCHRONIZE, OnMenuSynchronizeDevice)
    // proptree
    ON_NOTIFY(PTN_SELCHANGE, IDC_SOURCES, OnItemChanged)
    ON_NOTIFY(PTN_ITEMDRAG, IDC_SOURCES, OnItemDragged)
    //
    ON_COMMAND(ID_SOURCES_SYNCHRONIZEDPATHS, OnMenuSynchronizedPaths)
END_MESSAGE_MAP()

using namespace Gdiplus;
static Gdiplus::Bitmap* m_pLibOff = NULL;
static Gdiplus::Bitmap* m_pLibOn = NULL;
static Gdiplus::Bitmap* m_pNPOff = NULL;
static Gdiplus::Bitmap* m_pNPOn = NULL;
static Gdiplus::Bitmap* m_pQSOff = NULL;
static Gdiplus::Bitmap* m_pQSOn = NULL;
static Gdiplus::Bitmap* m_pCDOff = NULL;
static Gdiplus::Bitmap* m_pCDOn = NULL;
static Gdiplus::Bitmap* m_pPLOff = NULL;
static Gdiplus::Bitmap* m_pPLOn = NULL;
static Gdiplus::Bitmap* m_pDPOff = NULL;
static Gdiplus::Bitmap* m_pDPOn = NULL;
static Gdiplus::Bitmap* m_pRDOff = NULL;
static Gdiplus::Bitmap* m_pRDOn = NULL;
static Gdiplus::Bitmap* m_pNSOff = NULL;
static Gdiplus::Bitmap* m_pNSOn = NULL;
static Gdiplus::Bitmap* m_pPlugOff = NULL;
static Gdiplus::Bitmap* m_pPlugOn = NULL;
static Gdiplus::Bitmap* m_pColOff = NULL;
static Gdiplus::Bitmap* m_pColOn = NULL;

/*ctor*/ SourcesView::SourcesView(CFrameWnd* parent, UINT dropid, UINT pldropid_r, UINT sbdropid_r)
    : DockableWindow()
{
    m_DropTarget                = new SourcesViewDropTarget(this, dropid, pldropid_r, sbdropid_r);

    m_Parent                    = parent;
    this->m_DropID              = dropid;
    m_BrowseRootItem            = NULL;
    m_StdPlaylistRootItem       = NULL;
    m_DynPlaylistRootItem       = NULL;
    m_DevicesRootItem           = NULL;
    m_PluginsRootItem           = NULL;
    m_CollectedItem             = NULL;
    m_DevicesTimerID            = NULL;
    m_SearchTimerID             = NULL;
    m_DragAndDropInfo           = NULL;

    m_PropTree = new PropertyTree(dropid);
}

/*dtor*/ SourcesView::~SourcesView()
{
    m_DropTarget->Revoke();
    delete m_DropTarget;

    delete m_PropTree;
}

void SourcesView::RefreshDevices()
{
    if (this->m_DevicesRootItem != NULL)
    {
        musik::Core::PlaylistInfo info;

        musik::Core::StringArray hidden = Globals::Preferences->GetHiddenDevices();
        musik::Core::String src_str, dev_str;

        //
        // get a list of the drives currently on the system
        //
        musik::Core::StringArray curr_drives;
        FindAttachedDevices(curr_drives);

        //
        // remove devices that no longer exist
        //
        for (size_t i = 0; i < m_VisibleDrives.size(); i++)
        {
            bool is_active_drive = false;
            for (size_t j = 0; j < curr_drives.size(); j++)
            {
                if (m_VisibleDrives[i] == curr_drives[j])
                {
                    is_active_drive = true;
                    break;
                }
            }

            // is the drive hidden?
            bool is_hidden_device = false;
            for (size_t j = 0; j < hidden.size(); j++)
            {
                if(m_VisibleDrives[i] == hidden[j])
                {
                    is_hidden_device = true;
                    break;
                }
            }

            if (!is_active_drive || is_hidden_device)
            {
                PropertyTreeItem* pItem = m_DeviceItems[i];

                if (pItem == this->m_PropTree->GetFocusedItem())
                {
                    this->m_PropTree->SetFocusedItem(NULL);
                }

                pItem->Detach();
                delete pItem;
                m_DeviceItems.erase(m_DeviceItems.begin() + i);

                m_VisibleDrives.erase(m_VisibleDrives.begin() + i);
                --i;
            }
        }
            

        //
        // now we add drives that don't exist
        //
        for (size_t i = 0; i < curr_drives.size(); i++)
        {
            std::wstring curr_drive = curr_drives[i];

            // is the drive already visible?
            bool already_visible = false;
            for(size_t j = 0; j < m_VisibleDrives.size(); j++)
            {
                if(m_VisibleDrives[j] == curr_drive)
                {
                    already_visible = true;
                    break;
                }
            }

            // is the drive hidden?
            bool is_hidden_device = false;
            for (size_t j = 0; j < hidden.size(); j++)
            {
                if(hidden[j] == curr_drive)
                {
                    is_hidden_device = true;
                    break;
                }
            }

            // ... if its either, we don't want to display it
            if (already_visible || is_hidden_device)
            {
                continue;
            }

            UINT type = GetDriveType(curr_drives[i].c_str());
            
            switch(type)
            {
            case DRIVE_REMOVABLE:
            case DRIVE_FIXED:
            case DRIVE_REMOTE:
                {
                    musik::Core::String driveName = GetDeviceVolumeDescription(curr_drives[i]);

                    info = musik::Core::PlaylistInfo(driveName, musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE, -1);

                    PropertyTreeItem *removable = 
                        this->m_PropTree->InsertItem(new PropertyTreeItem(), m_DevicesRootItem);

                    removable->SetPlaylistInfo(info);
                    removable->SetBitmaps(m_pRDOff, m_pRDOn);

                    m_VisibleDrives.push_back(curr_drive);
                    m_DeviceItems.push_back(removable);
                }
                break;

            case DRIVE_CDROM:
                {
                    int curr_drive_id = curr_drives[i][0];

                    int count = 0;
                    const char* pStr = BASS_CD_GetDriveDescription(count);
                    while (pStr != 0)
                    {
                        int id = (int)BASS_CD_GetDriveLetter(count) + (int)'A';

                        if (id == curr_drive_id)
                        {
                            curr_drive_id = count;
                            break;
                        }

                        count++;
                        pStr = BASS_CD_GetDriveDescription(count);
                    }

                    musik::Core::String driveName = GetDeviceVolumeDescription(curr_drives[i]);

                    info = musik::Core::PlaylistInfo(driveName, musik::Core::MUSIK_PLAYLIST_TYPE_CDPLAYER, curr_drive_id);

                    PropertyTreeItem *cdplayer = 
                        this->m_PropTree->InsertItem(new PropertyTreeItem(), m_DevicesRootItem);

                    cdplayer->SetPlaylistInfo(info);
                    cdplayer->SetBitmaps(m_pCDOff, m_pCDOn);

                    m_VisibleDrives.push_back(curr_drive);
                    m_DeviceItems.push_back(cdplayer);
                }
                break;
            default:
                break;            
            }
        }
    }
}

int SourcesView::GetDriveCount()
{
    wchar_t logdev[1024];
    int dwSize = (int)GetLogicalDriveStrings(1024, logdev);

    musik::Core::StringArray drives;
    int count = 0;
    for (int i = 0; i < dwSize; i++)
    {
        if (logdev[i] == (wchar_t)'\0')
            count++;
    }

    return count;
}

musik::Core::String SourcesView::GetDeviceVolumeDescription(const musik::Core::String& drive_name)
{
    wchar_t volName[128] = _T("");

    GetVolumeInformation(
        drive_name.c_str(),
        volName,    // volume name
        128,        // volume name buffer size
        NULL,        // serial number
        NULL,
        NULL,
        NULL,
        NULL
   );

    musik::Core::String drivestr = volName;

    if (drivestr == _T(""))
    {
        drivestr = _T("unnamed");
    }

    drivestr += _T(" (");
    drivestr += drive_name;
    drivestr += _T(")");

    drivestr.Replace(_T("\\"), _T(""));

    return drivestr;
}

void SourcesView::FindAttachedDevices(musik::Core::StringArray& target)
{
    target.clear();

    wchar_t logdev[1024];
    int dwSize = (int)GetLogicalDriveStrings(1024, logdev);

    wchar_t buffer[32];
    musik::Core::StringArray drives;
    int at = 0;
    int count = 0;
    for (int i = 0; i < dwSize; i++)
    {
        wchar_t cur = logdev[i];
        buffer[at] = cur;
        
        if (cur == (wchar_t)'\0')
        {   
            target.push_back(buffer);
            count++;
            at = -1;
        }
        at++;
    }
}

void SourcesView::OnMenuCreateStockDynamicPlaylists()
{
    Globals::Library->InitDefaultDynPlaylists();
    this->LoadDynPlaylistItems();
}

void SourcesView::OnItemDragged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
    LPNMPROPTREE pNMPropTree = (LPNMPROPTREE) pNotifyStruct;
    *plResult = 0;

    if (pNMPropTree->pItem)
    {
        switch(pNMPropTree->pItem->GetPlaylistType())
        {
        case MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST:
        case MUSIK_SOURCES_TYPE_QUICKSEARCH:
        case MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST:
        case musik::Core::MUSIK_PLAYLIST_TYPE_CDPLAYER:
            return;
            break;
        }

        m_DragAndDropInfo = new musik::Core::PlaylistInfo(pNMPropTree->pItem->GetPlaylistInfo());
        //
        DoDrag(pNMPropTree->pItem);
        //
        delete m_DragAndDropInfo;
        m_DragAndDropInfo = NULL;
    }
}

void SourcesView::OnItemChanged(NMHDR* pNotifyStruct, LRESULT* plResult)
{
    LPNMPROPTREE pNMPropTree = (LPNMPROPTREE)pNotifyStruct;
      *plResult = 0;  

    this->NotifyParentItemChanged(pNMPropTree->pItem);
}

bool SourcesView::LoadRemovableLibrary(PropertyTreeItem* ptrItem)
{
    musik::Core::String drive_path;
    musik::Core::String drive_letter = ptrItem->GetPlaylistInfo().GetName();

    int pos = drive_letter.find(_T("(")) + 1;
    int len = drive_letter.size();
    drive_path = drive_letter.Right(len - pos);
    drive_path = drive_path.Left(drive_path.length() - 1);
    drive_path += "\\";

    musik::Core::String fn = drive_path + _T(".musikproject\\musik_removable_u.db");
    musik::Core::RemLibrary* pNewLib = NULL;

    if (musik::Core::Filename::FileExists(fn))
    {
        pNewLib = new musik::Core::RemLibrary(fn);
        pNewLib->SetRemPath(drive_path, true);
    }

    else
    {
        int res = MessageBox(
            _T("You have not yet built a library for this drive, would you like to do so now?"),
            _T("musikCube"),
            MB_YESNO | MB_ICONQUESTION);

        if (res == IDYES)
        {
            pNewLib = new musik::Core::RemLibrary(fn);
            pNewLib->AddPath(drive_path);
            pNewLib->SetRemPath(drive_path, false);

            musik::Core::BatchAdd* params = new musik::Core::BatchAdd(
                NULL,
                drive_path + _T("*.*"),
                NULL,
                pNewLib,
                NULL,
                Globals::Functor);

            musik::Core::BatchAddTask* ptask = new musik::Core::BatchAddTask();
            ptask->m_Params = params;

            pNewLib->QueueTask(ptask);
        }
        else
        {
            this->DeselectAllItems();
            return false;
        }
    }

    if (pNewLib)
    {
        if (ptrItem->m_Library)
            delete ptrItem->m_Library;

        ptrItem->m_Library = pNewLib;
    }

    return true;
}

void SourcesView::ShowDockBarMenu()
{
    if (this->m_EditInPlace.IsVisible())
    {
        this->m_EditInPlace.Cancel();
    }

    CPoint pos;
    ::GetCursorPos(&pos);
    ScreenToClient(&pos);
    
    PropertyTreeItem* pItem = this->m_PropTree->FindItem(pos);

    if (!pItem || pItem->IsRootLevel())
    {
        if (pItem != this->m_DynPlaylistRootItem)
        {
            return;
        }
    }

    int type = musik::Core::MUSIK_PLAYLIST_TYPE_UNKNOWN;
    type = pItem->GetPlaylistType();

    CMenu main_menu;
    CMenu* popup_menu;

    main_menu.LoadMenu(IDR_SOURCES_MENU);
    popup_menu = main_menu.GetSubMenu(0);

    // shuffle / clear now playing
    if (type == musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_SHUFFLEPLAYLIST, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_CLEAR, MF_ENABLED);
    }
    else 
    {    
        popup_menu->RemoveMenu(ID_SOURCES_SHUFFLEPLAYLIST, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_CLEAR, MF_BYCOMMAND);
    }

    // rename, delete
    if (type == musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD || 
         type == musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_RENAME, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_DELETE, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_MOVEUP, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_MOVEDOWN, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_RENAME, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_DELETE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_MOVEUP, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_MOVEDOWN, MF_BYCOMMAND);
    }

    // copy to / move to
    if (type == musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD || 
         type == musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC || 
         type == musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_COPYTO, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_MOVEFILESTO, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_COPYTO, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_MOVEFILESTO, MF_BYCOMMAND);
    }

    // edit query
    if (type == musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC)
        popup_menu->EnableMenuItem(ID_SOURCES_EDITQUERY, MF_ENABLED);
    else
        popup_menu->RemoveMenu(ID_SOURCES_EDITQUERY, MF_BYCOMMAND);
    
    // cdrom functions
    if (type == musik::Core::MUSIK_PLAYLIST_TYPE_CDPLAYER)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_RIPCD, MF_ENABLED);    
        popup_menu->EnableMenuItem(ID_SOURCES_QUERYFREEDB, MF_ENABLED);
        popup_menu->EnableMenuItem(ID_SOURCES_REFRESHCD, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_RIPCD, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_QUERYFREEDB, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_REFRESHCD, MF_BYCOMMAND);
    }

    if (type == MUSIK_SOURCES_TYPE_PLUGIN)
    {
        int id = pItem->GetPlaylistID();
        MainFramePlugin& plugin = Globals::Plugins.at(id);
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINEXECUTE, plugin.CanExecute() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));    
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINCONFIGURE, plugin.CanConfigure() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINSTOP, plugin.CanStop() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));
        popup_menu->EnableMenuItem(ID_SOURCES_PLUGINABOUT, plugin.CanAbout() ? MF_ENABLED : (MF_DISABLED|MF_GRAYED));
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINEXECUTE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINCONFIGURE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINSTOP, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_PLUGINABOUT, MF_BYCOMMAND);
    }

    // LIBRARY: "show all songs"
    if (type == musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY)
        popup_menu->EnableMenuItem(ID_SOURCES_SHOWALLSONGS, MF_ENABLED);    
    else
        popup_menu->RemoveMenu(ID_SOURCES_SHOWALLSONGS, MF_BYCOMMAND);

    // DEVICES AND COLLECTED: "unload device" & "synchronize device"
    if ((type == musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE) || (type == MUSIK_SOURCES_TYPE_COLLECTED))
    {
        if (pItem->m_Library != NULL)
        {
            popup_menu->EnableMenuItem(ID_SOURCES_SYNCHRONIZE, MF_ENABLED);
            popup_menu->EnableMenuItem(ID_SOURCES_SYNCHRONIZEDPATHS, MF_ENABLED);
        }
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_SYNCHRONIZE, MF_BYCOMMAND);
        popup_menu->RemoveMenu(ID_SOURCES_SYNCHRONIZEDPATHS, MF_BYCOMMAND);
    }

    // DEVICES: "unload device"
    if (type == musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE)
    {
        popup_menu->EnableMenuItem(ID_SOURCES_UNMOUNT, MF_ENABLED);
    }
    else
    {
        popup_menu->RemoveMenu(ID_SOURCES_UNMOUNT, MF_BYCOMMAND);
    }

    if (pItem == this->m_DynPlaylistRootItem)
        popup_menu->EnableMenuItem(ID_SOURCES_CREATESTOCK, MF_ENABLED);
    else
        popup_menu->RemoveMenu(ID_SOURCES_CREATESTOCK, MF_BYCOMMAND);

    ClientToScreen(&pos);
    popup_menu->TrackPopupMenu(0, pos.x, pos.y, this);
}

void SourcesView::OnMenuQueryFreeDB()
{
    int WM_SOURCESQUERYFREEDB = RegisterWindowMessage(_T("SOURCESQUERYFREEDB"));
    m_Parent->SendMessage(WM_SOURCESQUERYFREEDB, NULL);
}

void SourcesView::TogglePluginsVisible()
{
    Globals::Preferences->SetPluginsVisible(!Globals::Preferences->IsPluginsVisible());
    this->ReloadAllItems();
}

void SourcesView::ToggleDynPlaylistsVisible()
{
    Globals::Preferences->SetDynPlaylistsVisible(!Globals::Preferences->IsDynPlaylistsVisible());
    this->ReloadAllItems();
}

void SourcesView::ToggleStdPlaylistsVisible()
{
    Globals::Preferences->SetPlaylistsVisible(!Globals::Preferences->IsPlaylistsVisible());
    this->ReloadAllItems();
}

void SourcesView::ToggleDevicesVisible()
{
    Globals::Preferences->SetDevicesVisible(!Globals::Preferences->IsDevicesVisible());
    this->ReloadAllItems();
}

void SourcesView::OnMenuCopyFilesTo()
{
    musik::Core::StringArray files;
    this->GetSelFilenames(this->m_PropTree->GetFocusedItem(), files);
    //
    this->SourcesFileOp(FO_COPY, files);
}

void SourcesView::OnMenuMoveFilesTo()
{
    musik::Core::StringArray files;
    this->GetSelFilenames(this->m_PropTree->GetFocusedItem(), files);
    //
    this->SourcesFileOp(FO_MOVE, files);
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    // http://www.codeguru.com/forum/printthread.php?t=309472
    switch (uMsg)
    {
    case BFFM_INITIALIZED :
        if(lpData)
          ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
        break;
    default:
        break;
    }
    return 0; 
}

musik::Core::String SourcesView::SourcesFileOp(DWORD mode, const musik::Core::StringArray& files, musik::Core::String default_path)
{
    TCHAR path[MAX_PATH] = _T("");

    if (files.size())
    {
        BROWSEINFO bi = { 0 };
        bi.lpszTitle = _T("Please select a target directory.");
        bi.hwndOwner = GetSafeHwnd();
        bi.ulFlags |= BIF_NEWDIALOGSTYLE;
        bi.pidlRoot = NULL;

        if (default_path != _T(""))
        {
            bi.lParam =  (long)default_path.c_str(); 
            bi.lpfn    = BrowseCallbackProc;
        }

        LPITEMIDLIST pidl = SHBrowseForFolder (&bi);

        if (pidl != 0)
        {
            // get the name of the folder and put it in path
            SHGetPathFromIDList (pidl, path);

            // free memory used
            IMalloc * imalloc = 0;
            if (SUCCEEDED(SHGetMalloc (&imalloc)))
            {
                imalloc->Free (pidl);
                imalloc->Release ();
            }
            
            // use Michael Dunn's convenient SHFileOperation wrapper to
            // perform the copy using Window's shell api...
            BOOL apicalled;
            CShellFileOp copy;

            for (size_t i = 0; i < files.size(); i++)
            {
                copy.AddSourceFile(files.at(i));
            }
            copy.AddDestFile(path);
            
            copy.SetOperationFlags(
                mode,             // op type
                AfxGetMainWnd(), // handle to main window
                FALSE,           // flag - silent mode?
                FALSE,           // flag - allow undo?
                FALSE,           // flag - should wild cards affect files only?
                FALSE,           // flag - suppress confirmation messages?
                TRUE,            // flag - suppress confirmation messages when making directories?
                FALSE,           // flag - rename files when name collisions occur?
                FALSE);         // flag - simple progress dialog?

            if (!copy.Go(&apicalled))
                MessageBox(
                _T("File operation was unsuccessful."),
                _T("musikCube"),
                MB_ICONWARNING | MB_OK);
        }
    }

    return path;
}

void SourcesView::OnMenuRenameItem()
{
    this->RenameItem();
}

void SourcesView::OnMenuEditQuery()
{
    DynamicPlaylistDialog dlg;
    
    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        musik::Core::String name, query;

        Globals::Library->GetDynPlaylistInfo(
            pItem->GetPlaylistID(),
            name,
            query);

        dlg.SetName(name);
        dlg.SetQuery(query);
        this->DeselectAllItems(false);
        int ret = dlg.DoModal();

        if (ret == IDOK)
        {
            Globals::Library->RewriteDynPlaylistQuery(
                pItem->GetPlaylistID(),
                dlg.GetQuery());

            if (name != dlg.GetName())
            {
                Globals::Library->RenameDynPlaylist(
                    pItem->GetPlaylistID(),
                    dlg.GetName());
                this->LoadDynPlaylistItems();
            }
        }
    }
}

void SourcesView::OnMenuDeleteItem()
{
    this->DeleteSelectedItem();
}

void SourcesView::OnMenuShuffleNowPlaying()
{
    Globals::Player->Shuffle();

    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        if (pItem->GetPlaylistType() == musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING)
        {
            int WM_UPDATEPLAYLIST = RegisterWindowMessage(_T("UPDATEPLAYLIST"));
            m_Parent->SendMessage(WM_UPDATEPLAYLIST);
        }
    }
}

void SourcesView::ReloadAllItems()
{
    this->m_PropTree->SetRedraw(FALSE);
    this->m_PropTree->DetachAllItems();

    musik::Core::PlaylistInfo info;

    // browse
    if ( ! m_BrowseRootItem)
    {
        info = musik::Core::PlaylistInfo(_T("Browse"), -1, -1);
        //
        m_BrowseRootItem = new PropertyTreeItem();
        m_BrowseRootItem->SetPlaylistInfo(info);
        m_BrowseRootItem->Expand(true);
    }
    //
    this->m_PropTree->InsertItem(m_BrowseRootItem);
    LoadBrowseItems();

    // devices
    if (m_DevicesTimerID)
    {
        KillTimer(m_DevicesTimerID);
        m_DevicesTimerID = NULL;
    }
    //
    if (Globals::Preferences->IsDevicesVisible())
    {
        if ( ! m_DevicesRootItem)
        {
            info = musik::Core::PlaylistInfo(_T("Devices"), -1, -1);
            m_DevicesRootItem = new PropertyTreeItem();
            m_DevicesRootItem->SetPlaylistInfo(info);
            m_DevicesRootItem->Expand(true);
        }
        //
        this->m_PropTree->InsertItem(m_DevicesRootItem);
        //
        for (size_t i = 0; i < m_DeviceItems.size(); i++)
        {
            this->m_PropTree->InsertItem(m_DeviceItems[i], m_DevicesRootItem);
        }
        //
        RefreshDevices();
        m_DevicesTimerID = this->SetTimer(SOURCES_DEVICE_TIMER_ID, 2000, NULL);
    }
    
    // standard playlists
    if (Globals::Preferences->IsPlaylistsVisible())
    {
        if ( ! m_StdPlaylistRootItem)
        {
            info = musik::Core::PlaylistInfo(_T("Playlists"), -1, -1);
            m_StdPlaylistRootItem = new PropertyTreeItem();
            m_StdPlaylistRootItem->SetPlaylistInfo(info);
            m_StdPlaylistRootItem->Expand(true);
        }
        //
        this->m_PropTree->InsertItem(m_StdPlaylistRootItem);
        LoadStdPlaylistItems();
    }

    // dynamic playlists
    if (Globals::Preferences->IsDynPlaylistsVisible())
    {
        if ( ! m_DynPlaylistRootItem)
        {
            info = musik::Core::PlaylistInfo(_T("Dynamic Playlists"), -1, -1);
            m_DynPlaylistRootItem = new PropertyTreeItem();
            m_DynPlaylistRootItem->SetPlaylistInfo(info);
            m_DynPlaylistRootItem->Expand(true);
        }
        //
        this->m_PropTree->InsertItem(m_DynPlaylistRootItem);
        LoadDynPlaylistItems();
    }

    // plugins
    if ( Globals::Preferences->IsPluginsVisible())
    {
        if ( ! m_PluginsRootItem)
        {
            info = musik::Core::PlaylistInfo(_T("Plugins"), -1, -1);
            m_PluginsRootItem = new PropertyTreeItem();
            m_PluginsRootItem->SetPlaylistInfo(info);
            m_PluginsRootItem->Expand(TRUE);
        }
        //
        this->m_PropTree->InsertItem(m_PluginsRootItem);
        LoadPluginItems();
    }

    this->m_PropTree->UpdateScrollbar();

    this->m_PropTree->SetRedraw(TRUE);
    this->m_PropTree->RedrawWindow();
}

void SourcesView::FocusLibrary()
{
    this->FocusItem(this->m_BrowseItems.at(0));
}

void SourcesView::FocusQuickSearch()
{
    this->FocusItem(m_SearchItem);
}

void SourcesView::FocusNowPlaying()
{
    this->FocusItem(m_BrowseItems.at(2));
}

void SourcesView::FocusItem(PropertyTreeItem* pItem)
{
    if ( ! pItem)
    {
        this->DeselectAllItems(true);
        return;
    }

    this->DeselectAllItems(false);
    this->m_PropTree->SetFocusedItem(pItem);

    // emulate a mouse click
    this->NotifyParentItemChanged(pItem);
}

int SourcesView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (DockableWindow::OnCreate(lpCreateStruct) == -1)
    {
        return -1;
    }

    m_DropTarget->Register(this);

    // default font
    //
    if ( ! this->m_DefaultFont.CreateStockObject(DEFAULT_GUI_FONT))
    {
        return -1;
    }

    // dock bar properties
    //
    m_ForceDrawBorder = false;
    SetBarStyle(GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
    EnableDocking(CBRS_ALIGN_LEFT | CBRS_ALIGN_RIGHT);
    ShowGripper(false);

    // proptree
    //
    if ( ! this->m_PropTree->Create(WS_CHILD | WS_VISIBLE | PTS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_SOURCES))
    {
        return -1;
    }

    // edit in place
    //
    m_EditInPlace.Create(WS_CHILD | WS_CLIPCHILDREN, CRect(0, 0, 0, 0), m_PropTree, IDC_EDITINPLACE);
    m_EditInPlace.EnableWindow(FALSE);
    m_EditInPlace.SetFont(&this->m_DefaultFont);
    m_EditInPlace.SetMessageTarget(this);

    this->m_PropTree->SetFont(&this->m_DefaultFont);

    // devices
    //
    m_DevicesTimerID = this->SetTimer(SOURCES_DEVICE_TIMER_ID, 2000, NULL);

    // just as it says...
    //
    ReloadAllItems();

    return 0;
}

void SourcesView::LoadBrowseItems()
{
    if (m_BrowseItems.size() > 0)
    {
        for (size_t i = 0; i < m_BrowseItems.size(); i++)
        {
            this->m_PropTree->InsertItem(m_BrowseItems[i], m_BrowseRootItem);
        }
    }

    else
    {
        musik::Core::PlaylistInfo info;
        PropertyTreeItem *lib, *nowplaying, *netstreams, *search;

        // library
        info = musik::Core::PlaylistInfo("Library", musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY, NULL);
        lib = new PropertyTreeItem();
        lib->SetPlaylistInfo(info);
        lib->SetBitmaps(m_pLibOff, m_pLibOn);
        lib->m_Library = Globals::Library;
        this->m_PropTree->InsertItem(lib, m_BrowseRootItem);
        m_BrowseItems.push_back(lib);

        // collected
        info = musik::Core::PlaylistInfo("Collected", MUSIK_SOURCES_TYPE_COLLECTED, NULL);
        m_CollectedItem = new PropertyTreeItem();
        m_CollectedItem->SetPlaylistInfo(info);
        m_CollectedItem->SetBitmaps(m_pColOff, m_pColOn);
        this->m_PropTree->InsertItem(m_CollectedItem, m_BrowseRootItem);
        m_BrowseItems.push_back(m_CollectedItem);

        // now playing
        info = musik::Core::PlaylistInfo("Now Playing", musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING, NULL);
        nowplaying = new PropertyTreeItem();
        nowplaying->SetPlaylistInfo(info);
        nowplaying->SetBitmaps(m_pNPOff, m_pNPOn);
        this->m_PropTree->InsertItem(nowplaying, m_BrowseRootItem);
        m_BrowseItems.push_back(nowplaying);

        // net radio
        info = musik::Core::PlaylistInfo("Net Radio", MUSIK_SOURCES_TYPE_NETSTREAMS, NULL);
        netstreams = new PropertyTreeItem();
        netstreams->SetPlaylistInfo(info);
        netstreams->SetBitmaps(m_pNSOff, m_pNSOn);
        this->m_PropTree->InsertItem(netstreams, m_BrowseRootItem);
        m_BrowseItems.push_back(netstreams);

        // search
        info = musik::Core::PlaylistInfo("Search...", MUSIK_SOURCES_TYPE_QUICKSEARCH, -1); 
        search = new PropertyTreeItem();
        search->SetPlaylistInfo(info);
        search->SetBitmaps(m_pQSOff, m_pQSOn);
        this->m_PropTree->InsertItem(search, m_BrowseRootItem);
        m_BrowseItems.push_back(search);
        m_SearchItem = search;
    }
}

void SourcesView::LoadPluginItems()
{
    // load the plugins...
    if (m_PluginItems.size())
    {
        for (size_t i = 0; i < m_PluginItems.size(); i++)
        {
            this->m_PropTree->DeleteItem(m_PluginItems.at(i));
        }
    }
    m_PluginItems.clear();

    PropertyTreeItem* temp;
    for (size_t i = 0; i < Globals::Plugins.size(); i++)
    {
        if (Globals::Plugins.at(i).InSources())
        {
            musik::Core::PlaylistInfo info;
            info = musik::Core::PlaylistInfo(Globals::Plugins.at(i).GetPluginName(), MUSIK_SOURCES_TYPE_PLUGIN, i);
            
            temp = this->m_PropTree->InsertItem(new PropertyTreeItem(), m_PluginsRootItem);
            temp->SetPlaylistInfo(info);
            temp->SetBitmaps(m_pPlugOff, m_pPlugOn);

            m_PluginItems.push_back(temp);
        }
    }
}

void SourcesView::LoadStdPlaylistItems()
{
    if (!Globals::Preferences->IsPlaylistsVisible())
        return;

    // load the playlists...
    if (m_StdPlaylistItems.size())
    for (size_t i = 0; i < m_StdPlaylistItems.size(); i++)
    {
        this->m_PropTree->DeleteItem(m_StdPlaylistItems.at(i));
    }
    m_StdPlaylistItems.clear();

    musik::Core::PlaylistInfoArray items;
    Globals::Library->GetAllStdPlaylists(items);

    PropertyTreeItem* temp;
    for (size_t i = 0; i < items.size(); i++)
    {
        temp = this->m_PropTree->InsertItem(new PropertyTreeItem(), m_StdPlaylistRootItem);
        temp->SetPlaylistInfo(items.at(i));
        temp->SetBitmaps(m_pPLOff, m_pPLOn);
        m_StdPlaylistItems.push_back(temp);
    }

    // load the "Create..."
    musik::Core::PlaylistInfo info;
    info = musik::Core::PlaylistInfo("Create...", MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST, -1); 
    temp = this->m_PropTree->InsertItem(new PropertyTreeItem(), m_StdPlaylistRootItem);
    temp->SetPlaylistInfo(info);
    temp->SetBitmaps(m_pPLOff, m_pPLOn);
    m_StdPlaylistItems.push_back(temp);
}

void SourcesView::DeleteLastCreatedPlaylist(int type)
{
    musik::Core::PlaylistInfoArray items;

    switch (type)
    {
    case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
        {
            Globals::Library->GetAllStdPlaylists(items);
            Globals::Library->DeleteStdPlaylist(items.at(items.size() - 1).GetID());
        }
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        {
            Globals::Library->GetAllDynPlaylists(items);
            Globals::Library->DeleteDynPlaylist(items.at(items.size() - 1).GetID());
        }
        break;
    }
}

void SourcesView::LoadDynPlaylistItems()
{
    if (!Globals::Preferences->IsDynPlaylistsVisible())
    {
        return;
    }

    // load the playlists...
    for (size_t i = 0; i < m_DynPlaylistItems.size(); i++)
    {
        this->m_PropTree->DeleteItem(m_DynPlaylistItems.at(i));
    }

    m_DynPlaylistItems.clear();

    musik::Core::PlaylistInfoArray items;
    Globals::Library->GetAllDynPlaylists(items, false);

    PropertyTreeItem* temp;
    for (size_t i = 0; i < items.size(); i++)
    {
        temp = this->m_PropTree->InsertItem(new PropertyTreeItem(), m_DynPlaylistRootItem);
        temp->SetPlaylistInfo(items.at(i));
        temp->SetBitmaps(m_pDPOff, m_pDPOn);

        m_DynPlaylistItems.push_back(temp);
    }

    // load the "Create..."
    musik::Core::PlaylistInfo info;
    info = musik::Core::PlaylistInfo("Create...", MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST, -1); 
    temp = this->m_PropTree->InsertItem(new PropertyTreeItem(), m_DynPlaylistRootItem);
    temp->SetPlaylistInfo(info);
    temp->SetBitmaps(m_pDPOff, m_pDPOn);
    m_DynPlaylistItems.push_back(temp);
}

void SourcesView::OnDropFiles(HDROP hDropInfo, bool right_button)
{
    // set cursor back to hour glass
    SetCursor(LoadCursor(NULL, IDC_WAIT));

    // see if the drag landed on an existing
    // playlist, if it did, we'll append
    CPoint pos;
    ::GetCursorPos(&pos);
    ScreenToClient(&pos);

    PropertyTreeItem* pItem = this->m_PropTree->FindItem(pos);

    // no selected item to drop on
    if (!pItem || pItem->IsRootLevel())
    {
        DragFinish(hDropInfo);
        return;
    }

    // dnd vars
    size_t nNumFiles;
    TCHAR szNextFile [MAX_PATH];
    SHFILEINFO  rFileInfo;
    nNumFiles = DragQueryFile (hDropInfo, -1, NULL, 0);

    // build a list of dnd filenames
    musik::Core::StringArray* files = new musik::Core::StringArray();
    musik::Core::String sTemp;
    for (size_t i = 0; i < nNumFiles; i++)
    {
        if (DragQueryFile(hDropInfo, i, szNextFile, MAX_PATH) > 0)
        {
            // at a dir: recurse 
            SHGetFileInfo(szNextFile, 0, &rFileInfo, sizeof(rFileInfo), SHGFI_ATTRIBUTES);
            if (rFileInfo.dwAttributes & SFGAO_FOLDER)
            {
                sTemp = szNextFile;
                sTemp += "\\*.*";

                musik::Core::Directory::OpenDir(sTemp, files);
            }        

            // att a file: add to list
            else
            {
                files->push_back(szNextFile);
            }
        }
    }

    // distribute the drop event
    bool delete_files = false;
    bool success = false;
    switch(pItem->GetPlaylistType())
    {
    case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
        success = (musik::Core::MUSIK_LIBRARY_OK == Globals::Library->AppendStdPlaylist(pItem->GetPlaylistID(), *files));
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY:
        success = this->DropAddToLibrary(Globals::MainLibrary, files);
        delete_files = false;
        break;

    case MUSIK_SOURCES_TYPE_COLLECTED:
        success = this->DropAddToCollected(files);
        delete_files = false;
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        success = this->DropOnRemovableDevice(files, pItem);
        delete_files = false;
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        success = this->DropAddNowPlaying(files);
        break;

    default:
        break;
    }

    DragFinish(hDropInfo);
    if (delete_files)
    {
        delete files;
    }

    // if the mouse is no longer in the window, be sure
    // to set the hovered item to NULL and redraw.
    CRect rcWindow;
    ::GetWindowRect(this->GetSafeHwnd(), &rcWindow);

    CPoint mousePos;
    ::GetCursorPos(&mousePos);

    if (!rcWindow.PtInRect(mousePos))
    {
        OnDragLeave();
    }
}

bool SourcesView::DropAddToCollected(musik::Core::StringArray* pFiles)
{
    if (! this->m_CollectedItem->m_Library)
    {
        this->LoadCollectedLibrary();
    }

    return this->DropAddToLibrary(this->m_CollectedItem->m_Library, pFiles);
}

bool SourcesView::DropAddToLibrary(musik::Core::Library* pLibrary, musik::Core::StringArray* pFiles)
{
    musik::Core::BatchAdd* batchAddParams;
    musik::Core::BatchAddTask* batchAddThread;

    batchAddParams = new musik::Core::BatchAdd();
    batchAddParams->m_Library = pLibrary;
    batchAddParams->m_Files = pFiles;
    batchAddParams->m_DeleteFilelist = true;
    batchAddParams->m_Functor = Globals::Functor;

    batchAddThread = new musik::Core::BatchAddTask();
    batchAddThread->m_Params = batchAddParams;
    
    pLibrary->QueueTask(batchAddThread);

    return true;
}

bool SourcesView::DropOnRemovableDevice(musik::Core::StringArray* pFiles, PropertyTreeItem* pItem)
{
    musik::Core::Library*& pItemLib = pItem->m_Library;

    if (!pItemLib)
    {
        this->LoadRemovableLibrary(pItem);
    }

    bool success = false;
    if (pItemLib)
    {
        CPoint pos;
        ::GetCursorPos(&pos);

        CMenu popup;
        popup.LoadMenu(IDR_DROPDEVICE_MENU);

        CMenu* pSubMenu = popup.GetSubMenu(0);
        int selMenu = (int)pSubMenu->TrackPopupMenu(TPM_RETURNCMD, pos.x, pos.y, this, NULL);

        switch(selMenu)
        {
        case ID_DROPDEVICE_COPYFILESTODEVICE:
            success = this->DropCopyFilesToDevice(pFiles, pItem);
            break;

        case ID_DROPDEVICE_ADDTODEVICE:
            success = this->DropAddToDevice(pFiles, pItem);
            break;
        }
    }
    else
    {
        MessageBox(
            _T("No library has been built for this device yet. Please select it to build a library."),
            _T("musikCube"),
            MB_OK | MB_ICONINFORMATION
       );

        delete pFiles;
    }

    return success;
}

bool SourcesView::DropCopyFilesToDevice(musik::Core::StringArray* pFiles, PropertyTreeItem* pItem)
{
    musik::Core::String copyPath = SourcesFileOp(
            FO_COPY, 
            *pFiles, 
            ((musik::Core::RemLibrary*)pItem->m_Library)->GetRemPath() 
       );

    if (copyPath != _T(""))
    {
        musik::Core::BatchAdd* batchAddParams;
        musik::Core::BatchAddTask* batchAddThread;

        batchAddParams = new musik::Core::BatchAdd();
        batchAddParams->m_Library = pItem->m_Library;
        batchAddParams->m_Directory = copyPath + _T("\\*.*");
        batchAddParams->m_Functor = Globals::Functor;

        batchAddThread = new musik::Core::BatchAddTask();
        batchAddThread->m_Params = batchAddParams;
        
        pItem->m_Library->QueueTask(batchAddThread);
    }

    delete pFiles;
    return true;
}

bool SourcesView::DropAddToDevice(musik::Core::StringArray* pFiles, PropertyTreeItem* pItem)
{
    bool success = this->DropAddToLibrary(pItem->m_Library, pFiles);

    if (!success)
    {
        delete pFiles;
    }

    return success;
}

bool SourcesView::DropAddNowPlaying(musik::Core::StringArray* files)
{
    bool was_nowplaying_empty = Globals::Player->GetPlaylist()->GetCount() ? false : true;
    bool begin_trans = false;

    Globals::Library->BeginTransaction();
    musik::Core::Song song;
    for (size_t i = 0; i < files->size(); i++)
    {
        // add song to library, if necessary
        Globals::Library->AddSong(files->at(i));

        // get the song, add it to the pl aylist
        if (Globals::Library->GetSongAttrFromFilename(files->at(i), song) == musik::Core::MUSIK_LIBRARY_OK)
        {
            Globals::Player->GetPlaylist()->Add(song);
        }
    }
    Globals::Library->EndTransaction();

    // post a message to the main frame, letting
    // it know that drag and drop has completed
    if (was_nowplaying_empty)
    {
        if (!Globals::Player->IsPlaying())
            Globals::Player->Play(0, 0, musik::Core::MUSIK_PLAYER_SONG_USER_ADVANCE, true);
        else
            Globals::Player->FindNewIndex(Globals::Player->GetPlaying()->GetID());
    }

    return true;
}

void SourcesView::CreateNewStdPlaylist()
{
    PropertyTreeItem* pItem = this->m_StdPlaylistItems[this->m_StdPlaylistItems.size() - 1];
    Globals::Library->CreateStdPlaylist(L"New Playlist", musik::Core::StringArray(), m_StdPlaylistItems.size() - 1);
    EditItemInPlace(pItem, MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST, L"New Playlist");
}

void SourcesView::CreateNewDynPlaylist()
{
    PropertyTreeItem* pItem = this->m_DynPlaylistItems[this->m_DynPlaylistItems.size() - 1];
    EditItemInPlace(pItem, MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST, CString("New Playlist"));
}

void SourcesView::DeselectAllItems(bool redraw)
{
    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        pItem->Select(FALSE);
        this->m_PropTree->SetFocusedItem(NULL);

        if (redraw)
        {
            Invalidate();
        }
    }
}

void SourcesView::GetSelFilenames(PropertyTreeItem* pItem, musik::Core::String& files)
{
    int nMode = pItem->GetPlaylistType();

    switch (nMode)
    {
    case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
        Globals::Library->GetStdPlaylistFns(pItem->GetPlaylistID(), files);
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        Globals::Library->GetDynPlaylistFns(pItem->GetPlaylistID(), files); 
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        Globals::Library->GetStdPlaylistFns(*Globals::Player->GetPlaylist(), files);
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY:
        Globals::Library->GetAllFns(files);
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        if (pItem->m_Library)
            pItem->m_Library->GetAllFns(files);
        break;

    default:
        break;
    } 
}

void SourcesView::GetSelFilenames(PropertyTreeItem* pItem, musik::Core::StringArray& files)
{
    int nMode = pItem->GetPlaylistType();
    
    switch (nMode)
    {
    case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
        Globals::Library->GetStdPlaylistFns(pItem->GetPlaylistID(), files, false);
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
        Globals::Library->GetDynPlaylistFns(pItem->GetPlaylistID(), files, false); 
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
        Globals::Library->GetStdPlaylistFns(*Globals::Player->GetPlaylist(), files, false);
        break;


    case musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY:
        Globals::Library->RawQueryStringArray(_T(" SELECT DISTINCT filename FROM songs; "), files);
        break;

    case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        if (pItem->m_Library)
            pItem->m_Library->RawQueryStringArray(_T(" SELECT DISTINCT filename FROM songs; "), files);
        break;

    default:
        break;
    }     
}

void SourcesView::DoDrag(PropertyTreeItem* pItem)
{
    if (!pItem || 
        pItem->GetPlaylistType() == musik::Core::MUSIK_PLAYLIST_TYPE_CDPLAYER ||
        pItem->GetPlaylistType() == MUSIK_SOURCES_TYPE_PLUGIN)
        return;

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
    GetSelFilenames(pItem, files);

    if (files.size())
    {
        // CStringList containing files
        for (size_t i = 0; i < files.size(); i++)
        {
            lsDraggedFiles.AddTail(files.at(i));
            uBuffSize += files.at(i).GetLength() + 1;
        }

        files.clear();
    }

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

    pDrop->fWide = TRUE;

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
    etc.cfFormat = m_DropID;
    datasrc.CacheGlobalData (m_DropID, hgBool, &etc);

    // Start the drag 'n' drop!
    DROPEFFECT dwEffect = datasrc.DoDragDrop (DROPEFFECT_COPY | DROPEFFECT_MOVE);

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

void SourcesView::RenameItem(PropertyTreeItem* pItem, int mode, CPoint pos)
{
    if ( ! pItem)
    {
        pItem = this->m_PropTree->GetFocusedItem();
    }

    if ( ! pItem)
    {
        return;
    }

    // not trying to rename a root
    if (pItem->IsRootLevel())
    {
        return;
    }

    // not renaming the library, now playing,
    // or quick search
    if (pItem == m_BrowseItems.at(0) || pItem == m_BrowseItems.at(1))
        return;

    CPoint nPos;
    if (pos.x == -1 && pos.y == -1)
        nPos = pItem->GetLocation();
    else
        nPos = pos;

    CRect rcClient;
    GetClientRect(rcClient);
    
    int left;
    Globals::DrawGraphics ? left = 32 : left = 12;
    CRect rect(left, nPos.y + 4, rcClient.Width() - 1, nPos.y + PROPTREEITEM_DEFHEIGHT - 2);

    if (this->m_PropTree->IsScrollVisible())
    {
        rect.right -= 5;
    }

    EditItemInPlace(pItem, mode, pItem->GetLabelText());
}

void SourcesView::RemoveStdPlaylist(PropertyTreeItem* pItem)
{
    for (size_t i = 0; i < m_StdPlaylistItems.size(); i++)
    {
        if (pItem == m_StdPlaylistItems.at(i))
        {
            m_StdPlaylistItems.erase(m_StdPlaylistItems.begin() + i);
            this->m_PropTree->DeleteItem(pItem);
            return;
        }
    }
}

void SourcesView::RemoveDynPlaylist(PropertyTreeItem* pItem)
{
    for (size_t i = 0; i < m_DynPlaylistItems.size(); i++)
    {
        if (pItem == m_DynPlaylistItems.at(i))
        {
            m_DynPlaylistItems.erase(m_DynPlaylistItems.begin() + i);
            this->m_PropTree->DeleteItem(pItem);
            return;
        }
    }
}

void SourcesView::DeleteSelectedItem()
{
    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem)
    {
        switch(pItem->GetPlaylistType())
        {
        case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
        case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                PropertyTreeItemArray* ptrItems;

                // remove the playlist from the database
                if (pItem->GetPlaylistType() == musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD)
                {
                    ptrItems = &m_StdPlaylistItems;
                    Globals::Library->DeleteStdPlaylist(pItem->GetPlaylistID());
                }
                else
                {
                    ptrItems = &m_DynPlaylistItems;
                    Globals::Library->DeleteDynPlaylist(pItem->GetPlaylistID());
                }

                // find selection index
                int SelAt = 0;
                for (size_t i = 0; i < ptrItems->size(); i++)
                {
                    if (ptrItems->at(i) == pItem)
                    {
                        SelAt = i;
                        break;
                    }
                }

                pItem->Detach();
                delete pItem;
                ptrItems->erase(ptrItems->begin() + SelAt);

                // after detaching, if there is anothing playlist
                // in the same position, select that.
                if (ptrItems->size() > 1)
                {
                    if (SelAt == (int)ptrItems->size() - 1)
                    {
                        SelAt--;
                    }

                    FocusItem(ptrItems->at(SelAt));
                }
                // no playlists left to select! focus the library or
                // now playing.
                else
                {
                    if (!Globals::Player->IsPlaying())
                    {
                        FocusLibrary();
                    }
                    else
                    {
                        FocusNowPlaying();
                    }
                }

                this->m_PropTree->UpdateScrollbar();    // may need to hide scrollbar
            }
            break;
        }
    }
}

void SourcesView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    // user pressed f2 to rename an entry
    if (nChar == VK_F2)
        RenameItem();

    // user requested playlist deletion
    if (nChar == VK_DELETE)
    {
        DeleteSelectedItem();
    }

    if (GetKeyState(VK_CONTROL) < 0)
    {
        if (nChar == VK_UP)
            MoveSelectedItemUp();
        if (nChar == VK_DOWN)
            MoveSelectedItemDown();
    }
}

LRESULT SourcesView::OnItemEditCancel(WPARAM wParam, LPARAM lParam)
{
    int cmd = (int)lParam;

    this->m_EditInPlaceItem->GetPlaylistInfo().m_Name = this->m_PreEditInPlaceText;
    this->m_EditInPlaceItem = false;

    switch(cmd)
    {
    case MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST:
        DeleteLastCreatedPlaylist(musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD);
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST:
        DeleteLastCreatedPlaylist(musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC);
        break;

    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        FinishQuickSearch();
        break;
    }

    m_EditInPlace.EnableWindow(FALSE);
    this->DeselectAllItems();

    return 0L;
}

LRESULT SourcesView::OnItemEditCommit(WPARAM wParam, LPARAM lParam)
{
    int cmd = (int)lParam;
    ListViewRowEditor* pSender = (ListViewRowEditor*)wParam;
    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    CString sName = pSender->GetString();

    this->m_EditInPlaceItem->GetPlaylistInfo().m_Name = this->m_PreEditInPlaceText;
    this->m_EditInPlaceItem = false;

    switch (cmd)
    {
    case MUSIK_SOURCES_EDITINPLACE_RENAME:
        {
            int type = pItem->GetPlaylistType();
            switch (type)
            {
            case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
                Globals::Library->RenameStdPlaylist(pItem->GetPlaylistID(), (musik::Core::String)sName);
                break;

            case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
                Globals::Library->RenameDynPlaylist(pItem->GetPlaylistID(), (musik::Core::String)sName);
                break;
            }

            pItem->GetPlaylistInfo().m_Name = sName;
            Invalidate();
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        {
            if (pItem)
            {
                m_SearchItem->GetPlaylistInfo().m_Name = _T("Search...");
                FinishQuickSearch();
            }
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWSTDPLAYLIST:
        {
            LoadStdPlaylistItems();
            pItem = m_StdPlaylistItems[m_StdPlaylistItems.size() - 2];
            //
            Globals::Library->RenameStdPlaylist(pItem->GetPlaylistID(), (musik::Core::String)sName);
            m_StdPlaylistItems[m_StdPlaylistItems.size() - 2]->GetPlaylistInfo().m_Name = sName;
            this->m_PropTree->UpdateScrollbar();
        }
        break;

    case MUSIK_SOURCES_EDITINPLACE_NEWDYNPLAYLIST:
        {
            this->DeselectAllItems();
            Invalidate();

            DynamicPlaylistDialog dlg(m_EditInPlace.GetString());
            int ret = dlg.DoModal();

            if (ret == IDOK)
            {
                Globals::Library->CreateDynPlaylist(dlg.GetName(), dlg.GetQuery(), m_DynPlaylistItems.size() - 1);
                LoadDynPlaylistItems();
                this->m_PropTree->UpdateScrollbar();
            }
        }
        break;

    default:
        
        break;
    }

    m_EditInPlace.EnableWindow(FALSE);
    this->DeselectAllItems();

    SetFocus();
    return 0L;
}

void SourcesView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    ScreenToClient(&point);
    m_RightClick = this->m_PropTree->FindItem(point);
    if (m_RightClick)
    {
    this->ShowDockBarMenu();
    }
}

void SourcesView::EnableQuickSearch()
{   
    CPoint nPos = m_SearchItem->GetLocation();

    CRect rcClient;
    GetClientRect(rcClient);

    CString tmp = _T("");
    EditItemInPlace(m_SearchItem, MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH, tmp);

    this->m_QuickStr = L"";

    int WM_SOURCESQUICKSEARCH = RegisterWindowMessage(_T("SOURCESQUICKSEARCH"));
    this->m_Parent->SendMessage(WM_SOURCESQUICKSEARCH);
}

LRESULT SourcesView::OnItemEditChange(WPARAM wParam, LPARAM lParam)
{
    int cmd = (int)lParam;

    switch (lParam)
    {
    case MUSIK_SOURCES_EDITINPLACE_QUICKSEARCH:
        {
            CString editInPlaceText;
            m_EditInPlace.GetWindowText(editInPlaceText);

            if ((editInPlaceText != this->m_QuickStr) && (editInPlaceText != L""))
            {
                this->m_QuickStr = editInPlaceText;
                //
                if (this->m_SearchTimerID) { this->KillTimer(this->m_SearchTimerID); }
                this->m_SearchTimerID = this->SetTimer(SOURCES_SEARCH_TIMER_ID, 700, NULL);
            }
        }
        break;
    }

    return 0L;
}

void SourcesView::FinishQuickSearch()
{
    this->KillTimer(m_SearchTimerID);
    this->m_SearchItem->GetPlaylistInfo().m_Name = L"Search...";
    this->m_SearchItem->Select(FALSE);
    this->m_PropTree->SetFocusedItem(NULL);
    this->DeselectAllItems(true);
}

void SourcesView::OnDragLeave()
{
    PropertyTree::m_Hovered = NULL;
    RedrawWindow();
}

DROPEFFECT SourcesView::OnDragOver(CPoint point)
{
    DROPEFFECT dwReturn = DROPEFFECT_NONE;
    PropertyTreeItem* pItem = this->m_PropTree->FindItem(point);

    if (pItem != NULL)
    {
        switch(pItem->GetPlaylistType())
        {
        case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        case musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY:
        case MUSIK_SOURCES_TYPE_COLLECTED:
            dwReturn = DROPEFFECT_COPY;
            break;

        case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:

            dwReturn = DROPEFFECT_COPY;
            //
            if  (m_DragAndDropInfo != NULL)
            {
                if (
                       m_DragAndDropInfo->GetType() == pItem->GetPlaylistType()
                    && m_DragAndDropInfo->GetID() == pItem->GetPlaylistID()
                   )
                {
                    dwReturn = DROPEFFECT_NONE;
                }
            }
            //
            break;

        case musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING:

            dwReturn = DROPEFFECT_COPY;
            //
            if  (m_DragAndDropInfo != NULL)
            {
                // disallow drag and drop from Now Playing to Now Playing
                if (m_DragAndDropInfo->GetType() == pItem->GetPlaylistType())
                {
                    dwReturn = DROPEFFECT_NONE;
                }
            }
            //
            break;
        }

        if (pItem != PropertyTree::m_Hovered)
        {
            PropertyTree::m_Hovered = pItem;
            RedrawWindow();
        }
    }

    return dwReturn;
}

void SourcesView::EditItemInPlace(PropertyTreeItem* pItem, int args, const CString text)
{
    if (pItem)
    {
        CPoint nPos = pItem->GetLocation();
        CRect rcClient;
        GetClientRect(rcClient);

        int left;
        Globals::DrawGraphics ? left = 32 : left = 12;
        int scrollPos = this->m_PropTree->GetScrollPosition();
        CRect rect(left, nPos.y + 4 - scrollPos, rcClient.Width() - 1, nPos.y + PROPTREEITEM_DEFHEIGHT - 2 - scrollPos);

        if (this->m_PropTree->IsScrollVisible())
        {
            rect.right -= 5;
        }

        this->m_EditInPlaceItem = pItem;
        this->m_PreEditInPlaceText = this->m_EditInPlaceItem->GetPlaylistInfo().m_Name;
        this->m_EditInPlaceItem->GetPlaylistInfo().m_Name = L"";

        m_EditInPlace.EnableWindow(TRUE);
        m_EditInPlace.SetArgs((void*)args);
        m_EditInPlace.MoveWindow(rect);
        m_EditInPlace.SetFocus();
        m_EditInPlace.SetString(text);
        m_EditInPlace.ShowWindow(SW_SHOWDEFAULT);
    }
}

void SourcesView::OnMenuRipSelectedCD()
{
    DiscRipDialog* ripDialog;
    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();

    if (pItem && pItem->GetPlaylistType() == musik::Core::MUSIK_PLAYLIST_TYPE_CDPLAYER)
    {
        ripDialog = new DiscRipDialog(this, this->m_PropTree->GetFocusedItem()->GetPlaylistID());
    }
    else
    {
        ripDialog = new DiscRipDialog(this);
    }

    ripDialog->DoModal();

    delete ripDialog;
}

void SourcesView::OnMenuRefreshCD()
{
    int WM_SOURCESCDAUDIO = RegisterWindowMessage(_T("SOURCESCDAUDIO"));
    m_Parent->SendMessage(WM_SOURCESCDAUDIO, this->m_PropTree->GetFocusedItem()->GetPlaylistID());
}

void SourcesView::OnMenuDeviceUnloadLibrary()
{
    PropertyTreeItem *pSel = this->m_RightClick;
    if (pSel)
    {
        pSel->UnloadLibrary();
        this->DeselectAllItems();
        this->FocusLibrary();
    }

}

void SourcesView::OnMenuMoveItemUp()
{
    this->MoveSelectedItemUp();
}

void SourcesView::OnMenuMoveItemDown()
{
    this->MoveSelectedItemDown();
}

void SourcesView::MoveSelectedItemUp()
{
    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem && pItem->GetPrev())
    {
        // moving up takes the current playlist and decreases it's
        // order, then increases the other playlist's order.
        switch(pItem->GetPlaylistType())
        {
        case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
        case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                PropertyTreeItem* pA = pItem;
                PropertyTreeItem* pB = pItem->GetPrev();

                int a = pA->GetPlaylistInfo().m_Order;
                int b = pB->GetPlaylistInfo().m_Order;

                pA->GetPlaylistInfo().m_Order = b;
                pB->GetPlaylistInfo().m_Order = a;

                Globals::Library->UpdatePlaylistOrder(pA->GetPlaylistInfo());
                Globals::Library->UpdatePlaylistOrder(pB->GetPlaylistInfo());

                // move A above B
                pA->Detach();
                PropertyTreeItem* pFirst = pB->GetPrev();
                pB->SetPrev(pA);
                pA->SetNext(pB);
                pA->SetPrev(pFirst);

                if (pFirst)
                    pFirst->SetNext(pA);

                if (pB->GetParent()->GetChild() == pB)
                    pB->GetParent()->SetChild(pA);

                Invalidate();
            }
            break;
        }
    }
}

void SourcesView::MoveSelectedItemDown()
{
    PropertyTreeItem* pItem = this->m_PropTree->GetFocusedItem();
    if (pItem && pItem->GetNext())
    {
        // moving up takes the current playlist and decreases it's
        // order, then increases the other playlist's order.
        switch(pItem->GetPlaylistType())
        {
        case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
        case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                PropertyTreeItem* pA = pItem;
                PropertyTreeItem* pB = pItem->GetNext();
    
                // need to make sure we don't move the item below "Create..."
                if (m_StdPlaylistItems.size() && pB == m_StdPlaylistItems.at(m_StdPlaylistItems.size() - 1))
                    return;
                if (m_DynPlaylistItems.size() && pB == m_DynPlaylistItems.at(m_DynPlaylistItems.size() - 1))
                    return;

                // swap the order in the database
                int a = pA->GetPlaylistInfo().m_Order;
                int b = pB->GetPlaylistInfo().m_Order;

                pA->GetPlaylistInfo().m_Order = b;
                pB->GetPlaylistInfo().m_Order = a;

                Globals::Library->UpdatePlaylistOrder(pA->GetPlaylistInfo());
                Globals::Library->UpdatePlaylistOrder(pB->GetPlaylistInfo());

                // move B above A
                pB->Detach();        
                PropertyTreeItem* pFirst = pA->GetPrev();
                pA->SetPrev(pB);
                pB->SetNext(pA);
                pB->SetPrev(pFirst);

                if (pFirst)
                    pFirst->SetNext(pB);

                if (pA->GetParent()->GetChild() == pA)
                    pA->GetParent()->SetChild(pB);

                Invalidate();
            }
            break;
        }
    }
}

void SourcesView::OnSourcesNowPlayingClear()
{
    Globals::Player->GetPlaylist()->Clear();
    this->FocusNowPlaying();    // forces update of playlist in main ui
}

void SourcesView::OnMenuPluginExecute()
{
    PropertyTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
    {
        Globals::Plugins.at(pFocus->GetPlaylistID()).Execute();
    }
}

void SourcesView::OnMenuPluginConfigure()
{
    PropertyTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
    {
        Globals::Plugins.at(pFocus->GetPlaylistID()).Configure();
    }
}

void SourcesView::OnMenuPluginStop()
{
    PropertyTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
    {
        Globals::Plugins.at(pFocus->GetPlaylistID()).Stop();
    }
}

void SourcesView::OnMenuPluginAbout()
{
    PropertyTreeItem* pFocus = this->m_PropTree->GetFocusedItem();
    if (pFocus)
        Globals::Plugins.at(pFocus->GetPlaylistID()).About();
}

void SourcesView::OnMenuLibraryShowAllSongs()
{
    this->FocusLibrary();
}

void SourcesView::OnMenuSynchronizeDevice()
{
    if (GetFocusedItem()->m_Library)
    {
        Globals::SynchronizeDirs(GetFocusedItem()->m_Library);
    }
}

void SourcesView::LoadCollectedLibrary()
{
    if (m_CollectedItem->m_Library)
    {
        m_CollectedItem->UnloadLibrary();
    }

    CString oldpath_rel, fn;
    oldpath_rel = _T("~/.musikproject/musik_collected_u.db");
    fn = musikCubeApp::RelToAbs(oldpath_rel);

    musik::Core::Library* pNewLib = new musik::Core::Library(fn.GetBuffer());

    m_CollectedItem->m_Library = pNewLib;
    Globals::Collected = pNewLib;
}

void SourcesView::NotifyParentItemChanged(PropertyTreeItem* pItem)
{
    if (pItem)
    {
        // root level items are for display purposes only. we also don't wanwt
        if (pItem->IsRootLevel())
        {
            return;
        }

        int playlistType = pItem->GetPlaylistType();

        // Send the MainFrame a preliminary notification, letting it know that the
        // selection has changed. In the future this should be the only message we
        // send to the MainFrame, and we should provide an optional default playlist.
        int WM_SOURCESITEMCLICKED = RegisterWindowMessage(_T("SOURCESITEMCLICKED"));
        m_Parent->SendMessage(WM_SOURCESITEMCLICKED, NULL);

        switch (playlistType)
        {
        case musik::Core::MUSIK_PLAYLIST_TYPE_LIBRARY:
            {
                Globals::SetMainLibrary(Globals::MainLibrary);
                this->LoadDynPlaylistItems();
                this->LoadStdPlaylistItems();

                int WM_SOURCESLIBRARY = RegisterWindowMessage(_T("SOURCESLIBRARY"));
                m_Parent->SendMessage(WM_SOURCESLIBRARY, NULL);
            }
            break;

        case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
        case MUSIK_SOURCES_TYPE_COLLECTED:                  // surprise! its just a device!
            {
                if ( ! pItem->m_Library)
                {
                    switch (playlistType)
                    {
                    case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
                        this->LoadRemovableLibrary(pItem);
                        break;

                    case MUSIK_SOURCES_TYPE_COLLECTED:
                        this->LoadCollectedLibrary();
                        break;
                    }
                }

                if ( (! pItem) || ( ! pItem->m_Library)) { return; }

                Globals::SetMainLibrary(pItem->m_Library);
                this->LoadStdPlaylistItems();
                this->LoadDynPlaylistItems();
                //
                int WM_SOURCESREMOVABLEDEV = RegisterWindowMessage(_T("SOURCESREMOVABLEDEV"));
                m_Parent->SendMessage(WM_SOURCESREMOVABLEDEV, NULL);

                this->TellUserAboutCollected();
            }
            break;

        case musik::Core::MUSIK_PLAYLIST_TYPE_NOWPLAYING:
            {
                int WM_SOURCESNOWPLAYING = RegisterWindowMessage(_T("SOURCESNOWPLAYING"));
                m_Parent->SendMessage(WM_SOURCESNOWPLAYING, NULL);
            }
            break;

        case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
            {
                int WM_SOURCESSTDPLAYLIST = RegisterWindowMessage(_T("SOURCESSTDPLAYLIST"));
                m_Parent->SendMessage(WM_SOURCESSTDPLAYLIST, NULL);
            }
            break;

        case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
            {
                int WM_SOURCESDYNPLAYLIST = RegisterWindowMessage(_T("SOURCESDYNPLAYLIST"));
                m_Parent->SendMessage(WM_SOURCESDYNPLAYLIST, NULL);
            }
            break;

        case MUSIK_SOURCES_TYPE_QUICKSEARCH:
            this->EnableQuickSearch();
            break;

        case MUSIK_SOURCES_TYPE_NEWSTDPLAYLIST:
            this->CreateNewStdPlaylist();
            break;

        case MUSIK_SOURCES_TYPE_NEWDYNPLAYLIST:
            this->CreateNewDynPlaylist();
            break;

        case MUSIK_SOURCES_TYPE_NETSTREAMS:
            {
                int WM_SOURCESNETSTREAMS = RegisterWindowMessage(_T("SOURCESNETSTREAMS"));
                m_Parent->SendMessage(WM_SOURCESNETSTREAMS, NULL);
            }
            break;

        case musik::Core::MUSIK_PLAYLIST_TYPE_CDPLAYER:
            {
                int WM_SOURCESCDAUDIO = RegisterWindowMessage(_T("SOURCESCDAUDIO"));
                m_Parent->SendMessage(WM_SOURCESCDAUDIO, pItem->GetPlaylistID());
            }
            break;

        case MUSIK_SOURCES_TYPE_PLUGIN:
            {
                int pluginIndex = pItem->GetPlaylistID();
                Globals::Plugins[pluginIndex].OnSources();
            }
            break;
        }
    }
}

void SourcesView::LoadBitmaps()
{
    musikCubeApp* ptrApp = (musikCubeApp*)AfxGetApp();
    CString workingdir = ptrApp->GetWorkingDir();

    m_pLibOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\lib_uf.png"));
    m_pLibOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\lib_f.png"));

    m_pNPOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\np_uf.png"));
    m_pNPOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\np_f.png"));

    m_pQSOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\qs_uf.png"));
    m_pQSOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\qs_f.png"));

    m_pCDOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\cd_uf.png"));
    m_pCDOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\cd_f.png"));

    m_pPLOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\pl_uf.png"));
    m_pPLOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\pl_f.png"));

    m_pDPOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\dp_uf.png"));
    m_pDPOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\dp_f.png"));

    m_pRDOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\rd_uf.png"));
    m_pRDOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\rd_f.png"));

    m_pPlugOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\plug_uf.png"));
    m_pPlugOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\plug_f.png"));

    m_pNSOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\ns_uf.png"));
    m_pNSOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\ns_f.png"));

    m_pColOff = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\col_uf.png"));
    m_pColOn = Gdiplus::Bitmap::FromFile(
        workingdir + _T("\\img\\col_f.png"));
}

void SourcesView::DeviceTimerTick()
{
    this->RefreshDevices();
    this->m_PropTree->RefreshItems();
}

void SourcesView::SearchTimerTick()
{
    int WM_SOURCESQUICKSEARCHSTR = RegisterWindowMessage(_T("SOURCESQUICKSEARCHSTR"));
    m_Parent->SendMessage(WM_SOURCESQUICKSEARCHSTR, (WPARAM)&m_QuickStr);

    this->KillTimer(this->m_SearchTimerID);
    this->m_SearchTimerID = NULL;
}

void SourcesView::OnTimer(UINT nIDEvent)
{
    if (nIDEvent == this->m_DevicesTimerID)
    {
        this->DeviceTimerTick();
    }
    else if (nIDEvent == this->m_SearchTimerID)
    {
        this->SearchTimerTick();
    }
}

void SourcesView::OnMenuSynchronizedPaths()
{
    DirectorySyncDialog dirSyncDlg(this, this->GetFocusedItem()->m_Library);
    dirSyncDlg.DoModal();
}

void SourcesView::TellUserAboutCollected()
{
    if ( ! Globals::Preferences->GetUserKnowsAboutCollected())
    {
        MessageBox(
            L"It appears this is your first time using the \"Collected\" library.\n\r\n\r"
            L"\"Collected\" has its own set of standard and dynamic playlists, and can be thought\r\n"
            L"of as a \"holding tank\" for songs that you want to play, but don't necessarily want\r\n"
            L"in your main library yet (or at all).\n\r\n\r"
            L"This message will only be displayed once.",
            L"musikCube",
            MB_OK | MB_ICONINFORMATION);

        Globals::Preferences->SetUserKnowsAboutCollected(true);
        Globals::Preferences->SavePrefs();
    }
}

///////////////////////////////////////////////////

} } // namespace musik::Cube