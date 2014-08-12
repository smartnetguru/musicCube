///////////////////////////////////////////////////

#ifndef __C_MUSIKPLUGIN_H__
#define __C_MUSIKPLUGIN_H__

///////////////////////////////////////////////////

#include <bass.h>
#include <musikCore.h>
#include <iostream>
#include <vector>

#ifdef MUSIKPLUGIN_EXPORTS
#define MUSIKPLUGIN_API __declspec(dllexport)
#else
#define MUSIKPLUGIN_API __declspec(dllimport)
#endif

///////////////////////////////////////////////////

extern "C" {

///////////////////////////////////////////////////

// these can be implemented and/or initialized 

extern bool g_CanConfigure;
extern bool g_CanAbout;
extern musik::Core::String g_Name;
extern musik::Core::String g_Description; 
extern musik::Core::String g_Formats;
MUSIKPLUGIN_API void Configure();
MUSIKPLUGIN_API void About();
MUSIKPLUGIN_API void OnSources();
void OnPluginAttach();
void OnPluginDetach();

///////////////////////////////////////////////////

// stream prototype

///////////////////////////////////////////////////

// *** YOU DO NOT WANT TO MODIFY ANY OF THESE ***
// musikCube uses them for communication between the
// plugin and the main UI.

extern HMODULE g_Instance;
extern musik::Core::Library* g_Library;
extern musik::Core::Player* g_Player;
extern HWND g_MainWnd;

MUSIKPLUGIN_API const musik::Core::String& GetPluginFormats();
MUSIKPLUGIN_API const musik::Core::String& GetPluginName();
MUSIKPLUGIN_API const musik::Core::String& GetPluginDescription();
MUSIKPLUGIN_API void Unload();

///////////////////////////////////////////////////

// all of these must be implemented

MUSIKPLUGIN_API bool CanPlay(const musik::Core::String& filename);
MUSIKPLUGIN_API bool Play(HSTREAM stream, int offset = 0);
MUSIKPLUGIN_API bool CanSeek();
MUSIKPLUGIN_API bool Seek(HSTREAM stream, int ms);
MUSIKPLUGIN_API bool Pause(HSTREAM stream);
MUSIKPLUGIN_API bool Resume(HSTREAM stream);
MUSIKPLUGIN_API bool Stop(HSTREAM stream); 
MUSIKPLUGIN_API int  GetDuration(HSTREAM stream);
MUSIKPLUGIN_API int  GetTime(HSTREAM stream);
MUSIKPLUGIN_API bool IsActive(HSTREAM stream);
MUSIKPLUGIN_API HSTREAM LoadFile(const musik::Core::String& file);
MUSIKPLUGIN_API bool WriteTag(const musik::Core::SongInfo& info);
MUSIKPLUGIN_API bool ReadTag(const musik::Core::String& fn, musik::Core::SongInfo& target);

///////////////////////////////////////////////////

}   // extern "C"

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////

