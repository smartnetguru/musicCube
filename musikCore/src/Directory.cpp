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

#include "stdafx.h"
#include <musikCore/Directory.h>
#include <musikCore/Filename.h>
#include <musikCore/Player.h>

///////////////////////////////////////////////////

using namespace musik::Core;

///////////////////////////////////////////////////

#if defined(WIN32)

void Directory::OpenDir(String dir, StringArray* target)
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
        String fn, temp;
        Filename MFN;
        do
        {
            if (lpFindFileData.cFileName[0] != '.')
            {
                // file is a directory
                if (lpFindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    fn = dir.Left(dir.GetLength() - 3);    // remove *.* from full path
                    fn += lpFindFileData.cFileName;            // append to make new path

                    if (fn.Left(0) != _T("\\"))
                        fn += _T("\\*.*");                    // add \\*.* to the end of the new path
                    else
                        fn += _T("*.*");                        // add *.* to the end of the new path

                    Directory::OpenDir(fn, target);
                }

                // file is a file...
                else
                {
                    fn = dir.Left(dir.GetLength() - 3);    // remove *.* from full path...
                    fn += lpFindFileData.cFileName;            // append filename
                    
                    MFN.SetFilename(fn);
                    for (size_t i = 0; i < musik::Core::g_PlayerFormats.size(); i++)
                    {
                        if (MFN.GetExtension() == musik::Core::g_PlayerFormats.at(i))
                        {
                            target->push_back(fn);
                            break;
                        }

                    }
                }
            }

        }
        while (FindNextFile(hDir, &lpFindFileData));

        FindClose(hDir);
    }
}

#else

void Directory::OpenDir(String dir, StringArray* target)
{
    if (dir.IsEmpty() || !target)
        return;

    DIR* pDir = opendir (utf16to8(dir).c_str());
    if (!pDir) 
        return;

    dirent* pEntry;

    String fn;
    Filename MFN;
    while ((pEntry = readdir (pDir))) 
    {
        // got a directory, so recurse...
        if (DT_DIR & pEntry->d_type && wcscmp(utf8to16(pEntry->d_name).c_str(), _T(".")) && wcscmp(utf8to16(pEntry->d_name).c_str(), _T(".."))) 
        {
            String sSubDir;
            if (dir == _T("/"))
                sSubDir = dir + String(pEntry->d_name);
            else
                sSubDir = dir + String (_T("/")) + String (pEntry->d_name);

            OpenDir (sSubDir, target);
        }

        // not a directory, so check file attributes
        else
        {
            if (dir == _T("/"))
                fn = _T("/") + String(pEntry->d_name);    
            else
                fn = dir + _T("/") + String(pEntry->d_name);

            MFN.SetFilename(fn);
            if (MFN.GetExtension() == _T("mp3") || MFN.GetExtension() == _T("ogg"))
                target->push_back(fn);
        }
    }

    closedir(pDir);
}

///////////////////////////////////////////////////

#endif

///////////////////////////////////////////////////
