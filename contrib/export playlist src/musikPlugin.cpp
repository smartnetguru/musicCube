///////////////////////////////////////////////////
// 
// Info:
//
//	...
//
// Credits:
//
//	musikCube by Casey Langen
//	Plugin by Martin Dittus
//	HTML Support by Rajiv Makhijani
//	musikCube 1.0 compatibility, simple M3U and XSPF Support by Julian Cromarty
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include <fstream>

#include "musikPlugin.h"
#include "musikPluginDlg.h"

// plugin properties
#include "musikPluginProps.h"


///////////////////////////////////////////////////

// plugin info
musik::Core::String g_Name = STR_PLUGIN_NAME;
musik::Core::String g_FullName = STR_PLUGIN_NAME _T(" ") STR_PLUGIN_VERSION;
musik::Core::String g_Description = _T( "Exports playlists in PLS, M3U, HTML and XSPF formats" );



// *** THESE NEED TO BE SET BY YOU ***

bool g_CanConfigure = false;
bool g_CanAbout = true;
bool g_CanStop = false;
bool g_InSources = true;		// visible in the sources panel?
bool g_WantsPrefs = false;		// musikCube can take care of loading and saving preferences for you

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

// DllMain is already provided by MFC
/*
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
*/

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

// overriden default functions

// determines if all prerequisites for executing the plugin are met
MUSIKPLUGIN_API bool CanExecute()
{
	// TODO: check playlist type
	/*switch (g_VisiblePlaylist->GetType())
	{
	case musik::Core::MUSIK_PLAYLIST_TYPE_UNKNOWN:
    case musik::Core::MUSIK_PLAYLIST_TYPE_CDPLAYER:
    case musik::Core::MUSIK_PLAYLIST_TYPE_REMOVABLE:
		break;
	}
	*/

	// are there tracks in the visible playlist?
	if (g_VisiblePlaylist->GetCount() > 0)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////

// tools

string CreateExtM3u(musik::Core::Playlist *pPlaylist)
{
	ostringstream oss;
	musik::Core::Song song;
	musik::Core::String strStreamFormat = _T("4");

	oss << "#EXTM3U\n";

	for (size_t i=0; i<pPlaylist->GetCount(); i++)
	{
		oss << "#EXTINF:";

		song = pPlaylist->GetSong(i);

		if (song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_FORMAT).Equals(strStreamFormat))
			oss << "-1";
		else
			oss << song.GetDuration();
			
		oss << "," << 
			musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST)) << " - " <<
			musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_TITLE)) << "\n";
		oss << 
			musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_FILENAME)) << "\n";
	}
	return oss.str();
}

string CreateM3u(musik::Core::Playlist *pPlaylist)
{
	ostringstream oss;
	musik::Core::Song song;
	musik::Core::String strStreamFormat = _T("4");

	for (size_t i=0; i<pPlaylist->GetCount(); i++)
	{
		song = pPlaylist->GetSong(i);
		
		oss << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_FILENAME)) << "\n";
	}
	return oss.str();
}

string CreateHtml(musik::Core::Playlist *pPlaylist)
{
	ostringstream oss;
	musik::Core::Song song;
	musik::Core::String strStreamFormat = _T("4");

	oss << "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"
		<< "<html xmlns=\"http://www.w3.org/1999/xhtml\">"
		<< "<head>"
		<< "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\" />"
		<< "<title>musikCube Playlist</title>"
		<< "<style type=\"text/css\">"
		<< "<!--"
		<< "body,td,th {"
		<< "	font-size: 10px;"
		<< "	color: #000000;"
		<< "	font-family: Verdana, Arial, Helvetica, sans-serif;"
		<< "}"
		<< "-->"
		<< "</style></head>"
		<< "<body>"
		<< "<table border=\"0\" cellpadding=\"3\" cellspacing=\"0\">"
		<< "<tr>"
		<< "	<td><div align=\"left\"><strong>Title</strong></div></td>"
		<< "	<td><div align=\"left\"><strong>Artist</strong></div></td>"
		<< "	<td><div align=\"left\"><strong>Album</strong></div></td>"
		<< "</tr>";
	for (size_t i=0; i<pPlaylist->GetCount(); i++)
	{		
		oss << "<tr>";
		song = pPlaylist->GetSong(i);
		oss << "	<td>";
		oss << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_TITLE));
		oss	<< "	</td><td>";		
		oss << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST));
		oss	<< "	</td><td>";
		oss << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_ALBUM));
		oss	<< "</td></tr>";
	}

	oss << "</table></body></html>";
	return oss.str();
}

string CreatePls(musik::Core::Playlist *pPlaylist)
{
	ostringstream oss;
	musik::Core::Song song;
	musik::Core::String strStreamFormat = _T("4");

	oss << "[playlist]\n";

	for (size_t i=0; i<pPlaylist->GetCount(); i++)
	{
		song = pPlaylist->GetSong(i);

		oss << "File" << (i+1) << "=" << 
			musik::Core::utf16to8(pPlaylist->GetField(i, musik::Core::MUSIK_LIBRARY_TYPE_FILENAME)) << "\n";
		oss << "Title" << (i+1) << "=" <<
			musik::Core::utf16to8(pPlaylist->GetField(i, musik::Core::MUSIK_LIBRARY_TYPE_ARTIST)) << " - " <<
			musik::Core::utf16to8(pPlaylist->GetField(i, musik::Core::MUSIK_LIBRARY_TYPE_TITLE)) << "\n";

		oss << "Length" << (i+1) << "=";
		if (song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_FORMAT).Equals(strStreamFormat))
			oss << "-1\n";
		else
			oss << song.GetDuration() << "\n";
	}
	
	oss << "NumberOfEntries=" << pPlaylist->GetCount() << "\n" <<
		"Version=2\n";
		
	return oss.str();
}

