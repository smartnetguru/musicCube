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
#include "musikCubeApp.h"
#include "ListView.h"

#include <sys/stat.h>
#include <afxdisp.h>    // OLE stuff
#include <shlwapi.h>    // Shell functions (PathFindExtension() in this case)
#include <afxpriv.h>    // ANSI to/from Unicode conversion macros

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(ListView, CListCtrl)

BEGIN_MESSAGE_MAP(ListView, CListCtrl)
    ON_WM_NCCALCSIZE()
    ON_WM_DROPFILES()
    ON_WM_CREATE()
    ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

/*ctor*/ ListView::ListView()
{
    NCOverride = FALSE;    //False as default...
    Who = SB_BOTH;        //Default remove both...
}

/*ctor*/ ListView::~ListView()
{
}

int ListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CListCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_MouseTrack = false;

    return 0;
}

void ListView::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
    // TODO: Add your message handler code here and/or call default

    if(NCOverride==TRUE) //If the NCOverride is true we remove each time the CListCtrl calc its rect.
    {
          if(Who==SB_HORZ) ModifyStyle(WS_HSCROLL ,0,0); //Just by modifing the style we remove the scrollbar
          if(Who==SB_VERT) ModifyStyle(WS_VSCROLL,0,0);
          if(Who==SB_BOTH) ModifyStyle(WS_HSCROLL | WS_VSCROLL,0,0);
    }

    CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void ListView::HideScrollBars(int Type, int Which)
{
    /*
    if(Type==LCSB_CLIENTDATA) //This is the clientrect function
    {
        RECT ierect;
        int cxvs, cyvs;
        GetClientRect(&ierect); //Get client width and height

        cxvs = GetSystemMetrics (SM_CXVSCROLL); //Get the system metrics - VERT
        cyvs = GetSystemMetrics (SM_CYVSCROLL); //Get the system metrics - HORZ
        
        if(Which==SB_HORZ) cxvs=0; //Set VERT to zero when choosen HORZ
        if(Which==SB_VERT) cyvs=0; //Set HORZ to zero when choosen VERT

        //Here we set the position of the window to the clientrect + the size of the scrollbars
        SetWindowPos(NULL, ierect.left, ierect.top, ierect.right+cxvs, ierect.bottom+cyvs, SWP_NOMOVE | SWP_NOZORDER);

        //Her we modify the rect so the right part is subbed from the rect.
        if(Which==SB_BOTH||Which==SB_HORZ) ierect.bottom -= ierect.top;
        if(Which==SB_BOTH||Which==SB_VERT) ierect.right -= ierect.left;

        //Just to be safe that the left/top corner is 0...
        ierect.top = 0;
        ierect.left = 0;
        
        HRGN iehrgn = NULL; //This range is created base on which scrollbar that is going to be removed!

        //The -2 is probably a border of some kind that we also need to remove. I could not find any good
        //metrics that gave me an 2 as an answer. So insted we makes it static with -2.
        if(Which==SB_BOTH) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom-2);
        if(Which==SB_HORZ) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right, ierect.bottom-2);
        if(Which==SB_VERT) iehrgn=CreateRectRgn (ierect.left, ierect.top, ierect.right-2, ierect.bottom);
        
        //After the range has been made we add it...
        SetWindowRgn (iehrgn, TRUE);

        //Reset of NCOverride
        NCOverride=FALSE;
    }
    */

    //This is the NcCalcSize override
    if(Type == LCSB_NCOVERRIDE) 
    {
        NCOverride=TRUE;    //Set to true, so we run the code on each OnNcCalcSize.
        Who=Which;            //Selects which scrollbars to get hidden.
    }
}

void ListView::OnDragColumn(int source, int dest)
{
    // virtual
}

COLORREF ListView::GetModColor(COLORREF color, int pixel_change)
{
    int r, g, b;
    
    r = GetRValue(color);
    g = GetGValue(color);
    b = GetBValue(color);
    int avg = (r + g + b) / 3;

    // color is more bright than dim, so the stripe 
    // color will be slightly darker
    if (avg > 128)
    {
        r -= pixel_change;
        g -= pixel_change;
        b -= pixel_change;

        if (r < 0) r = 0;
        if (g < 0) g = 0;
        if (b < 0) b = 0;
    }

    // opposite
    else
    {
        r += pixel_change;
        g += pixel_change;
        b += pixel_change;

        if (r > 255) r = 255;
        if (g > 255) g = 255;
        if (b > 255) b = 255;
    }

    // set color 
    COLORREF ret = RGB(r, g, b);
    return ret;

}

LRESULT ListView::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
    m_MouseTrack = false;
    return 1L;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube