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

#include "PlaylistInfoView.h"
#include "musikCubeApp.h"
#include "PlaylistView.h"
#include "GraphicsUtility.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// PlaylistInfoWorker
///////////////////////////////////////////////////

void PlaylistInfoViewWorker::run()
{
    // set flags
    m_Stop = false;
    m_Active = true;
    m_Finished = false;

    // this is the loop.. set it to true from 
    // anywhere and it will abort. threads run
    // very passivly, as to not interfere with 
    // the interface
    while (!m_Stop)
    {
        if (Globals::Preferences->GetPlaylistInfoVizStyle() != PLAYLIST_INFO_VIZ_STYLE_NONE)
        {
            m_Parent->Invalidate();
        }

        Sleep(100);
    }

    // be sure to flag as finished
    m_Finished = true;
}

///////////////////////////////////////////////////
// PlaylistInfoView
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(PlaylistInfoView, CWnd)

BEGIN_MESSAGE_MAP(PlaylistInfoView, CWnd)
    ON_WM_PAINT()
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*ctor*/ PlaylistInfoView::PlaylistInfoView(PlaylistView* parent)
{
    m_ListCtrl = parent;

    m_InfoWorker = NULL;
    m_hBGBitmap = NULL;
}

/*dtor*/ PlaylistInfoView::~PlaylistInfoView()
{
    if (m_hBGBitmap)
        DeleteObject (m_hBGBitmap);

    if (m_ListCtrl)
        m_ListCtrl->SetInfoCtrl(NULL);
    
    if (m_InfoWorker)
    {
        m_InfoWorker->Stop();
        m_InfoWorker->join();
        delete m_InfoWorker;
        m_InfoWorker = NULL;
    }
}

void PlaylistInfoView::OnPaint()
{
    CPaintDC dc(this);

    CRect rect;
    GetClientRect(&rect);
    CRect innerrect(rect);
    innerrect.DeflateRect(1, 1, 1, 1);
    CMemDC memDC(&dc, &rect);

    memDC.Draw3dRect(rect, Globals::Preferences->GetBtnShadowColor(), Globals::Preferences->GetBtnHilightColor());
    memDC.FillSolidRect(innerrect, Globals::Preferences->GetBtnFaceColor());    

    int style = Globals::Preferences->GetPlaylistInfoVizStyle();
    if (Globals::Player->IsPlaying() && !Globals::Player->IsPaused())
    {
        // draw the bitchin' background
        switch (Globals::Preferences->GetPlaylistInfoVizStyle())
        {
        case PLAYLIST_INFO_VIZ_STYLE_EQ:
            DrawEQ(memDC.GetSafeHdc());
            break;
        }
    }

    // put the text over it
    if (Globals::Preferences->PlaylistInfoVisible())
    {
        // select the proper font
        CFont* oldfont = memDC.SelectObject(&m_Font);

        SetTextAlign(memDC.GetSafeHdc(), TA_CENTER);
        SetBkMode(memDC.GetSafeHdc(), TRANSPARENT); 
        SetTextColor(memDC.GetSafeHdc(), Globals::Preferences->GetBtnTextColor());
        memDC.TextOut(rect.Width() / 2, 3, m_strInfo.c_str());

        // get the old font back
        memDC.SelectObject(oldfont);
    }
}

int PlaylistInfoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    
    m_Font.CreateFont(-10,0,0,0,FW_BOLD,FALSE,FALSE,
            0,ANSI_CHARSET,OUT_TT_PRECIS,
            CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
            FF_SWISS, _T("Tahoma"));
    
    CDC* pDC = GetDC();
    pDC->SetBkColor(GetSysColor(COLOR_BTNHILIGHT));
    ReleaseDC(pDC);

    BITMAPINFO bminfo;    
    ZeroMemory (&bminfo, sizeof (BITMAPINFO));
    bminfo.bmiHeader.biWidth  = VIZ_WIDTH;
    bminfo.bmiHeader.biHeight = VIZ_HEIGHT;
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;
    bminfo.bmiHeader.biXPelsPerMeter = 0;
    bminfo.bmiHeader.biYPelsPerMeter = 0;
    bminfo.bmiHeader.biClrUsed = 0;
    bminfo.bmiHeader.biClrImportant = 0;
    bminfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
    bminfo.bmiHeader.biSizeImage = VIZ_WIDTH * VIZ_HEIGHT * 4;
    
    m_hBGBitmap = CreateDIBSection (NULL, &bminfo, DIB_RGB_COLORS, (void **)&m_pBGBitmapBits, NULL, NULL); 

    // startup task
    m_InfoWorker = new PlaylistInfoViewWorker;
    m_InfoWorker->m_Parent = this;
    m_InfoWorker->start();

    return 0;
}

