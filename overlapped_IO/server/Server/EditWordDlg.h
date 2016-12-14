#if !defined(AFX_EDITWORDDLG_H__385931F7_268C_45A4_9EAD_20FC26169773__INCLUDED_)
#define AFX_EDITWORDDLG_H__385931F7_268C_45A4_9EAD_20FC26169773__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditWordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditWordDlg dialog

class CEditWordDlg : public CDialog
{
// Construction
public:
	CEditWordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEditWordDlg)
	enum { IDD = IDD_WORD_DLG };
	CString	m_strWord;
	CString	m_strMeaning1;
	CString	m_strMeaning2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditWordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditWordDlg)
	virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
BOOL	m_bWordEdit;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITWORDDLG_H__385931F7_268C_45A4_9EAD_20FC26169773__INCLUDED_)
