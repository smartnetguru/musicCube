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

#pragma once

#include "StaticLink.h"

#include <musikCore.h>

// -- ... -----------------------------------

class CAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(CAboutDlg)

public:
	CAboutDlg(LPCTSTR caption, CWnd* pParent = NULL);
	virtual ~CAboutDlg();

	enum { IDD = IDD_ABOUT };

	BOOL OnInitDialog();

	void SetPluginName(LPCTSTR pluginName);
	void SetLink(LPCTSTR link);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	LPCTSTR m_caption;

	CStatic m_cPluginName;
	CStaticLink m_cLink;

	CFont m_boldFont;

	CString m_pluginName;
	CString m_link;
};


// -- ... -----------------------------------

class CMainDlg : public CDialog
{
	DECLARE_DYNAMIC(CMainDlg)

public:
	CMainDlg(LPCTSTR caption, CWnd* pParent = NULL);
	virtual ~CMainDlg();

	enum { IDD = IDD_MAIN };

	BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()

	CString m_caption;	
};


// -- ... -----------------------------------



// -- ... -----------------------------------

class CmusikPluginApp : public CWinApp
{
public:
	CmusikPluginApp();

public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
