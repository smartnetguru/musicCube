///////////////////////////////////////////////////
// 
// Info:
//
//    ...
//
// Credits:
//
//    musikCube by Casey Langen
//    Plugin by Martin Dittus
//
///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlugin.h"

///////////////////////////////////////////////////

#ifndef COMPILE_STANDALONE

///////////////////////////////////////////////////

#include "musikPluginDlg.h"

// plugin properties
#include "musikPluginProps.h"


///////////////////////////////////////////////////

// plugin info
musik::Core::String g_Name = STR_PLUGIN_NAME;
musik::Core::String g_FullName = STR_PLUGIN_NAME _T(" ") STR_PLUGIN_VERSION;
musik::Core::String g_Description = _T( "Small player UI.");

musik::Core::String g_CaptionAbout = STR_ABOUT_CAPTION;
musik::Core::String g_CaptionConfig = STR_CONFIG_CAPTION;
musik::Core::String g_CaptionMain = STR_MAIN_CAPTION;

// link to website that lists the recommendations
musik::Core::String g_WebsiteURL = STR_PLUGIN_WEBSITE;

// etc
CMainDlg* g_MainDlg;

// open plugin when sources icon is clicked?
bool g_OpenOnSources = false;


// *** THESE NEED TO BE SET BY YOU ***

bool g_CanConfigure = false;
bool g_CanAbout = true;
//bool g_CanExecute = true;
//bool g_CanStop = false;
bool g_InSources = true;        // visible in the sources panel?
bool g_WantsPrefs = true;        // musikCube can take care of loading and saving preferences for you

///////////////////////////////////////////////////

// these will get initialized automatically by musikCube

HMODULE g_Instance = NULL;
musik::Core::Library* g_Library = NULL;
musik::Core::Player* g_Player = NULL;
HWND g_MainWnd = NULL;
musik::Core::Playlist* g_VisiblePlaylist = NULL;
CIniFile* g_Prefs = NULL;

///////////////////////////////////////////////////

// you do not want to change this.
// 
// revision 1:
//   - GetPluginRevision added
//   - functor implements OnSeek() event
//   - musikCube version 0.92.2

int g_PluginRevision = 1;

///////////////////////////////////////////////////

// do not modify these unless you know what you're doing

void SetPluginInstance( HMODULE instance )
{ g_Instance = instance; }

HMODULE GetPluginInstance()
{ return g_Instance; }

void Initialize( musik::Core::Library* lib, musik::Core::Player* player, HWND hwnd )
{ g_Library = lib; g_Player = player; g_MainWnd = hwnd; OnPluginAttach(); }

const musik::Core::String& GetPluginName()
{ return g_Name; }

const musik::Core::String& GetPluginDescription()
{ return g_Description; }

void SetPluginLibrary( musik::Core::Library* lib )
{ g_Library = lib; }

void Unload()
{ OnPluginDetach(); }

void SetVisiblePlaylist( musik::Core::Playlist* playlist )
{ g_VisiblePlaylist = playlist; }

void SetPrefs( CIniFile* prefs )
{ g_Prefs = prefs; LoadPrefs(); }

///////////////////////////////////////////////////

// tools

