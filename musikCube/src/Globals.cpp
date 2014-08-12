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

#include "Globals.h"
#include "musikCubeApp.h"

///////////////////////////////////////////////////

namespace Globals {

///////////////////////////////////////////////////

musik::Core::Player*        Player = NULL;
musik::Core::Library*       Library = NULL;
musik::Core::Library*       MainLibrary = NULL;
musik::Core::Library*       Collected = NULL;
musik::Cube::Preferences*   Preferences = NULL;
MainFrameFunctor*           Functor = NULL;
bool                        DrawGraphics = true;
MainFramePluginList         Plugins;

///////////////////////////////////////////////////

void FindPlugins(musik::Core::String dir, musik::Core::StringArray* target)
{
    // stopping conditions
    if (dir.IsEmpty() || !target)
        return;

    // find the first file in the directory...
    WIN32_FIND_DATA lpFindFileData;
    HANDLE hDir = FindFirstFile(dir.c_str(), &lpFindFileData);

    // assure the first file exists...
    if (hDir != INVALID_HANDLE_VALUE)
    {
        musik::Core::String fn, temp;
        musik::Core::Filename MFN;
        do
        {
            if (lpFindFileData.cFileName[0] != '.')
            {
                // file is a directory
                if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    fn = dir.Left(dir.GetLength() - 3);     // remove *.* from full path
                    fn += lpFindFileData.cFileName;         // append to make new path

                    if (fn.Left(0) != _T("\\"))
                        fn += _T("\\*.*");                  // add \\*.* to the end of the new path
                    else
                        fn += _T("*.*");                    // add *.* to the end of the new path

                    FindPlugins(fn, target);
                }

                // file is a file...
                else
                {
                    fn = dir.Left(dir.GetLength() - 3);     // remove *.* from full path...
                    fn += lpFindFileData.cFileName;         // append filename

                    MFN.SetFilename(fn);
                    if (MFN.GetExtension() == _T("dll"))
                        target->push_back(fn);
                }
            }

        }
        while (FindNextFile(hDir, &lpFindFileData));

        FindClose(hDir);
    }
}

void Globals::SetMainLibrary(musik::Core::Library* library)
{
    Globals::Library = library;

    // make sure the plugins reflect the active library
    for (size_t i = 0; i < Globals::Plugins.size(); i++)
    {
        Globals::Plugins.at(i).SetPluginLibrary(library);
    }
}

void Globals::SynchronizeDirs(musik::Core::Library* library)
{
    // tasks to add new files
    musik::Core::StringArray synchs;
    library->GetAllPaths(synchs, false);
    for (size_t i = 0; i < synchs.size(); i++)
    {
        musik::Core::StringArray* files = new musik::Core::StringArray();

        // setup params
        musik::Core::BatchAdd* params = new musik::Core::BatchAdd(
            files, 
            synchs.at(i) + _T("*.*"),
            NULL, 
            library, 
            NULL, 
            Globals::Functor, 
            0, 
            0, 
            1);

        // setup and start task
        musik::Core::BatchAddTask* task = new musik::Core::BatchAddTask;
        task->m_Params = params;
        library->QueueTask(task);
    }

    // task to remove obselete entries
    musik::Core::RemoveOld* params = new musik::Core::RemoveOld(
        library, 
        Globals::Functor);

    // spawn it
    musik::Core::RemoveOldTask* task = new musik::Core::RemoveOldTask();
    task->m_Params = params;
    library->QueueTask(task);
}

