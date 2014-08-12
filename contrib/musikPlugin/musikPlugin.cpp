///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlugin.h"
#include "windows.h"
#include "iniFile.h"

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***

musik::Core::String g_Name = _T( "plugin name" );
musik::Core::String g_Description = _T( "plugin description" );
bool g_CanConfigure = true;
bool g_CanAbout = true;
bool g_CanExecute = true;
bool g_CanStop = true;
bool g_InSources = true;		// visible in the sources panel?
bool g_WantsPrefs = true;		// musikCube can take care of loading and saving preferences for you

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

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved	 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

    return TRUE;
}

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
{ g_Prefs = prefs; }

///////////////////////////////////////////////////

// implement me

void Configure()
{
}

///////////////////////////////////////////////////

// implement me

void About()
{
}

///////////////////////////////////////////////////

// implement me

void Execute()
{
	musik::Core::Playlist playlist;
	musik::Core::SongInfoArray info;
	g_Library->GetAllSongs( playlist );
	g_Library->GetInfoArrayFromPlaylist( playlist, info ); 
}

///////////////////////////////////////////////////

// implement me

void Stop()
{
}

///////////////////////////////////////////////////

// optional

void OnPluginAttach()
{
}

///////////////////////////////////////////////////

// optional

void OnPluginDetach()
{
	if ( g_Prefs )
	{
		g_Prefs->WriteFile();
		delete g_Prefs;
		g_Prefs = NULL;
	}

	// add your code here
}

///////////////////////////////////////////////////

void OnSources()
{
}

///////////////////////////////////////////////////

