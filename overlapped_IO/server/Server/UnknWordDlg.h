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
	void	AddWord(CString strWord);	//��ӵ���
protected:
	void	InitListCtrlSetting(void);	//��ʼ���б���ͼ�ؼ�
	void	UpdateLstCtrl(void);		//�����б���ͼ�ؼ�

	CServerView	 *m_pServView;					//������ָ��
	CMap<CString, LPCTSTR, int, int> m_WordMap;	//����map����

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NOTWORDDLG_H__22882AFE_1EA3_4193_B2ED_F09F2E8A3F3E__INCLUDED_)
