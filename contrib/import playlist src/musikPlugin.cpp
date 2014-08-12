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
musik::Core::String g_Description = _T( "Imports M3U, PLS and XSPF playlist files.");


// link to website that lists the recommendations
musik::Core::String g_WebsiteURL = STR_PLUGIN_WEBSITE;

// parent window
//CWnd g_MainCWnd;

// global variables
musik::Core::BatchAddTask* g_BatchTask;
musik::Core::Playlist g_NewPlaylist;		// sent to BatchAddTask

bool g_IsTaskRunning = false;



// *** THESE NEED TO BE SET BY YOU ***

bool g_CanConfigure = false;
bool g_CanAbout = true;
//bool g_CanExecute = true;
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
bool CanExecute()
{
	// only if there is not a task in progress
	//if (g_IsTaskRunning)
	//	return false;
	//else
		return true;
}

///////////////////////////////////////////////////

// custom Functor, passed to the BatchAddTask
class MyFunctor : public Functor 
{
	// update progress display
	void OnTaskProgress( size_t progress, musik::Core::Task* task_addr )
	{
		int WM_TASK_PROGRESS = RegisterWindowMessage( _T( "TASK_PROGRESS" ) );
		::PostMessage( g_MainWnd, WM_TASK_PROGRESS, (WPARAM)progress, NULL );
	}

	void OnTaskEnd( musik::Core::Task* task_addr )
	{
		// our task has finished
		g_IsTaskRunning = false;

		// notify parent
		int WM_TASK_END = RegisterWindowMessage( _T( "TASK_END" ) );
		::PostMessage( g_MainWnd, WM_TASK_END, (WPARAM)task_addr, NULL );
	} 

	bool VerifyPlaylist( void* pl_addr){
		// we already know that all playlists involved here are valid, but let's check anyway:
#ifdef CREATE_PLAYLIST
		if (pl_addr == (void*)&g_NewPlaylist)
			return true;
		else
			return false;
#else
		return true;
#endif
	}
};

MyFunctor g_MyFunctor;


///////////////////////////////////////////////////

// tools

// here it is: the primitive but tolerant file parser ;)
void ReadPlaylist(istream &is, musik::Core::StringArray *pFiles, musik::Core::String filename)
{
	string str;
	musik::Core::String line;
	musik::Core::String::size_type idx;
	bool isPls = false;						// is the playlist in PLS format? otherwise it's M3U or XSPF
	bool isXspf = false;						// is the playlist in XSPF format? otherwise it's M3U

	musik::Core::String extension;
	for (unsigned int i = 0; i < filename.size(); i++)
	{

		if (filename[i] == '.')
		{
			i++;
			if (filename[i] == 'x')
			{
				isXspf = true;
			}
			else if (filename[i] == 'p')
			{
				isPls = true;
			};
			break;
		};
	};



	// 1. read first line to guess file type (we ignore the file extension and 
	// try to guess from the content instead)
	getline(is, str);
	line = musik::Core::utf8to16(str);		// use musikCube::String (because we like the convenience)
	line.ToLower();

	if (line.Find(_T("?<?xml version=\"1.0\" encoding=\"UTF-8\"?>")) == 0)		// XSPF header?
	{
		getline(is, str);
		isXspf = true;
	}
	else if (line.Find(_T("[playlist]")) == 0)	// pls header?
	{
		isPls = true;
	}
	else if (line[0] == '#')				// comment? (possibly from an m3u file)
	{
		// do nothing
	}
	else									// "old style" m3u: just a list of files
	{
		if (line.size() != 0)
			pFiles->push_back(line);
	}

	// 2. read rest of file
	while (getline(is, str))
	{
		line = musik::Core::utf8to16(str);
		line.ToLower();

		//XSPF
		if (isXspf)
		{
			// look for lines that start like this: "file1=...", "File43=...", etc
			idx = line.find(_T("<location>"));
			if ((idx != string::npos) &&			// the line has a "=" in it
				(line.size() > idx+1))				// and does not end after the "="
			{
				if (line.find(_T("<location>")) == 0) 		// the line starts with "file"
				{
					// extract and add filename
					line.erase((line.size()-11));
					pFiles->push_back(line.substr(idx + 10));
				}
			}
		}
		// pls
		else if (isPls)
		{
			// look for lines that start like this: "file1=...", "File43=...", etc
			idx = line.find(_T("="));
			if ((idx != string::npos) &&			// the line has a "=" in it
				(line.size() > idx+1))				// and does not end after the "="
			{
				if (line.find(_T("file")) == 0) 		// the line starts with "file"
				{
					// extract and add filename
					pFiles->push_back(line.substr(idx + 1));
				}
			}
		}
		// m3u
		else
		{
			// add every line that is not a comment
			if (line[0] != '#')						// not a comment
				pFiles->push_back(line);
		}
	}
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
	ostringstream oss;
	musik::Core::Playlist p;
/*
	oss << "new: " << g_NewPlaylist.GetCount() << "\n";
	g_Library->GetStdPlaylist(0, p, true);
	oss << "0: " << p.GetCount() << "\n";
	g_Library->GetStdPlaylist(1, p, true);
	oss << "1: " << p.GetCount() << "\n";
	g_Library->GetStdPlaylist(2, p, true);
	oss << "2: " << p.GetCount() << "\n";
	g_Library->GetStdPlaylist(3, p, true);
	oss << "3: " << p.GetCount() << "\n";
	g_Library->GetStdPlaylist(4, p, true);
	oss << "4: " << p.GetCount() << "\n";
	g_Library->GetStdPlaylist(5, p, true);
	oss << "5: " << p.GetCount() << "\n";

	CString t = oss.str().c_str();
	MessageBox( NULL, t, g_Name, MB_ICONINFORMATION );
*/
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CWnd mainCWnd;
	mainCWnd.Attach(g_MainWnd); 

	CAboutDlg aboutDlg(g_Name, &mainCWnd);
	aboutDlg.SetPluginName(g_FullName);
	aboutDlg.SetLink(g_WebsiteURL);
	aboutDlg.DoModal();
	
	mainCWnd.Detach();
}

