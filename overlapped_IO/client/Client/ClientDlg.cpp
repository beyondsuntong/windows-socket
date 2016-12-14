// ClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "LookupSocket.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDlg dialog

CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientDlg::IDD, pParent),
m_bRunning(FALSE),
m_pLookupSocket(NULL),
m_nEventTotal(0)
{
	//{{AFX_DATA_INIT(CClientDlg)
	m_sServPort = SERVERPORT;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientDlg)
	DDX_Control(pDX, IDC_CLIENT_IPADDRESS, m_ctlServIP);
	DDX_Control(pDX, IDC_CLIENT_MEANING_EDIT, m_ctlMeaning);
	DDX_Control(pDX, IDC_CLIENT_WORD_EDIT, m_ctlWord);
	DDX_Text(pDX, IDC_CLIENT_PORT_EDIT, m_sServPort);
	DDV_MinMaxInt(pDX, m_sServPort, 5000, 6000);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	//{{AFX_MSG_MAP(CClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, OnConnectButton)
	ON_BN_CLICKED(IDC_LOOKUP_BUTTON, OnLookupButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDlg message handlers

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	

	//初始化
	SetWindowText(_T("英汉电子词典---Client"));	//设置标题
	m_ctlServIP.SetAddress(127,0 , 0, 1);		//服务器地址
	InitSocket();								//初始化套接字

	//单词编辑框无效	
	m_ctlWord.EnableWindow(FALSE);	
	//查询按钮无效
	(CButton*)GetDlgItem(IDC_LOOKUP_BUTTON)->EnableWindow(FALSE);
	//确定按钮无效
	(CButton*)GetDlgItem(IDOK)->EnableWindow(FALSE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


/*
 * 连接服务器
 */
void CClientDlg::OnConnectButton() 
{
	int nRet;			//返回值
	UpdateData(TRUE);	//获取数据
	
	//获取服务器IP
	DWORD dwServIP;
	m_ctlServIP.GetAddress(dwServIP);
	if (0 == dwServIP)
	{
		AfxMessageBox("请填写服务器地址。");
		return;
	}
	//服务器端口
	if (0 == m_sServPort)
	{
		AfxMessageBox("请填写服务器端口。");
		return;
	}
	//服务器地址
	SOCKADDR_IN	servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(m_sServPort);
	int nServLen = sizeof(servAddr);

	nRet = connect(m_sHost, (SOCKADDR*)&servAddr, nServLen);
	if (SOCKET_ERROR == nRet)
	{
		AfxMessageBox(_T("连接服务器失败！"));
		return;
	}

	//客户端运行
	m_bRunning = TRUE;

	//创建事件对象
	m_arrEvent[m_nEventTotal] = WSACreateEvent();//发送事件out
	m_nEventTotal++;
	m_arrEvent[m_nEventTotal] = WSACreateEvent();//接收事件in
	m_nEventTotal++;
	m_pLookupSocket = new CLookupSocket(this, m_sHost, 
						m_arrEvent[m_nEventTotal - 2],
						m_arrEvent[m_nEventTotal - 1]);
	//创建工作线程
	DWORD dwThreadId;
	m_hWorkerThread = CreateThread(NULL, 0, WorkerThread, this, 0, &dwThreadId);

	//有效状态
	m_ctlWord.EnableWindow(TRUE);
	(CButton*)GetDlgItem(IDC_LOOKUP_BUTTON)->EnableWindow(TRUE);
	(CButton*)GetDlgItem(IDOK)->EnableWindow(TRUE);

	//无效状态
	m_ctlServIP.EnableWindow(FALSE);
	(CEdit*)GetDlgItem(IDC_CLIENT_PORT_EDIT)->EnableWindow(FALSE);
	(CButton*)GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
}

/*
 * 查询单词
 */
void CClientDlg::OnLookupButton() 
{
	if (NULL != m_pLookupSocket)			//CLookupSocket指针
	{
		CString strWord;
		m_ctlWord.GetWindowText(strWord);	//获取单词
		
		if (strWord.IsEmpty())
		{
			m_ctlMeaning.SetWindowText(_T("请输入单词。"));
			return;
		}		
		CString strWordLookup;
		if (IsWord((char*)(LPCSTR)strWord, strWordLookup))		//判断是否为单词
		{
			if (FALSE == m_pLookupSocket->Lookup(strWordLookup))//发送数据
			{
				AfxMessageBox(_T("查询单词失败！"));
			}
		}else
		{
			//提示信息
			m_ctlMeaning.SetWindowText(_T("请输入单词。不要包含汉字和其他符号。"));
		}		
	}	
}
/*
 * 初始化套接字
 */
BOOL CClientDlg::InitSocket(void )
{
	int			reVal;			//返回值	

	WSADATA wsaData;			//初始化套接字动态库	
	if ((reVal = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("初始化套接字动态库错误%d\n", WSAGetLastError());
		return FALSE;
	}
	
	//创建套接字
	if ((m_sHost = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 
					0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("创建套接字错误%d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	} 
	return TRUE;
}

/*
 * 工作线程
 */
DWORD WINAPI CClientDlg::WorkerThread( void *pParam )
{
	CClientDlg *pClientDlg = (CClientDlg*)pParam;//主窗口指针
	while (pClientDlg->m_bRunning)
	{
		
		DWORD dwIndex;				//返回值
		DWORD dwFlags;				//标志
		DWORD dwBytesTraned;		//实际传输的数据	
		LPWSAOVERLAPPED pIO= NULL;	//重叠结构指针
		//等待事件
		if ((dwIndex = WSAWaitForMultipleEvents(pClientDlg->m_nEventTotal,
												pClientDlg->m_arrEvent,
												FALSE,
												THREAD_SLEEP_TIME, TRUE))
												== WSA_WAIT_FAILED)
		{
			printf("WSAWaitForMultipleEvents 失败 %d\n", WSAGetLastError());
			return 0;
		} 
		if (WSA_WAIT_TIMEOUT == dwIndex)
		{
			continue;
		}
		//重置事件
		WSAResetEvent(pClientDlg->m_arrEvent[dwIndex - WSA_WAIT_EVENT_0]);

		pIO= pClientDlg->MapEventToOverlapped(dwIndex - WSA_WAIT_EVENT_0);
		ASSERT(NULL != pIO);
		
		//检查操作完成状态
		BOOL bRet = WSAGetOverlappedResult(pClientDlg->m_sHost, 
											pIO,
											&dwBytesTraned,
											TRUE, 
											&dwFlags);
		if(bRet == FALSE || dwBytesTraned == 0)//发生错误
		{	
			return -1;
			
		}else
		{
			//接收数据完成
			if (pIO == &pClientDlg->m_pLookupSocket->m_iIO)
			{
				ASSERT( 1== dwIndex);
				pClientDlg->m_pLookupSocket->HandleData();
			}else
			{
				//发送数据完毕
			}
		}
	}
	
	return 0;
}

/*
 * 根据序号得到WSAOVERLAPPED指针
 */
WSAOVERLAPPED * CClientDlg::MapEventToOverlapped( int nIndex )
{
	if ( 0 == nIndex)
	{
		return &m_pLookupSocket->m_oIO;//发送数据操作重叠结构
	}else if ( 1 == nIndex)
	{
		return &m_pLookupSocket->m_iIO;//接收数据操作重叠结构
	}else 
	{
		return NULL;
	}
}
/*
 * 显示单词含义
 */
void CClientDlg::ShowMeaning( TCHAR *pMeaning )
{
	if (NULL == pMeaning )//字符串为空
	{
		return;
	}
	CString strMeaning(pMeaning);		//单词含义
	m_ctlMeaning.SetWindowText(_T(""));	//清空原来字符串

	//解析字符串
	CString meaning1;					//第一个含义
	CString meaning2;					//第二个含义
	int nIndex = strMeaning.Find(">");
	if (-1 == nIndex)
	{
		return;
	}
	meaning1 = strMeaning.Mid(1,nIndex -1);
	int nLen = strMeaning.GetLength();
	meaning2 = strMeaning.Mid(nIndex + 2, nLen - meaning1.GetLength() - 4);

	CString meaning;			//显示含义字符串
	if (!meaning2.IsEmpty())	//第二个含义不为空
	{
		meaning ="◇ " +
				meaning1 +
				"\r\n"	+		//换行
				"◇ " +
				meaning2;
	}else						//第二个含义为空
	{
		meaning ="◇ " +
				meaning1;
	}

	m_ctlMeaning.SetWindowText(meaning);//显示含义
}


/*
 * 判断是否为单词
 */
BOOL CClientDlg::IsWord( char * pExp, CString &strWord )
{
	int nCount = strlen(pExp);	//字符长度
	int nLen = 0;
	char *pTemp = pExp;
	while (*pTemp == ' ')		//剔除前面的空格
	{
		nLen++;
		pTemp++;
	}	
	char *pStartWord = pTemp;	//字符开始位置

	//结束循环的条件:1.遇到空格 2.字符串结束
	while (*pTemp != ' ' && nLen != nCount)
	{
		if (*pTemp < 65)
		{
			return FALSE;
		}else if ( *pTemp > 90 && *pTemp < 97)
		{
			return FALSE;
		}else if ( *pTemp > 122)
		{
			return FALSE;
		}else
		{
			pTemp++;
			nLen++;
		}		
	}
	int nNum = pTemp - pStartWord;		//有效字符数量
	CString strTemp(pStartWord, nNum);	//临时变量
	strWord = strTemp;
	return TRUE;
}

/*
 * 开始查询单词
 */
void CClientDlg::OnOK() 
{
	OnLookupButton();
}

/*
 * 退出
 */
void CClientDlg::OnCancel() 
{
	m_bRunning = FALSE;								//设置客户端运行状态
	WaitForSingleObject(m_hWorkerThread, INFINITE);	//等待工作线程退出
	delete m_pLookupSocket;							//删除CLookupSocket指针
	m_pLookupSocket = NULL;
	WSACleanup();									//释放套接字资源
	CDialog::OnCancel();							//销毁对话框
}
