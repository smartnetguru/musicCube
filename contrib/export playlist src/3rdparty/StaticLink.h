//
// Static Text control with clickable link(URL-EMail) and multiline tooltip support
// by Bulent Ozkir February 10, 2001
// http://www.mindcracker.com/mindcracker/c_cafe/controls/staticlink.asp
//
// Minor modifications by Martin Dittus 2005
//

#pragma once

/////////////////////////////////////////////////////////////////////////////
// CStaticLink window

class CStaticLink : public CStatic
{
// Construction
public:
    CStaticLink();

// Attributes
public:
    void SetLinkCursor(HCURSOR hCursor) { m_hLinkCursor = hCursor; }
    void SetLink(LPCTSTR lpszLink);
    void SetWindowText(LPCTSTR lpszString);
protected:
    void AdjustSize();

// you can change these any time:
    COLORREF        m_colorUnvisited;         // color for unvisited
    COLORREF        m_colorVisited;           // color for visited
    BOOL            m_bVisited;               // whether visited or not
    HCURSOR            m_hLinkCursor;
    CToolTipCtrl    m_tooltip;
    
    // URL/filename for non-text controls (e.g., icon, bitmap) or when link is
    // different from window text. If you don't set this, CStaticIcon will
    // use GetWindowText to get the link.
    CString            m_strLink;
    CFont            m_font;                  // underline font for text control

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CStaticLink)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CStaticLink();
 
    // Generated message map functions
protected:
    //{{AFX_MSG(CStaticLink)
    afx_msg void OnClicked();
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);    
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