///////////////////////////////////////////////////

// implement me

void Execute()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// parent window for dialogs
	CWnd mainCWnd;
	mainCWnd.Attach(g_MainWnd); 


	// just to make shure
	if (!CanExecute())
	{
		MessageBox( NULL, _T("Can't execute this plugin."), g_Name, MB_ICONINFORMATION );
		return;
	}

	//CMainDlg mainDlg(g_Name, &g_MainCWnd);
	//mainDlg.DoModal();

	CFileDialog fileDlg(TRUE, _T("m3u"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, 
		_T("Playlist Files (*.m3u, *.pls, *.xml)|*.m3u;*.pls;*.xml||"), &mainCWnd);

	INT_PTR result = fileDlg.DoModal();

	if(result == IDOK)
	{
		AfxGetApp()->DoWaitCursor(1);

		// playlist file
		musik::Core::String fileName = fileDlg.GetFileName();		

		// list of files/streams in playlist, sent to BatchAddTask, 
		// and to be deleted by BatchAddTask
		musik::Core::StringArray *pFiles = new musik::Core::StringArray();


	// read playlist file
		ifstream f;

		f.open(musik::Core::utf16to8(fileName).c_str(), ios::in);
		if ( f.fail())
		{
			AfxGetApp()->DoWaitCursor(0);
			MessageBox( NULL, _T("Could not read the file."), _T("Error"), MB_ICONINFORMATION );
			return;
		}

		ReadPlaylist(f, pFiles, fileName);

		f.close();


#ifdef CREATE_PLAYLIST
	// create new musikCube playlist
		// get index of last standard playlist
		musik::Core::PlaylistInfoArray plInfo;
		int plPos = 0;
		if (g_Library->GetAllStdPlaylists(plInfo, true) == musik::Core::MUSIK_LIBRARY_OK)
            plPos = plInfo.size() - 1;
	
		// only create playlist, add files later
		musik::Core::Filename fn(fileName);
		musik::Core::StringArray noFiles;
		//int plID = g_Library->CreateStdPlaylist(fn.GetJustFilenameNoExt(), *pFiles, plPos);
		int plID = g_Library->CreateStdPlaylist(fn.GetJustFilenameNoExt(), noFiles, plPos);
		g_Library->GetStdPlaylist(plID, g_NewPlaylist, true); // TODO: check retcode

		// request playlist sources UI update
		int WM_SOURCESRESET = RegisterWindowMessage( _T( "SOURCESRESET" ) );
		//::PostMessage( g_MainWnd, WM_CMD, NULL, NULL );
		mainCWnd.PostMessage( WM_SOURCESRESET, NULL );
#endif
		

	// import files
		if ( pFiles->size() > 0 )
		{

#ifdef CREATE_PLAYLIST
			// add to new playlist
			musik::Core::BatchAdd* params = new musik::Core::BatchAdd(
				pFiles, 
                "",			// directories
				&g_NewPlaylist,	// playlist
				g_Library, 
				NULL,		// player
				&g_MyFunctor,	// functor
				true,		// update playlist?
				false,		// add to player?
				true);		// delete file list?
#else
			// add to "now playing"
			musik::Core::BatchAdd* params = new musik::Core::BatchAdd(
				pFiles, 
                "",			// directories
				NULL,		// playlist
				g_Library, 
				g_Player, 	// player
				&g_MyFunctor,	// functor
				false,		// update playlist?
				true,		// add to player?
				true);		// delete file list?
#endif
			// spawn g_BatchTask
			g_IsTaskRunning = true;
			g_BatchTask = new musik::Core::BatchAddTask;
			g_BatchTask->m_Params = params;
			g_Library->QueueTask(g_BatchTask);
		}

		AfxGetApp()->DoWaitCursor(0);
	}
	
	// cleanup
	mainCWnd.Detach();
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
	//g_MainCWnd.Attach(g_MainWnd);
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

	//g_MainCWnd.Detach();
}

///////////////////////////////////////////////////

void OnSources()
{
}

///////////////////////////////////////////////////

