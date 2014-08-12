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
// 
// Usage: 
//
//     Use Set() to set the mask, that should be
//   it. Follow the following guidelines:
//
//     %a [id]    <-- attribute, such as title, artist, album, etc
//     %f [size]  <-- regular font
//     %b [size]  <-- bold font
//     %i [size]  <-- italic font
//     %m [size]  <-- bold and italic font
//     %c [str]   <-- comment
//
// Example: 
//
//   m_Info->Set(_T("%b20 %a4 %c from the album %a1"));
//
//      %b20 initializes the font to 20 with a bold attribute
//      %a4 prints the current song's title
//      %c prints the comment "from the album" using default font
//      %a1 prints the album name, with the previously specified font  
//
///////////////////////////////////////////////////

#include "stdafx.h"

#include "musikCubeApp.h"
#include "TransportViewInfoLine.h"

#include "3rdparty/memdc.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(TransportViewInfoLine, CWnd)

BEGIN_MESSAGE_MAP(TransportViewInfoLine, CWnd)
    ON_WM_CREATE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

/*ctor*/ TransportViewInfoLine::TransportViewInfoLine()
{
    m_Stopped.push_back(_T("%c playback stopped"));
}

/*dtor*/ TransportViewInfoLine::~TransportViewInfoLine()
{
    size_t i;

    for (i = 0; i < m_Captions.size(); i++)
        delete m_Captions.at(i);

    for (i = 0; i < m_Comments.size(); i++)
        delete m_Comments.at(i);
}

void TransportViewInfoLine::UpdateInfo(bool refresh)
{
    SetRedraw(false);
    int type;
    for (size_t i = 0; i < m_Captions.size(); i++)
    {
        type = m_Captions.at(i)->GetType();
        m_Captions.at(i)->SetDynText((CString)Globals::Player->GetPlaying()->GetField(type));                
    }
    SetRedraw(true);

    Layout(refresh);
}

void TransportViewInfoLine::Layout(bool refresh)
{
    SetRedraw(false);

    CSize rect = GetSize();

    FontBaseline blLargest, blCurrent;
    GetBaseline(GetHeight(), &blLargest);

    int nCurrX = 0;
    int nCurrY = 0;

    CRect rcClient;
    CPoint ptLoc;
    for (size_t i = 0; i < m_LayoutOrder.size(); i++)
    {
        GetBaseline(m_LayoutOrder.at(i)->GetFontSize(), &blCurrent);

        ptLoc.x = nCurrX;
        ptLoc.y = 0;
        ptLoc.y = blLargest.m_Ascending - blCurrent.m_Ascending;    

        m_LayoutOrder.at(i)->GetClientRect(rcClient);
        m_LayoutOrder.at(i)->MoveWindow(CRect(ptLoc, CSize(rcClient.Width(), blLargest.m_Height)));

        nCurrX += m_LayoutOrder.at(i)->GetWidth();
    }

    SetRedraw(true);

    if (refresh)
        RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ERASENOW | RDW_ALLCHILDREN);
}

int TransportViewInfoLine::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}

void TransportViewInfoLine::Set(CString mask)
{
    if (mask.IsEmpty())
        return;

    // reinitalize the objects
    musik::Core::Filename::DelimitStr(mask.GetBuffer(NULL), _T("%"), m_Items, false);
    InitObjects();
}