void Globals::LoadMainFramePlugins()
{
    // make sure plugin directory exists
    musik::Core::String path = musik::Cube::musikCubeApp::GetWorkingDir() + _T("\\plugins\\");
    musik::Core::Filename::RecurseMkDir(path.GetBuffer());

    // scan path
    path = musik::Cube::musikCubeApp::GetWorkingDir() + _T("\\plugins\\*.*");
    musik::Core::StringArray dllfiles;
    FindPlugins(path, &dllfiles);

    size_t count = 0;
    for (size_t i = 0; i < dllfiles.size(); i++)
    {
        HMODULE mod = LoadLibrary(dllfiles.at(i));

        MainFramePlugin plugin;

        plugin.m_Revision = 0;
        plugin.Configure = (MainFramePlugin::CONFIGURE)GetProcAddress(mod, "Configure");
        plugin.SetPluginLibrary = (MainFramePlugin::SETPLUGINLIBRARY)GetProcAddress(mod, "SetPluginLibrary");
        plugin.Initialize = (MainFramePlugin::INITIALIZE)GetProcAddress(mod, "Initialize");
        plugin.Unload = (MainFramePlugin::UNLOAD)GetProcAddress(mod, "Unload");
        plugin.Execute = (MainFramePlugin::EXECUTE)GetProcAddress(mod, "Execute");
        plugin.About = (MainFramePlugin::ABOUT)GetProcAddress(mod, "About");
        plugin.SetPluginInstance = (MainFramePlugin::SETPLUGININSTANCE)GetProcAddress(mod, "SetPluginInstance");
        plugin.GetPluginInstance = (MainFramePlugin::GETPLUGININSTANCE)GetProcAddress(mod, "GetPluginInstance");
        plugin.GetPluginName = (MainFramePlugin::GETPLUGINNAME)GetProcAddress(mod, "GetPluginName");
        plugin.GetPluginDescription = (MainFramePlugin::GETPLUGINDESCRIPTION)GetProcAddress(mod, "GetPluginDescription");
        plugin.Stop = (MainFramePlugin::STOP)GetProcAddress(mod, "Stop");
        plugin.GetPluginFunctor = (MainFramePlugin::GETPLUGINFUNCTOR)GetProcAddress(mod, "GetPluginFunctor");
        plugin.CanConfigure = (MainFramePlugin::CANCONFIGURE)GetProcAddress(mod, "CanConfigure");
        plugin.CanAbout = (MainFramePlugin::CANABOUT)GetProcAddress(mod, "CanAbout");
        plugin.CanExecute = (MainFramePlugin::CANCONFIGURE)GetProcAddress(mod, "CanExecute");
        plugin.CanStop = (MainFramePlugin::CANSTOP)GetProcAddress(mod, "CanStop");
        plugin.SetVisiblePlaylist = (MainFramePlugin::SETVISIBLEPLAYLIST)GetProcAddress(mod, "SetVisiblePlaylist");
        plugin.OnSources = (MainFramePlugin::ONSOURCES)GetProcAddress(mod, "OnSources");
        plugin.InSources = (MainFramePlugin::INSOURCES)GetProcAddress(mod, "InSources");
        plugin.WantsPrefs = (MainFramePlugin::WANTSPREFS)GetProcAddress(mod, "WantsPrefs");
        plugin.SetPrefs = (MainFramePlugin::SETPREFS)GetProcAddress(mod, "SetPrefs");
        plugin.GetPluginRevision = (MainFramePlugin::GETPLUGINREVISION)GetProcAddress(mod, "GetPluginRevision");

        if (plugin.Configure && plugin.SetPluginLibrary && plugin.Initialize && plugin.Unload &&
             plugin.Execute && plugin.About && plugin.SetPluginInstance && plugin.Stop && 
             plugin.GetPluginInstance && plugin.GetPluginName && plugin.GetPluginDescription && plugin.GetPluginFunctor &&
             plugin.CanConfigure && plugin.CanAbout && plugin.CanExecute && plugin.CanStop &&
             plugin.SetVisiblePlaylist && plugin.OnSources && plugin.InSources && plugin.WantsPrefs && plugin.SetPrefs)
        {
            plugin.Initialize(Globals::Library, Globals::Player, AfxGetApp()->GetMainWnd()->GetSafeHwnd());
            plugin.SetPluginInstance(mod);

            if (! (&plugin.GetPluginRevision == NULL))
                plugin.SetRevision(0);
            else
                plugin.SetRevision(plugin.GetPluginRevision());

            // this plugin wants to use preferences, so get them loaded.
            if (plugin.WantsPrefs())
            {
                musik::Core::Filename MFN(dllfiles.at(i));
                musik::Core::String inifn = MFN.GetPath();
                inifn += MFN.GetJustFilenameNoExt();
                inifn += _T(".ini");

                CIniFile* pini = new CIniFile(musik::Core::utf16to8(inifn, false));
                plugin.SetPrefs(pini);
            }

            Globals::Plugins.push_back(plugin);
        }
        else
            FreeLibrary(mod);
    }
}

void Globals::UnloadMainFramePlugins()
{
    for (size_t i = 0; i < Globals::Plugins.size(); i++)
    {
        Globals::Plugins.at(i).Stop();
        Globals::Plugins.at(i).Unload();    
        FreeLibrary(Globals::Plugins.at(i).GetPluginInstance());
    }

    Globals::Plugins.clear();
}

///////////////////////////////////////////////////

}   // namespace Globals