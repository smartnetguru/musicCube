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
#include "GraphicsUtility.h"
#include "FlatToggleButton.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////

FlatToggleButton::FlatToggleButton(Gdiplus::Bitmap* inUnchecked, Gdiplus::Bitmap* inChecked)
: CButtonST()
{
    SetBitmaps(inUnchecked, inChecked);

    SetColor(2, GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 10), FALSE);
    SetColor(3, Globals::Preferences->GetBtnTextColor(), FALSE);

    SetColor(4, GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 10), FALSE);
    SetColor(5, Globals::Preferences->GetBtnTextColor(), FALSE);

    SetColor(0, GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 20), FALSE);
    SetColor(1, Globals::Preferences->GetBtnTextColor(), FALSE);

    DrawBorder(TRUE, FALSE);
}

void FlatToggleButton::SetBitmaps(Gdiplus::Bitmap* inUnchecked, Gdiplus::Bitmap* inChecked)
{
    m_UncheckedBmp = inUnchecked;
    m_CheckedBmp = inChecked;
}

DWORD FlatToggleButton::OnDrawBorder(CDC* pDC, CRect* rect)
{
    CRect rcBorder = *rect;
    pDC->Draw3dRect(
        rcBorder, 
        Globals::Preferences->GetBtnShadowColor(), 
        Globals::Preferences->GetBtnShadowColor());


    if (m_bIsPressed)
    {
        pDC->Draw3dRect(
            rcBorder, 
            Globals::Preferences->GetBtnShadowColor(),
            Globals::Preferences->GetBtnShadowColor());
    }

    return BTNST_OK;
}

DWORD FlatToggleButton::OnDrawBackground(CDC* pDC, CRect* rect)
{
    COLORREF crColor;
    if (m_bIsPressed)
    {
        crColor = GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), -40);
    }
    else if (m_bMouseOnButton)
    {
        crColor = GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 20);
    }
    else
    {
        crColor = GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 10);
    }

    CBrush brBackground(crColor);
    pDC->FillRect(rect, &brBackground);

    if (m_bIsPressed)
    {
        COLORREF clrLight = GraphicsUtility::ShadeColor(Globals::Preferences->GetBtnFaceColor(), 20);

        CRect light = *rect;
        light.DeflateRect(1, 1, 1, 1);
        pDC->Draw3dRect(&light, clrLight, clrLight);
    }

    if (Globals::DrawGraphics)
    {
        Gdiplus::Bitmap* bmp = NULL;
        m_bMouseOnButton ? bmp = m_UncheckedBmp : bmp = m_CheckedBmp;

        if (bmp)
        {
            int l = (rect->Width() / 2) - 9;
            int t = (rect->Height() / 2) - 9;

            if (m_bIsPressed && !GetCheck() && m_bMouseOnButton)
            {
                l++;
                t++;
            }

            Gdiplus::Graphics graphics(pDC->GetSafeHdc());
            graphics.DrawImage(bmp, l, t, 18, 18);
        }
    }

    return BTNST_OK;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube