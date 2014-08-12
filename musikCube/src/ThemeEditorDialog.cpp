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
#include "Preferences.h"
#include "musikCubeApp.h"
#include "ThemeEditorDialog.h"
#include "FileDialog.h"

///////////////////////////////////////////////////

namespace musik { namespace Cube {

///////////////////////////////////////////////////
// ThemeEditorDialog
///////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(ThemeEditorDialog, CDialog)

BEGIN_MESSAGE_MAP(ThemeEditorDialog, CDialog)
    ON_MESSAGE(CPN_SELENDOK, OnColorChanged)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_BUTTON_LOAD, OnBnClickedButtonLoad)
    ON_BN_CLICKED(IDOK, OnBnClickedOk)
    ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
    ON_BN_CLICKED(IDC_RADIO_USEDARKICONS, OnChangeButtonIconTheme)
    ON_BN_CLICKED(IDC_RADIO_USELIGHTICONS, OnChangeButtonIconTheme)
END_MESSAGE_MAP()

/*ctor*/ ThemeEditorDialog::ThemeEditorDialog(CWnd* pParent /*=NULL*/)
: CDialog(ThemeEditorDialog::IDD, pParent)
{
}

/*dtor*/ ThemeEditorDialog::~ThemeEditorDialog()
{
}

LONG ThemeEditorDialog::OnColorChanged(UINT lParam, LONG wParam)
{
    COLORREF newColor = (COLORREF)lParam;
    unsigned int senderID = (unsigned int)wParam;

    CColourPicker& colorPicker = *m_ID_ToControlMap[senderID];

    *m_ID_ToColorMap[senderID] = newColor;        
    colorPicker.SetColour(newColor);

    AfxGetApp()->GetMainWnd()->SendMessage(WM_SYSCOLORCHANGE);

    return 0L;
}

void ThemeEditorDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_BUTTON_ACTIVECAPTION, m_ActiveCaptionPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_ACTIVECAPTION, Globals::Preferences->MUSIK_COLOR_ACTIVECAPTION);

    DDX_Control(pDX, IDC_BUTTON_ACTIVECAPTIONTEXT, m_ActiveCaptionTextPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_ACTIVECAPTIONTEXT, Globals::Preferences->MUSIK_COLOR_CAPTIONTEXT);
    
    DDX_Control(pDX, IDC_BUTTON_INACTIVECAPTION, m_InActiveCaptionPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_INACTIVECAPTION, Globals::Preferences->MUSIK_COLOR_INACTIVECAPTION);

    DDX_Control(pDX, IDC_BUTTON_INACTIVECAPTIONTEXT, m_InActiveCaptionTextPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_INACTIVECAPTIONTEXT, Globals::Preferences->MUSIK_COLOR_INACTIVECAPTIONTEXT);

    DDX_Control(pDX, IDC_BUTTON_BTNFACE, m_BtnFacePicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_BTNFACE, Globals::Preferences->MUSIK_COLOR_BTNFACE);

    DDX_Control(pDX, IDC_BUTTON_BTNTEXT, m_BtnTextPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_BTNTEXT, Globals::Preferences->MUSIK_COLOR_BTNTEXT);

    DDX_Control(pDX, IDC_BUTTON_BTNHILIGHT, m_BtnHilightPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_BTNHILIGHT, Globals::Preferences->MUSIK_COLOR_BTNHILIGHT);

    DDX_Control(pDX, IDC_BUTTON_BTNSHADOW, m_BtnShadowPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_BTNSHADOW, Globals::Preferences->MUSIK_COLOR_BTNSHADOW);

    DDX_Control(pDX, IDC_BUTTON_LISTCTRL, m_ListCtrlPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_LISTCTRL, Globals::Preferences->MUSIK_COLOR_LISTCTRL);

    DDX_Control(pDX, IDC_BUTTON_LISTCTRLTEXT, m_ListCtrlTextPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_LISTCTRLTEXT, Globals::Preferences->MUSIK_COLOR_LISTCTRLTEXT);

    DDX_Control(pDX, IDC_BUTTON_HIGHLIGHT, m_HighLightPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_HIGHLIGHT, Globals::Preferences->MUSIK_COLOR_HIGHLIGHT);

    DDX_Control(pDX, IDC_BUTTON_HIGHLIGHTTEXT, m_HighLightTextPicker);
    DDX_ColourPicker(pDX, IDC_BUTTON_HIGHLIGHTTEXT, Globals::Preferences->MUSIK_COLOR_HIGHLIGHTTEXT);
}

int ThemeEditorDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTION] = &m_ActiveCaptionPicker;
    m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTION] = &Globals::Preferences->MUSIK_COLOR_ACTIVECAPTION;

    m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTIONTEXT] = &m_ActiveCaptionTextPicker;
    m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTIONTEXT] = &Globals::Preferences->MUSIK_COLOR_CAPTIONTEXT;

    m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTION] = &m_InActiveCaptionPicker;
    m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTION] = &Globals::Preferences->MUSIK_COLOR_INACTIVECAPTION;

    m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTIONTEXT] = &m_InActiveCaptionTextPicker;
    m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTIONTEXT] = &Globals::Preferences->MUSIK_COLOR_INACTIVECAPTIONTEXT;

    m_ID_ToControlMap[IDC_BUTTON_BTNFACE] = &m_BtnFacePicker;
    m_ID_ToColorMap[IDC_BUTTON_BTNFACE] = &Globals::Preferences->MUSIK_COLOR_BTNFACE;

    m_ID_ToControlMap[IDC_BUTTON_BTNTEXT] = &m_BtnTextPicker;
    m_ID_ToColorMap[IDC_BUTTON_BTNTEXT] = &Globals::Preferences->MUSIK_COLOR_BTNTEXT;

    m_ID_ToControlMap[IDC_BUTTON_BTNHILIGHT] = &m_BtnHilightPicker;
    m_ID_ToColorMap[IDC_BUTTON_BTNHILIGHT] = &Globals::Preferences->MUSIK_COLOR_BTNHILIGHT;

    m_ID_ToControlMap[IDC_BUTTON_BTNSHADOW] = &m_BtnShadowPicker;
    m_ID_ToColorMap[IDC_BUTTON_BTNSHADOW] = &Globals::Preferences->MUSIK_COLOR_BTNSHADOW;

    m_ID_ToControlMap[IDC_BUTTON_LISTCTRL] = &m_ListCtrlPicker;
    m_ID_ToColorMap[IDC_BUTTON_LISTCTRL] = &Globals::Preferences->MUSIK_COLOR_LISTCTRL;

    m_ID_ToControlMap[IDC_BUTTON_LISTCTRLTEXT] = &m_ListCtrlTextPicker;
    m_ID_ToColorMap[IDC_BUTTON_LISTCTRLTEXT] = &Globals::Preferences->MUSIK_COLOR_LISTCTRLTEXT;

    m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHT] = &m_HighLightPicker;
    m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHT] = &Globals::Preferences->MUSIK_COLOR_HIGHLIGHT;

    m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHTTEXT] = &m_HighLightTextPicker;
    m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHTTEXT] = &Globals::Preferences->MUSIK_COLOR_HIGHLIGHTTEXT;

    return 0;
}

void ThemeEditorDialog::ReloadColors()
{
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTION]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTION]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_ACTIVECAPTIONTEXT]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_ACTIVECAPTIONTEXT]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTION]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTION]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_INACTIVECAPTIONTEXT]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_INACTIVECAPTIONTEXT]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNFACE]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNFACE]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNTEXT]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNTEXT]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNHILIGHT]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNHILIGHT]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_BTNSHADOW]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_BTNSHADOW]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_LISTCTRL]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_LISTCTRL]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_LISTCTRLTEXT]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_LISTCTRLTEXT]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHT]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHT]);
    ((ThemeEditorColorPicker&)*m_ID_ToControlMap[IDC_BUTTON_HIGHLIGHTTEXT]).SetColour((COLORREF&)*m_ID_ToColorMap[IDC_BUTTON_HIGHLIGHTTEXT]);

    if (Globals::Preferences->UseLightIconTheme())
    {
        ((CButton*)GetDlgItem(IDC_RADIO_USELIGHTICONS))->SetCheck(1);
        ((CButton*)GetDlgItem(IDC_RADIO_USEDARKICONS))->SetCheck(0);
    }
    else
    {
        ((CButton*)GetDlgItem(IDC_RADIO_USEDARKICONS))->SetCheck(1);
        ((CButton*)GetDlgItem(IDC_RADIO_USELIGHTICONS))->SetCheck(0);
    }
}

