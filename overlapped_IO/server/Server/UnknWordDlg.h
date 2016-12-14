#if !defined(AFX_NOTWORDDLG_H__22882AFE_1EA3_4193_B2ED_F09F2E8A3F3E__INCLUDED_)
#define AFX_NOTWORDDLG_H__22882AFE_1EA3_4193_B2ED_F09F2E8A3F3E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NotWordDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNotWordDlg dialog
class CServerView;

class CUnknWordDlg : public CDialog
{
// Construction
public:
	CUnknWordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNotWordDlg)
	enum { IDD = IDD_NOWORD_DLG };
	CListCtrl	m_ctlList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotWordDlg)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNotWordDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void	AddWord(CString strWord);	//添加单词
protected:
	void	InitListCtrlSetting(void);	//初始化列表视图控件
	void	UpdateLstCtrl(void);		//更新列表视图控件

	CServerView	 *m_pServView;					//主窗口指针
	CMap<CString, LPCTSTR, int, int> m_WordMap;	//单词map数量

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTWORDDLG_H__22882AFE_1EA3_4193_B2ED_F09F2E8A3F3E__INCLUDED_)