void LoadPrefs()
{
    g_Prefs->Clear();
    g_Prefs->ReadFile();

    g_OpenOnSources = g_Prefs->GetValueB("Window", "Open on click", false);

    if (g_MainDlg != NULL)
    {
        g_MainDlg->SetWinPosXY(
            g_Prefs->GetValueI("Window", "xpos", -1),
            g_Prefs->GetValueI("Window", "ypos", -1));
        g_MainDlg->SetAlwaysOnTop(g_Prefs->GetValueB("Window", "Always on top", false));        

        g_MainDlg->SetShowRating(g_Prefs->GetValueB("Display", "Show rating", true));
        g_MainDlg->SetShowRemaining(g_Prefs->GetValueB("Display", "Show remaining time", false));

        g_MainDlg->SetAlbumMode(g_Prefs->GetValueB("Queries", "Album Mode", true));
        g_MainDlg->SetQueryLimit(g_Prefs->GetValueI("Queries", "Limit", 50));
        g_MainDlg->SetAlbumLimit(g_Prefs->GetValueI("Queries", "Album Limit", 5));
        g_MainDlg->SetQueryRndNew(musik::Core::utf8to16(
            g_Prefs->GetValue("Queries", "Random New", 
                "julianday(timeadded) > (julianday('now') - 7) ORDER BY random() LIMIT %d")).c_str());
        g_MainDlg->SetQueryRndPop(musik::Core::utf8to16(
            g_Prefs->GetValue("Queries", "Random Popular", 
                "timesplayed > (SELECT avg(timesplayed) FROM "
                "songs WHERE timesplayed > 0) ORDER BY random() LIMIT %d")).c_str());
        g_MainDlg->SetQueryRndOld(musik::Core::utf8to16(
            g_Prefs->GetValue("Queries", "Random Old", 
                "julianday(timeadded) < (julianday('now') - 60) ORDER BY random() LIMIT %d")).c_str());
        g_MainDlg->SetQueryRndUnpl(musik::Core::utf8to16(
            g_Prefs->GetValue("Queries", "Random Unplayed", 
                "timesplayed = 0 ORDER BY random() LIMIT %d")).c_str());
        g_MainDlg->SetAlbumQuery(musik::Core::utf8to16(
            g_Prefs->GetValue("Queries", "Album Query",
                "LOWER(album) = (SELECT DISTINCT LOWER(album) FROM "
                "(SELECT album, count(title) AS numtracks FROM songs GROUP BY album HAVING "
                "%s) WHERE album <> '' AND numtracks > %d)")).c_str());
    }
}

void SavePrefs()
{
    g_Prefs->SetValueB("Window", "Open on click", g_OpenOnSources);

    if (g_MainDlg != NULL)
    {
        int x, y;
        g_MainDlg->GetWinPosXY(x, y);
        g_Prefs->SetValueI("Window", "xpos", x);
        g_Prefs->SetValueI("Window", "ypos", y);
        g_Prefs->SetValueB("Window", "Always on top", g_MainDlg->GetAlwaysOnTop());
        g_Prefs->SetValueB("Display", "Show rating", g_MainDlg->GetShowRating());
        g_Prefs->SetValueB("Display", "Show remaining time", g_MainDlg->GetShowRemaining());

        g_Prefs->SetValueB("Queries", "Album Mode", g_MainDlg->GetAlbumMode());
        g_Prefs->SetValueI("Queries", "Limit", g_MainDlg->GetQueryLimit());
        g_Prefs->SetValueI("Queries", "Album Limit", g_MainDlg->GetAlbumLimit());
        g_Prefs->SetValue("Queries", "Random New", musik::Core::utf16to8((LPCTSTR)g_MainDlg->GetQueryRndNew()).c_str());
        g_Prefs->SetValue("Queries", "Random Popular", musik::Core::utf16to8((LPCTSTR)g_MainDlg->GetQueryRndPop()).c_str());
        g_Prefs->SetValue("Queries", "Random Old", musik::Core::utf16to8((LPCTSTR)g_MainDlg->GetQueryRndOld()).c_str());
        g_Prefs->SetValue("Queries", "Random Unplayed", musik::Core::utf16to8((LPCTSTR)g_MainDlg->GetQueryRndUnpl()).c_str());
        g_Prefs->SetValue("Queries", "Album Query", musik::Core::utf16to8((LPCTSTR)g_MainDlg->GetAlbumQuery()).c_str());
    }

    g_Prefs->WriteFile();
}

// returns the main application window
CWnd* GetAppWindow()
{
    return NULL;
}

///////////////////////////////////////////////////

// dll exports

