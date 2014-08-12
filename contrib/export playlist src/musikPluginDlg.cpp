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

#include "musikPluginDlg.h"

// -- ... -----------------------------------

IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)

CAboutDlg::CAboutDlg(LPCTSTR caption, CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent), m_caption(caption)
{
}

CAboutDlg::~CAboutDlg()
{
	m_boldFont.DeleteObject();
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ABOUT_NAME, m_cPluginName);
	DDX_Control(pDX, IDC_ABOUT_LINK, m_cLink);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_STN_CLICKED(IDC_ABOUT_LINK, OnStnClickedAboutLink)
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_caption);

// link control
	m_cLink.SetWindowText(m_link);
	//m_cLink.SetLink(m_link);

// plugin name
	m_cPluginName.SetWindowText(m_pluginName);

	CFont *pFont;
    LOGFONT bold;
	pFont = m_cPluginName.GetFont();
	pFont->GetLogFont(&bold);
    bold.lfWeight = FW_BOLD;
    m_boldFont.CreateFontIndirect(&bold);
	m_cPluginName.SetFont(&m_boldFont);

// etc

	return TRUE;	// as we did not set the focus to a control
}


void CAboutDlg::SetPluginName(LPCTSTR pluginName)
{
	m_pluginName = pluginName;
}

void CAboutDlg::SetLink(LPCTSTR link)
{
	m_link = link;
}


// -- ... -----------------------------------

IMPLEMENT_DYNAMIC(CMainDlg, CDialog)
CMainDlg::CMainDlg(LPCTSTR caption, CWnd* pParent /*=NULL*/)
	: CDialog(CMainDlg::IDD, pParent), m_caption(caption)
{
}

CMainDlg::~CMainDlg()
{
}

void CMainDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMainDlg, CDialog)
END_MESSAGE_MAP()


BOOL CMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_caption);

// etc
	// ...

	return TRUE;	// as we did not set the focus to a control
}



// -- ... -----------------------------------



// -- ... -----------------------------------


BEGIN_MESSAGE_MAP(CmusikPluginApp, CWinApp)
END_MESSAGE_MAP()


CmusikPluginApp::CmusikPluginApp()
{
}


BOOL CmusikPluginApp::InitInstance()
{
	CWinApp::InitInstance();
	// initialize data here

	return TRUE;
}


CmusikPluginApp theApp;


void CAboutDlg::OnStnClickedAboutLink()
{
	// TODO: Add your control notification handler code here
}
