///////////////////////////////////////////////////

#include "stdafx.h"
#include "musikPlugin.h"

///////////////////////////////////////////////////

// *** THESE NEED TO BE SET BY YOU ***
#define PLUGIN_NAME _T("core_mod")
#define PLUGIN_VERSION _T("0.02")
#define PLUGIN_DESCRIPTION _T("MOD File Format")

musik::Core::String g_Name = PLUGIN_NAME;
musik::Core::String g_Description = PLUGIN_DESCRIPTION;
musik::Core::String g_Formats = _T( "it,xm,s3m,mtm,mod,mo3,umx" );		// list of supported extensions, comma separated
bool g_CanConfigure = false;
bool g_CanAbout = true;

///////////////////////////////////////////////////

// these will get initialized automatically by musikCube

HMODULE g_Instance = NULL;
HSYNC g_Sync = NULL;

///////////////////////////////////////////////////

// specific to THIS plugin

///////////////////////////////////////////////////

BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved	 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		OnPluginAttach();
		break;

	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

    return TRUE;
}

///////////////////////////////////////////////////

// do not modify these unless you know what you're doing

const musik::Core::String& GetPluginDescription()
{ return g_Description; }

const musik::Core::String& GetPluginName()
{ return g_Name; }

const musik::Core::String& GetPluginFormats()
{ return g_Formats; }

void Unload()
{ OnPluginDetach(); }

///////////////////////////////////////////////////

// implement me

void Configure()
{
}

///////////////////////////////////////////////////

// implement me

void About()
{
	WCHAR text[1024];
	WCHAR title[32];
	wsprintf(text,L"%s - %s\n%s\nby Otto",PLUGIN_NAME, PLUGIN_VERSION, PLUGIN_DESCRIPTION);
	wsprintf(title,L"%s %s",PLUGIN_NAME,PLUGIN_VERSION);
	MessageBox(NULL,text,title,MB_OK);
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

}

///////////////////////////////////////////////////

bool CanPlay( const musik::Core::String& filename )
{
	musik::Core::Filename MFN( filename );
	musik::Core::String extension = MFN.GetExtension();
	if ( extension == _T( "it" )
		 || extension == _T( "xm" )
		 || extension == _T( "s3m" )
		 || extension == _T( "mtm" )
		 || extension == _T( "mod" )
		 || extension == _T( "mo3" )
		 || extension == _T( "umx" )
		)
		return true;

	return false;
}

///////////////////////////////////////////////////

bool Play( HSTREAM stream, int offset )
{
	BASS_ChannelPlay(
		stream,
		true
		);

	if ( offset )
		Seek( stream, offset );

	return true;
}

///////////////////////////////////////////////////

bool CanSeek()
{ 
	return true; 
}

///////////////////////////////////////////////////

bool Seek( HSTREAM stream, int ms )
{
	float secs = (float)ms / 1000.0f;
	QWORD pos = BASS_ChannelSeconds2Bytes( stream, secs );
	BASS_ChannelSetPosition( (HMUSIC)stream, pos );
	return true;
}

///////////////////////////////////////////////////

bool Pause( HSTREAM stream )
{
	BASS_ChannelPause( stream );
	return true;
}

///////////////////////////////////////////////////

bool Resume( HSTREAM stream )
{
	BASS_ChannelPlay( stream, false );
	return true;
}

///////////////////////////////////////////////////

bool Stop( HSTREAM stream )
{	
	BASS_ChannelStop( (HMUSIC)stream );
	BASS_MusicFree( (HMUSIC)stream );
	return true;
}

///////////////////////////////////////////////////

int GetDuration( HSTREAM stream )
{	
	QWORD bytelength = BASS_ChannelGetLength( stream );
	float secs = BASS_ChannelBytes2Seconds( (HMUSIC)stream, bytelength ); 
	secs *= 1000; // to ms
	return (int)secs;
}

///////////////////////////////////////////////////

int GetTime( HSTREAM stream )
{
	QWORD pos = BASS_ChannelGetPosition( stream );
	float secs = BASS_ChannelBytes2Seconds( stream, pos );
	secs *= 1000.0f;
	return (int)secs;
}

///////////////////////////////////////////////////

bool IsActive( HSTREAM stream )
{
	DWORD state = BASS_ChannelIsActive( (HMUSIC)stream );
	if ( state == BASS_ACTIVE_PLAYING || state == BASS_ACTIVE_PAUSED )
		return true;

	return false;
}

///////////////////////////////////////////////////

HSTREAM LoadFile( const musik::Core::String& filename )
{
	HMUSIC load = NULL;
	musik::Core::Filename fn( filename );

	// standard BASS MOD formats
	if ( CanPlay(filename) )
	{
		load = BASS_MusicLoad(
			FALSE,
			filename.c_str(),
			0,
			0,
			BASS_MUSIC_AUTOFREE | BASS_UNICODE | BASS_MUSIC_POSRESET | BASS_MUSIC_PRESCAN,
			0 );
	}

	if ( load )
		BASS_ChannelPreBuf( load, 500);
	
	return load;
}

///////////////////////////////////////////////////

bool WriteTag( const musik::Core::SongInfo& info )
{
	// MOD files don't have any tags, so this does nothing
	return true;
}

///////////////////////////////////////////////////

bool ReadTag( const musik::Core::String& fn, musik::Core::SongInfo& target )
{
	// Set the filename and format
	musik::Core::Filename mfn( fn );
	target.SetFilename(fn);
	target.SetFormat(mfn.GetExtension());

	// load the music
	HMUSIC music;

	music = BASS_MusicLoad(
		FALSE,
		fn.c_str(),
		0,
		0,
		BASS_UNICODE | BASS_MUSIC_PRESCAN | BASS_MUSIC_NOSAMPLE,
		0 );

	if (music)
	{
		// get the title
		char *name =(char *)BASS_ChannelGetTags(music, BASS_TAG_MUSIC_NAME);

		if (name)
		{
			target.SetTitle(musik::Core::utf8to16(name));
		}
		else
		{
			// no title found, use the filename
			target.SetTitle(mfn.GetJustFilenameNoExt());
		}

		// set the duration
		target.SetDuration(musik::Core::IntToString(GetDuration(music)));

		// unload the music
		BASS_MusicFree(music);

		return true;
	}

	return false;
}

///////////////////////////////////////////////////