void ThemeEditorDialog::OnBnClickedButtonLoad()
{
    CString filter = _T("musikCube Theme Files (*.ini)|*.ini||");
    
    FileDialog ofdlg(
        TRUE, 
        _T("ini"), 
        NULL, 
        OFN_EXPLORER | OFN_FILEMUSTEXIST, 
        filter
   );

    if (ofdlg.DoModal() == IDOK)
    {
        std::string fn = musik::Core::utf16to8(ofdlg.GetFileName().GetBuffer(), false);
        CIniFile iniFile(fn);

        if (!iniFile.ReadFile())
        {
            MessageBox(
                _T("Failed to read theme file from disk.  Invalid access rights?"),
                _T("musikCube"),
                MB_OK | MB_ICONERROR
           );

            return;
        }

        Globals::Preferences->MUSIK_COLOR_ACTIVECAPTION = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Active Caption", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_CAPTIONTEXT = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Active Caption Text", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_INACTIVECAPTION = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Inactive Caption", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_INACTIVECAPTIONTEXT = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Inactive Caption Text", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_BTNFACE = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Button Face", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_BTNTEXT = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Button Text", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_BTNHILIGHT = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Button Hilight", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_BTNSHADOW = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Button Shadow", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_LISTCTRL = 
            StringToCOLORREF(iniFile.GetValue("Theme", "List", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_LISTCTRLTEXT = 
            StringToCOLORREF(iniFile.GetValue("Theme", "List Text", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_HIGHLIGHT = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Highlight", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->MUSIK_COLOR_HIGHLIGHTTEXT = 
            StringToCOLORREF(iniFile.GetValue("Theme", "Highlight Text", COLORREFToString(AUTOMATIC_COLOR)));
        Globals::Preferences->SetUseLightIconTheme(
            StringToBool(iniFile.GetValue("Theme", "Use Light Icons", BoolToString(false))));

        this->ReloadColors();
        AfxGetApp()->GetMainWnd()->SendMessage(WM_SYSCOLORCHANGE);
    }
}

void ThemeEditorDialog::OnBnClickedOk()
{
    OnOK();
}

void ThemeEditorDialog::OnBnClickedButtonSave()
{
    CString filter = _T("musikCube Theme Files (*.ini)|*.ini||");
    
    FileDialog sfdlg(
        FALSE, 
        _T("ini"), 
        NULL, 
        OFN_EXPLORER | OFN_OVERWRITEPROMPT, 
        filter
   );

    if (sfdlg.DoModal() == IDOK)
    {
        CIniFile iniFile(musik::Core::utf16to8(sfdlg.GetFileName().GetBuffer(), false));

        iniFile.SetValue(
            "Theme", "Active Caption", COLORREFToString(Globals::Preferences->MUSIK_COLOR_ACTIVECAPTION));
        iniFile.SetValue(
            "Theme", "Active Caption Text", COLORREFToString(Globals::Preferences->MUSIK_COLOR_CAPTIONTEXT));
        iniFile.SetValue(
            "Theme", "Inactive Caption", COLORREFToString(Globals::Preferences->MUSIK_COLOR_INACTIVECAPTION));
        iniFile.SetValue(
            "Theme", "Inactive Caption Text", COLORREFToString(Globals::Preferences->MUSIK_COLOR_INACTIVECAPTIONTEXT));
        iniFile.SetValue(
            "Theme", "Button Face", COLORREFToString(Globals::Preferences->MUSIK_COLOR_BTNFACE));
        iniFile.SetValue(
            "Theme", "Button Text", COLORREFToString(Globals::Preferences->MUSIK_COLOR_BTNTEXT));
        iniFile.SetValue(
            "Theme", "Button Hilight", COLORREFToString(Globals::Preferences->MUSIK_COLOR_BTNHILIGHT));
        iniFile.SetValue(
            "Theme", "Button Shadow", COLORREFToString(Globals::Preferences->MUSIK_COLOR_BTNSHADOW));
        iniFile.SetValue(
            "Theme", "List", COLORREFToString(Globals::Preferences->MUSIK_COLOR_LISTCTRL));
        iniFile.SetValue(
            "Theme", "List Text", COLORREFToString(Globals::Preferences->MUSIK_COLOR_LISTCTRLTEXT));
        iniFile.SetValue(
            "Theme", "Highlight", COLORREFToString(Globals::Preferences->MUSIK_COLOR_HIGHLIGHT));
        iniFile.SetValue(
            "Theme", "Highlight Text", COLORREFToString(Globals::Preferences->MUSIK_COLOR_HIGHLIGHTTEXT));
        iniFile.SetValue(
            "Theme", "Use Light Icons", BoolToString(Globals::Preferences->UseLightIconTheme()));

        if (!iniFile.WriteFile())
        {
            MessageBox(
                _T("Failed to write theme file to disk.  Invalid access rights?"),
                _T("musikCube"),
                MB_OK | MB_ICONERROR
           );
        }
    }
}

void ThemeEditorDialog::OnChangeButtonIconTheme()
{
    int useLightIcons = ((CButton*)GetDlgItem(IDC_RADIO_USELIGHTICONS))->GetCheck();
    Globals::Preferences->SetUseLightIconTheme(useLightIcons == NULL ? false : true);

    AfxGetApp()->GetMainWnd()->SendMessage(WM_SYSCOLORCHANGE);
}

BOOL ThemeEditorDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    ReloadColors();

    return TRUE;
}

///////////////////////////////////////////////////

} } // namespace musik::Cube