BOOL PlaylistInfoView::OnEraseBkgnd(CDC* pDC)
{
    return false;
}



void PlaylistInfoView::DrawEQ(HDC hdc)
{
    // draws a visualization of the current sound spectrum to a small bitmap, then stretches
    // it to the size of the control to paint it

    float spectrum[512];
    Globals::Player->GetSpectrum(spectrum);

    if (!spectrum)
        return;
    

    COLORREF clrBack = Globals::Preferences->GetBtnFaceColor();

    // swap channels when drawing to a DIB
    clrBack = RGB(GetBValue(clrBack), GetGValue(clrBack), GetRValue(clrBack));

    // swap channels when drawing to a DIB
    COLORREF colorEQ = GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), -30);
    colorEQ = RGB(GetBValue(colorEQ), GetGValue(colorEQ), GetRValue(colorEQ));

    // fill with the proper bg color    
    for (int n = 0; n < (VIZ_WIDTH * VIZ_HEIGHT)-1; n++)
    {
        m_pBGBitmapBits[n] = clrBack;
    }

    // now draw the viz directly to the bitmap bits

    // we're only going to work with the lower frequencies (0 - 6.5khz) because the higher
    // ones don't really show much
    for (int x = 0; x < VIZ_WIDTH-1; x++)
    {
        // get the "height" of the sample
        // seeing if doing it without the min stops the odd crash
        //int sample_y = min((int)(spectrum[x] * 4.0f * (float)VIZ_HEIGHT), (VIZ_HEIGHT - 1));
        int sample_y = (int)(spectrum[x] * 4.0f * (float)VIZ_HEIGHT);
        //
        if (sample_y > (VIZ_HEIGHT-1))
        {
            sample_y = VIZ_HEIGHT-1;
        }

        for (int y = 0; y < sample_y; y++)
        {
            // every other pixel for fun effects
            if (x % 2)
            {
                if (y % 2)
                {
                    m_pBGBitmapBits[(VIZ_WIDTH * y) + x] = colorEQ;
                }
            }
        }
    }

    CRect rcClient;
    GetClientRect(&rcClient);

    // create a DC to work with the viz bitmap
    HDC hSrcDC;
    hSrcDC = CreateCompatibleDC(hdc);
    HGDIOBJ hOldBmp = ::SelectObject(hSrcDC, m_hBGBitmap);
    
    // and draw it
    ::StretchBlt (hdc,
                1,                    // destination rect
                1,                    // '
                rcClient.right-2,    // '
                rcClient.bottom-2,    // '
                hSrcDC,
                0,                    // source rect
                0,                    // '
                VIZ_WIDTH-1,        // '
                VIZ_HEIGHT-1,        // '
                SRCCOPY);

    // restore stuff
    ::SelectObject (hSrcDC, hOldBmp);
    ::DeleteDC(hSrcDC);
}

void PlaylistInfoView::UpdateInfo()
{
    if (m_ListCtrl->GetPlaylist() && m_ListCtrl->GetPlaylist()->GetCount() != 0)
    {
        m_strInfo.Format(
            _T("number of songs: %d    (calculating...)"),
            m_ListCtrl->GetPlaylist()->GetCount());

        m_ListCtrl->RedrawWindow();

        size_t runtime = m_ListCtrl->GetPlaylist()->GetTotalTime();
        double totsize = m_ListCtrl->GetPlaylist()->GetTotalSize();
        
        CTimeSpan span(0, 0, 0, runtime);
        CString strtime;
        if (runtime > 172800)
            strtime = span.Format(_T("%D days, %H:%M:%S"));
        else if (runtime > 86400)
            strtime = span.Format(_T("%D day, %H:%M:%S"));
        else
            strtime = span.Format(_T("%H:%M:%S"));
    
        CString strsize(_T("0.0 MB"));
        if (totsize < 1024.0)
            strsize.Format(_T("%.2f B"), totsize);
        else if (totsize < (1024.0 * 1024.0))
            strsize.Format(_T("%.2f KB"), totsize / 1024.0);
        else if (totsize < (1024.0 * 1024.0 * 1024.0))
            strsize.Format(_T("%.2f MB"), totsize / 1024.0 / 1024.0);
        else if (totsize < (1024.0 * 1024.0 * 1024.0 * 1024.0))
            strsize.Format(_T("%.2f GB"), totsize / 1024.0 / 1024.0 / 1024.0);

        m_strInfo.Format(
            _T("number of songs: %d    running time: %s    size: %s"), 
            m_ListCtrl->GetPlaylist()->GetCount(), 
            strtime, 
            strsize);
    }
    else
        m_strInfo = _T("empty playlist");

    Invalidate();
}

///////////////////////////////////////////////////

} } // namespace musik::Cube