void TransportViewInfoLine::InitObjects()
{
    size_t i;

    // clear the old items
    for (i = 0; i < m_Captions.size(); i++)
        delete m_Captions.at(i);
    for (i = 0; i < m_Comments.size(); i++)
        delete m_Comments.at(i);
    
    m_Captions.clear();
    m_Comments.clear();
    m_LayoutOrder.clear();

    // make new items
    musik::Core::StringArray& itemsref = m_Items;
    Globals::Player->IsPlaying() ? itemsref = m_Items : itemsref = m_Items;

    size_t nObjs = itemsref.size();
    CString sCheck, sCurr;
    for (i = 0; i < nObjs; i++)
    {
        sCheck = itemsref.at(i).Left(1);
        sCheck.MakeLower();

        // we found a new font
        if (sCheck == _T("f") || sCheck == _T("b") || sCheck == _T("i") || sCheck == _T("m"))
        {
            sCurr = itemsref.at(i).Right(itemsref.at(i).GetLength()  - 1);
            sCurr.TrimLeft();
            sCurr.TrimRight();

            m_FontSize = musik::Core::StringToInt(sCurr.GetBuffer());

            if (sCheck == _T("b") || sCheck == _T("m"))
                m_FontBold = 1;
            else
                m_FontBold = 0;

            if (sCheck == _T("i") || sCheck == _T("m"))
                m_FontItalic = 1;
            else
                m_FontItalic = 0;
        }

        // found a comment (default font)
        else if (sCheck == _T("c"))
        {
            // find the comment
            sCurr = itemsref.at(i).Right(itemsref.at(i).GetLength()  - 1);

            // create the object
            AutoSizingStatic* pTemp;
            pTemp = new AutoSizingStatic();
            pTemp->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);
            
            // set it to the right values
            pTemp->SetDynFont(Globals::Preferences->GetNowPlayingCaptionFontSize());
            pTemp->SetDynText(sCurr);
            pTemp->SetType(-1);

            // add to comment array
            m_Comments.push_back(pTemp);
            m_LayoutOrder.push_back(pTemp);
        }

        // found a new object, so create it
        // with the current font and text
        else if (sCheck == _T("a"))
        {
            sCurr = itemsref.at(i).Right(itemsref.at(i).GetLength()  - 1);
            sCurr.TrimLeft();
            sCurr.TrimRight();

            AutoSizingStatic* pTemp;
            pTemp = new AutoSizingStatic();
            pTemp->Create(NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this);

            pTemp->SetDynFont(m_FontSize, m_FontBold, m_FontItalic);
            pTemp->SetDynText(_T("Stopped"));
            pTemp->SetType(musik::Core::StringToInt(sCurr.GetBuffer()));

            m_Captions.push_back(pTemp);
            m_LayoutOrder.push_back(pTemp);
        }
        else
            cout << _T("Invalid mask specified.");
    }

    if (Globals::Player->IsPlaying())
        UpdateInfo();
}

int TransportViewInfoLine::GetHeight()
{
    int largest = -1;
    for (size_t i = 0; i < m_LayoutOrder.size(); i++)
    {
        if (m_LayoutOrder.at(i)->GetFontSize() > largest)
            largest = m_LayoutOrder.at(i)->GetFontSize();
    }

    if (largest < 14)
        largest = 14;

    return largest;
}

int TransportViewInfoLine::GetWidth()
{
    size_t i;
    int width = 0;

    for (i = 0; i < m_Captions.size(); i++)
        width += m_Captions.at(i)->GetWidth();

    for (i = 0; i < m_Comments.size(); i++)
        width += m_Comments.at (i)->GetWidth();

    return width;
}

CSize TransportViewInfoLine::GetSize()
{
    return CSize(GetWidth(), GetHeight()); 
}

void TransportViewInfoLine::GetBaseline(int font_size, FontBaseline* baseline)
{
    CFont *temp_font;

    temp_font = new CFont();
    temp_font->CreateStockObject(DEFAULT_GUI_FONT);    
    
    LOGFONT temp_log_font;
    temp_font->GetLogFont(&temp_log_font);
    temp_log_font.lfHeight = font_size;

    delete temp_font;
    temp_font = new CFont();
    temp_font->CreateFontIndirect(&temp_log_font);
    
    CDC *pDC = GetDC();
    TEXTMETRIC metrics;
    CFont* pOldFont = pDC->SelectObject(temp_font);
    pDC->GetTextMetrics(&metrics);
    pDC->SelectObject(pOldFont);

    delete temp_font;

    // SH 2004-01 GDI leak cleanup  -- when doing GetDC, ReleaseDC is a must afterwards.
    ReleaseDC(pDC);

    baseline->m_Ascending = metrics.tmAscent;
    baseline->m_Descending = metrics.tmDescent;
    baseline->m_Height = metrics.tmHeight;
}

void TransportViewInfoLine::OnPaint()
{
    CPaintDC dc(this);

    CRect rect;
    GetClientRect(&rect);
    CMemDC memDC(&dc, &rect);

    memDC.FillSolidRect(rect, Globals::Preferences->GetBtnFaceColor());

    DefWindowProc(WM_PAINT, (WPARAM)memDC->m_hDC, (LPARAM)0);
}

BOOL TransportViewInfoLine::OnEraseBkgnd(CDC* pDC)
{
    return false;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube