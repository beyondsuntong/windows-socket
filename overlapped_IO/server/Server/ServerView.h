// ServerView.h : interface of the CServerView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERVERVIEW_H__5DBF04CD_BF2A_4F5B_9C6A_156688EB09EA__INCLUDED_)
#define AFX_SERVERVIEW_H__5DBF04CD_BF2A_4F5B_9C6A_156688EB09EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CClientSocket;
class CServerDoc;
class CUnknWordDlg;

class CServerView : public CListView
{
protected: // create from serialization only
	CServerView();
	DECLARE_DYNCREATE(CServerView)

// Attributes
public:
	CServerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CServerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CServerView)
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddWord();
	afx_msg void OnEditWord();
	afx_msg void OnDelWord();
	afx_msg void OnUpdateAddWord(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDelWord(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditWord(CCmdUI* pCmdUI);
	afx_msg void OnServerRun();
	afx_msg void OnUpdateServerRun(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnShowUnKnWordDlg();
	afx_msg void OnUpdateShowNOTWordDlg(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg LRESULT OnAddUnknWord(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	void	ShowUnKnWordDlg(void);
protected:
	void	InitListCtrlSetting(void);				//��ʼ���б���ͼ�ؼ�
	void	UpDateListCtl(void);					//�����б���ͼ�ؼ�
	BOOL	InitSocket(void);						//��ʼ���׽���

	static DWORD WINAPI ServiceThread(void *pParam);//�����̺߳���

protected:
	_RecordsetPtr	m_pRecordSet;		//��¼������
	BOOL			m_bRunning;			//����������״̬
	SOCKET			m_sListen;			//�����׽���
	BOOL			m_bShowUnknWordDlg;	//�Ƿ���ʾUnknWordDlg�Ի���
	CUnknWordDlg	*m_pUnknWordDlg;	//UnWordDlg�Ի���
	HANDLE			m_hThreadService;	//�����߳̾��


};

#ifndef _DEBUG  // debug version in ServerView.cpp
inline CServerDoc* CServerView::GetDocument()
   { return (CServerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERVIEW_H__5DBF04CD_BF2A_4F5B_9C6A_156688EB09EA__INCLUDED_)