MUSIKPLUGIN_API bool CanExecute()
{
    if (g_MainDlg == NULL)                    // if player window dows not exist yet
        return true;                        // -> allow
    else if (g_MainDlg->IsWindowVisible())    // if window is already open
        return false;                        // -> nope
    else                                    // else (window exists and is hidden)
        return true;                        // -> allow
}

MUSIKPLUGIN_API bool CanStop()
{
    if (g_MainDlg == NULL)                    // if player window dows not exist yet
        return false;                        // -> nope
    else if (!g_MainDlg->IsWindowVisible())    // if window exists but is hidden
        return false;                        // -> nope
    else                                    // else (window exists and is open)
        return true;                        // -> allow
}


///////////////////////////////////////////////////

// functor implementation

void Functor::OnNewSong() { if (g_MainDlg != NULL) g_MainDlg->RefreshDisplay(); }
void Functor::OnPause() { if (g_MainDlg != NULL) g_MainDlg->RefreshDisplay(); }
void Functor::OnResume() { if (g_MainDlg != NULL) g_MainDlg->RefreshDisplay(); }
void Functor::OnStop() { if (g_MainDlg != NULL) g_MainDlg->RefreshDisplay(); }
void Functor::OnPlaybackFailed() { if (g_MainDlg != NULL) g_MainDlg->RefreshDisplay(); }

///////////////////////////////////////////////////

// implement me

void Configure()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CConfigDlg configDlg(g_CaptionConfig, GetAppWindow());
    configDlg.SetOntop(g_Prefs->GetValueB("Window", "Always on top", true));

    INT_PTR nRet = configDlg.DoModal();

    if (nRet == IDOK)
    {
        if (g_MainDlg != NULL)
            g_MainDlg->SetAlwaysOnTop(configDlg.GetOntop());

        SavePrefs();
    }
}

///////////////////////////////////////////////////

// implement me

void About()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CAboutDlg aboutDlg(g_CaptionAbout, GetAppWindow());
    aboutDlg.SetPluginName(g_FullName);
    aboutDlg.SetLink(g_WebsiteURL);
    aboutDlg.DoModal();
}

///////////////////////////////////////////////////

// implement me

CWnd* g_AppWnd = NULL;

void Execute()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    /*
    if (!CanExecute())
    {
        //MessageBox( NULL, _T("Can't execute this plugin."), g_Name, MB_ICONINFORMATION );
        return;
    }
    */

    if (g_MainDlg == NULL)
    {
        g_AppWnd = CWnd::FromHandle( g_MainWnd );
        g_MainDlg = new CMainDlg(g_CaptionMain, g_Player, g_AppWnd );
        LoadPrefs();
    }

    int message = RegisterWindowMessage( _T( "SOURCESUNSEL" ) );
    ::SendMessage( g_MainWnd, message, NULL, NULL );

    g_MainDlg->ShowWindow(SW_SHOW);
    g_MainDlg->BringWindowToTop();

}

///////////////////////////////////////////////////

// implement me

void Stop()
{
    if ( g_MainDlg )
        g_MainDlg->ShowWindow(SW_HIDE);

    SavePrefs();
}

///////////////////////////////////////////////////

// optional

void OnPluginAttach()
{
    // init
    g_MainDlg = NULL;
}

///////////////////////////////////////////////////

// optional

void OnPluginDetach()
{
    if ( g_Prefs )
    {
        SavePrefs();

        delete g_Prefs;
        g_Prefs = NULL;
    }

    // add your code here
    if (g_MainDlg != NULL) 
    {
        g_MainDlg->ShowWindow(SW_HIDE);
        delete g_MainDlg;
    }
}

///////////////////////////////////////////////////

void OnSources()
{    
    //if (g_OpenOnSources)
    //{
        Execute();
    //}
}

///////////////////////////////////////////////////

#endif // COMPILE_STANDALONE

///////////////////////////////////////////////////
