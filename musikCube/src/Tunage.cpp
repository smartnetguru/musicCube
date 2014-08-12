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

#include "StdAfx.h"

#include <afxinet.h>

#include "musikCubeApp.h"
#include "Tunage.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

UINT TunageWorker(LPVOID pParam)
{
    CInternetSession m_Session;

    if (Globals::Preferences->GetTunageURL() != "") 
    {
        musik::Core::String url(Globals::Preferences->GetTunageURL());
        Tunage::ParseTags(url, true);

        try
        {
            delete m_Session.OpenURL(url);
        }
        catch (...)
        {
            
        }

        m_Session.Close();
    }

    return (UINT)1;
}

///////////////////////////////////////////////////
// Tunage
///////////////////////////////////////////////////

/*ctor*/ Tunage::Tunage()
{
}

/*dtor*/ Tunage::~Tunage()
{
}

void Tunage::Execute()
{
    if (Globals::Preferences->GetTunagePostURL())
        Execute_PostURL();

    if (Globals::Preferences->GetTunageRunApp())
        Execute_RunApp();

    if (Globals::Preferences->GetTunageWriteFile())
        Execute_WriteFile();
}

void Tunage::Execute_PostURL()
{    
    AfxBeginThread(TunageWorker, (LPVOID)NULL);
}

void Tunage::Execute_RunApp()
{
    // bail if empty
    if (Globals::Preferences->GetTunageCmdLine() == "")
        return;

    musik::Core::String str(Globals::Preferences->GetTunageCmdLine());
    ParseTags(str, false);

    WinExec(musik::Core::utf16to8(str).c_str(), SW_NORMAL);
}

void Tunage::Execute_WriteFile()
{
    // bail if empty
    if ((Globals::Preferences->GetTunageFilename() == "") || (Globals::Preferences->GetTunageFileLine() == ""))
        return;

    if (!Globals::Preferences->GetTunageAppendFile())
    {
        if ( musik::Core::Filename::FileExists(Globals::Preferences->GetTunageFilename()))
            remove(Globals::Preferences->GetTunageFilename().c_str());
    }

    CStdioFile out;
    CString fn = musik::Core::utf8to16(Globals::Preferences->GetTunageFilename()).c_str();
    if (!out.Open(fn, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText))
        return;


    musik::Core::String str(Globals::Preferences->GetTunageFileLine());
    ParseTags(str, false);

    if (Globals::Preferences->GetTunageAppendFile())
    {
        out.SeekToEnd();
        out.WriteString(str + "\n");
    }
    else
        out.WriteString(str);

    out.Close();
}

void Tunage::ParseTags(musik::Core::String& str, bool encode)
{    
    musik::Core::String artist = Globals::Player->GetPlaying()->GetArtist();
    if (encode)
        URLEncode(artist);
    str.Replace(_T("$ARTIST"), artist);

    musik::Core::String playlistpos;
    playlistpos.Format(_T("%d of %d"), Globals::Player->GetIndex(), Globals::Player->GetPlaylist()->GetCount());
    str.Replace(_T("$PLAYLISTPOS"), playlistpos);

    musik::Core::String album = Globals::Player->GetPlaying()->GetAlbum();
    if (encode)
        URLEncode(album);
    str.Replace(_T("$ALBUM"), album);
    
    musik::Core::String title = Globals::Player->GetPlaying()->GetTitle();
    if (encode)
        URLEncode(title);
    str.Replace(_T("$TITLE"), title);

    musik::Core::String genre = Globals::Player->GetPlaying()->GetGenre();
    if (encode)
        URLEncode(genre);
    str.Replace(_T("$GENRE"), genre);

    musik::Core::String name;
    name.Format(_T("%s - %s"), artist.c_str(), title.c_str());
    str.Replace(_T("$NAME"), name);

    int time = musik::Core::StringToInt(Globals::Player->GetPlaying()->GetDuration());
    musik::Core::String duration = musik::Core::Player::GetTimeStr(time);
    str.Replace(_T("$DURATION"), duration);

    musik::Core::String filename = Globals::Player->GetPlaying()->GetFilename();
    if (encode)
        URLEncode(filename);
    str.Replace(_T("$FILENAME"), filename);

    double filesize = musik::Core::StringToFloat(
        Globals::Player->GetPlaying()->GetFilesize());

    musik::Core::String strsize(_T("0.0 mb"));
    if (filesize < 0)
        strsize.Format(_T("net radio"));
    else if (filesize < 1024.0)
        strsize.Format(_T("%.2f b"), filesize);
    else if (filesize < (1024.0 * 1024.0))
        strsize.Format(_T("%.2f kb"), filesize / 1024.0);
    else if (filesize < (1024.0 * 1024.0 * 1024.0))
        strsize.Format(_T("%.2f mb"), filesize / 1024.0 / 1024.0);
    else if (filesize < (1024.0 * 1024.0 * 1024.0 * 1024.0))
        strsize.Format(_T("%.2f gb"), filesize / 1024.0 / 1024.0 / 1024.0);

    str.Replace(_T("$FILESIZE"), strsize);
    str.Replace(_T("$BITRATE"), Globals::Player->GetPlaying()->GetBitrate().c_str());
    str.Replace(_T("$TIMESPLAYED"), Globals::Player->GetPlaying()->GetTimesPlayed().c_str());
    str.Replace(_T("$TRACKNUM"), Globals::Player->GetPlaying()->GetTrackNum().c_str());
    str.Replace(_T("$RATING"), Globals::Player->GetPlaying()->GetRating().c_str());
}

void Tunage::URLEncode(musik::Core::String& str)
{
    // this is pretty much taken from Oddsock's Do Something plugin:
    // http://www.oddsock.org/tools/dosomething/
    str.Replace(_T("%"), _T("%25"));
    str.Replace(_T(";"), _T("%3B"));
    str.Replace(_T("/"), _T("%2F"));
    str.Replace(_T("?"), _T("%3F"));
    str.Replace(_T(":"), _T("%3A"));
    str.Replace(_T("@"), _T("%40"));
    str.Replace(_T("&"), _T("%26"));
    str.Replace(_T("="), _T("%3D"));
    str.Replace(_T("+"), _T("%2B"));
    str.Replace(_T(" "), _T("%20"));
    str.Replace(_T("\""), _T("%22"));
    str.Replace(_T("#"), _T("%23"));
    str.Replace(_T("<"), _T("%3C"));
    str.Replace(_T(">"), _T("%3E"));
    str.Replace(_T("!"), _T("%21"));
    str.Replace(_T("*"), _T("%2A"));
    str.Replace(_T("'"), _T("%27"));
    str.Replace(_T("("), _T("%28"));
    str.Replace(_T(")"), _T("%29"));
    str.Replace(_T(","), _T("%2C"));
}

///////////////////////////////////////////////////

} } // namespace musikCube