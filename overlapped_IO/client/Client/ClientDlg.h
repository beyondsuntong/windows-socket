// ClientDlg.h : header file
//

#if !defined(AFX_CLIENTDLG_H__4A0BB7A9_017F_4AAF_B11D_AA0FD68A85DE__INCLUDED_)
#define AFX_CLIENTDLG_H__4A0BB7A9_017F_4AAF_B11D_AA0FD68A85DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClientDlg dialog
class CLookupSocket;

class CClientDlg : public CDialog
{
// Construction
public:
	CClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClientDlg)
	enum { IDD = IDD_CLIENT_DLG };
	CIPAddressCtrl	m_ctlServIP;
	CEdit	m_ctlMeaning;
	CEdit	m_ctlWord;
	short	m_sServPort;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnConnectButton();
	afx_msg void OnLookupButton();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	WSAOVERLAPPED	*MapEventToOverlapped(int nIndex);		//根据事件句柄序号得到OVERLAPPED指针
	void			ShowMeaning(TCHAR *pMeaning);			//显示单词含义
protected:
	BOOL			InitSocket(void );						//初始化套结字
	BOOL			IsWord(char * pExp, CString &strWord );	//判断是否为英文
protected:
	static	DWORD WINAPI	WorkerThread(void *pParam);	//工作线程 
protected:
	WSAEVENT		m_arrEvent[MAX_WAIT_EVENTS];		//事件对象数组
	DWORD			m_nEventTotal;						//事件总数
	SOCKET			m_sHost;							//主机套接字
	BOOL			m_bRunning;							//客户端运行状态
	HANDLE			m_hWorkerThread;					//工作线程句柄
	CLookupSocket	*m_pLookupSocket;					//CLookupSocket指针
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTDLG_H__4A0BB7A9_017F_4AAF_B11D_AA0FD68A85DE__INCLUDED_)