string CreateXspf(musik::Core::Playlist *pPlaylist)
{
	ostringstream oss;
	musik::Core::Song song;
	musik::Core::String strStreamFormat = _T("4");
	musik::Core::String temp = "0";

	oss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		<< "<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\">\n"
		<< "<title>musikCube Playlist</title>\n"
		<< "<trackList>\n";
		
	for (size_t i=0; i<pPlaylist->GetCount(); i++)
	{		
		song = pPlaylist->GetSong(i);
		oss << "<track>\n";
        oss << "<title>" << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_TITLE)) << "</title>\n";
		oss << "<creator>" << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_ARTIST)) << "</creator>\n";
		oss << "<album>" << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_ALBUM)) << "</album>\n";
		if ((song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM)) != temp)
		{
			oss << "<trackNum>" << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_TRACKNUM)) << "</trackNum>\n";
		}
		oss << "<location>file://";
		oss << musik::Core::utf16to8(song.GetField(musik::Core::MUSIK_LIBRARY_TYPE_FILENAME));
		oss << "</location>\n</track>\n";
	}

	oss << "</trackList></playlist>";
	return oss.str();
}

///////////////////////////////////////////////////

// implement me

void Configure()
{
}

///////////////////////////////////////////////////

// implement me

void About()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd parent;
	parent.Attach(g_MainWnd); 

	CAboutDlg aboutDlg(g_Name, &parent);
	aboutDlg.SetPluginName(g_FullName);
	//aboutDlg.SetLink(g_WebsiteURL);
	aboutDlg.DoModal();
	
	parent.Detach();
}

///////////////////////////////////////////////////

// implement me

void Execute()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// parent window for dialogs
	CWnd parent;
	parent.Attach(g_MainWnd); 


	if (!CanExecute())
	{
		MessageBox( NULL, _T("Can't execute this plugin."), g_Name, MB_ICONINFORMATION );
		return;
	}

	// attempt to get the name of the current playlist
	musik::Core::PlaylistInfoArray pli;
	CString playlist;
	int id = g_VisiblePlaylist->GetID();
	
	switch (g_VisiblePlaylist->GetType())
	{
	case musik::Core::MUSIK_PLAYLIST_TYPE_STANDARD:
		g_Library->GetAllStdPlaylists(pli, false);
		break;

	case musik::Core::MUSIK_PLAYLIST_TYPE_DYNAMIC:
		g_Library->GetAllDynPlaylists(pli, false);
		break;

	default:
		break;
	}

	if (pli.size() != 0)
	{
		for (size_t i=0; i<pli.size(); i++)
		{
			if (pli[i].GetID() == id)
			{
				playlist = pli[i].GetName();
				break;
			}
		}
	}

	// show file dialog
	CFileDialog fileDlg(FALSE, _T("m3u"), playlist, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("Extended M3U Playlist (*.m3u)|*.m3u|Simple M3U Playlist (*.m3u)|*.m3u|PLS Playlist (*.pls)|*.pls|HTML (*.html)|*.html|XSPF (*.xml)|*.xml||"), &parent);

	if(fileDlg.DoModal() == IDOK)
	{
	// export playlist
		AfxGetApp()->DoWaitCursor(1);

		musik::Core::String pathName = fileDlg.GetPathName();
		//musik::Core::String fileName = fileDlg.GetFileName();

		ofstream f;
		f.open(musik::Core::utf16to8(pathName).c_str(), ios::out);
		if ( f.fail())
		{
			AfxGetApp()->DoWaitCursor(0);
			MessageBox( NULL, _T("Could not open the file for writing."), _T("Error"), MB_ICONINFORMATION );
			return;
		}
		if (fileDlg.GetOFN().nFilterIndex == 1)				// Extended m3u file
		{
			f << CreateExtM3u(g_VisiblePlaylist);
		}
		else if (fileDlg.GetOFN().nFilterIndex == 2)				// Simple m3u file
		{
			f << CreateM3u(g_VisiblePlaylist);
		}
		else if (fileDlg.GetOFN().nFilterIndex == 3)		// pls file
		{
			f << CreatePls(g_VisiblePlaylist);
		}
		else if (fileDlg.GetOFN().nFilterIndex == 4)		// html file
		{
			f << CreateHtml(g_VisiblePlaylist);
		}
		else if (fileDlg.GetOFN().nFilterIndex == 5)		// xspf file
		{
			f << CreateXspf(g_VisiblePlaylist);
		}
		else {
		}
		f.close();

		AfxGetApp()->DoWaitCursor(0);
	}
	
	// cleanup
	parent.Detach();
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

