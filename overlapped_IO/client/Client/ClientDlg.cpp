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
	

	//��ʼ��
	SetWindowText(_T("Ӣ�����Ӵʵ�---Client"));	//���ñ���
	m_ctlServIP.SetAddress(127,0 , 0, 1);		//��������ַ
	InitSocket();								//��ʼ���׽���

	//���ʱ༭����Ч	
	m_ctlWord.EnableWindow(FALSE);	
	//��ѯ��ť��Ч
	(CButton*)GetDlgItem(IDC_LOOKUP_BUTTON)->EnableWindow(FALSE);
	//ȷ����ť��Ч
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
 * ���ӷ�����
 */
void CClientDlg::OnConnectButton() 
{
	int nRet;			//����ֵ
	UpdateData(TRUE);	//��ȡ����
	
	//��ȡ������IP
	DWORD dwServIP;
	m_ctlServIP.GetAddress(dwServIP);
	if (0 == dwServIP)
	{
		AfxMessageBox("����д��������ַ��");
		return;
	}
	//�������˿�
	if (0 == m_sServPort)
	{
		AfxMessageBox("����д�������˿ڡ�");
		return;
	}
	//��������ַ
	SOCKADDR_IN	servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_port = htons(m_sServPort);
	int nServLen = sizeof(servAddr);

	nRet = connect(m_sHost, (SOCKADDR*)&servAddr, nServLen);
	if (SOCKET_ERROR == nRet)
	{
		AfxMessageBox(_T("���ӷ�����ʧ�ܣ�"));
		return;
	}

	//�ͻ�������
	m_bRunning = TRUE;

	//�����¼�����
	m_arrEvent[m_nEventTotal] = WSACreateEvent();//�����¼�out
	m_nEventTotal++;
	m_arrEvent[m_nEventTotal] = WSACreateEvent();//�����¼�in
	m_nEventTotal++;
	m_pLookupSocket = new CLookupSocket(this, m_sHost, 
						m_arrEvent[m_nEventTotal - 2],
						m_arrEvent[m_nEventTotal - 1]);
	//���������߳�
	DWORD dwThreadId;
	m_hWorkerThread = CreateThread(NULL, 0, WorkerThread, this, 0, &dwThreadId);

	//��Ч״̬
	m_ctlWord.EnableWindow(TRUE);
	(CButton*)GetDlgItem(IDC_LOOKUP_BUTTON)->EnableWindow(TRUE);
	(CButton*)GetDlgItem(IDOK)->EnableWindow(TRUE);

	//��Ч״̬
	m_ctlServIP.EnableWindow(FALSE);
	(CEdit*)GetDlgItem(IDC_CLIENT_PORT_EDIT)->EnableWindow(FALSE);
	(CButton*)GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
}

/*
 * ��ѯ����
 */
void CClientDlg::OnLookupButton() 
{
	if (NULL != m_pLookupSocket)			//CLookupSocketָ��
	{
		CString strWord;
		m_ctlWord.GetWindowText(strWord);	//��ȡ����
		
		if (strWord.IsEmpty())
		{
			m_ctlMeaning.SetWindowText(_T("�����뵥�ʡ�"));
			return;
		}		
		CString strWordLookup;
		if (IsWord((char*)(LPCSTR)strWord, strWordLookup))		//�ж��Ƿ�Ϊ����
		{
			if (FALSE == m_pLookupSocket->Lookup(strWordLookup))//��������
			{
				AfxMessageBox(_T("��ѯ����ʧ�ܣ�"));
			}
		}else
		{
			//��ʾ��Ϣ
			m_ctlMeaning.SetWindowText(_T("�����뵥�ʡ���Ҫ�������ֺ��������š�"));
		}		
	}	
}
/*
 * ��ʼ���׽���
 */
BOOL CClientDlg::InitSocket(void )
{
	int			reVal;			//����ֵ	

	WSADATA wsaData;			//��ʼ���׽��ֶ�̬��	
	if ((reVal = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("��ʼ���׽��ֶ�̬�����%d\n", WSAGetLastError());
		return FALSE;
	}
	
	//�����׽���
	if ((m_sHost = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 
					0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("�����׽��ִ���%d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	} 
	return TRUE;
}

/*
 * �����߳�
 */
DWORD WINAPI CClientDlg::WorkerThread( void *pParam )
{
	CClientDlg *pClientDlg = (CClientDlg*)pParam;//������ָ��
	while (pClientDlg->m_bRunning)
	{
		
		DWORD dwIndex;				//����ֵ
		DWORD dwFlags;				//��־
		DWORD dwBytesTraned;		//ʵ�ʴ��������	
		LPWSAOVERLAPPED pIO= NULL;	//�ص��ṹָ��
		//�ȴ��¼�
		if ((dwIndex = WSAWaitForMultipleEvents(pClientDlg->m_nEventTotal,
												pClientDlg->m_arrEvent,
												FALSE,
												THREAD_SLEEP_TIME, TRUE))
												== WSA_WAIT_FAILED)
		{
			printf("WSAWaitForMultipleEvents ʧ�� %d\n", WSAGetLastError());
			return 0;
		} 
		if (WSA_WAIT_TIMEOUT == dwIndex)
		{
			continue;
		}
		//�����¼�
		WSAResetEvent(pClientDlg->m_arrEvent[dwIndex - WSA_WAIT_EVENT_0]);

		pIO= pClientDlg->MapEventToOverlapped(dwIndex - WSA_WAIT_EVENT_0);
		ASSERT(NULL != pIO);
		
		//���������״̬
		BOOL bRet = WSAGetOverlappedResult(pClientDlg->m_sHost, 
											pIO,
											&dwBytesTraned,
											TRUE, 
											&dwFlags);
		if(bRet == FALSE || dwBytesTraned == 0)//��������
		{	
			return -1;
			
		}else
		{
			//�����������
			if (pIO == &pClientDlg->m_pLookupSocket->m_iIO)
			{
				ASSERT( 1== dwIndex);
				pClientDlg->m_pLookupSocket->HandleData();
			}else
			{
				//�����������
			}
		}
	}
	
	return 0;
}

/*
 * ������ŵõ�WSAOVERLAPPEDָ��
 */
WSAOVERLAPPED * CClientDlg::MapEventToOverlapped( int nIndex )
{
	if ( 0 == nIndex)
	{
		return &m_pLookupSocket->m_oIO;//�������ݲ����ص��ṹ
	}else if ( 1 == nIndex)
	{
		return &m_pLookupSocket->m_iIO;//�������ݲ����ص��ṹ
	}else 
	{
		return NULL;
	}
}
/*
 * ��ʾ���ʺ���
 */
void CClientDlg::ShowMeaning( TCHAR *pMeaning )
{
	if (NULL == pMeaning )//�ַ���Ϊ��
	{
		return;
	}
	CString strMeaning(pMeaning);		//���ʺ���
	m_ctlMeaning.SetWindowText(_T(""));	//���ԭ���ַ���

	//�����ַ���
	CString meaning1;					//��һ������
	CString meaning2;					//�ڶ�������
	int nIndex = strMeaning.Find(">");
	if (-1 == nIndex)
	{
		return;
	}
	meaning1 = strMeaning.Mid(1,nIndex -1);
	int nLen = strMeaning.GetLength();
	meaning2 = strMeaning.Mid(nIndex + 2, nLen - meaning1.GetLength() - 4);

	CString meaning;			//��ʾ�����ַ���
	if (!meaning2.IsEmpty())	//�ڶ������岻Ϊ��
	{
		meaning ="�� " +
				meaning1 +
				"\r\n"	+		//����
				"�� " +
				meaning2;
	}else						//�ڶ�������Ϊ��
	{
		meaning ="�� " +
				meaning1;
	}

	m_ctlMeaning.SetWindowText(meaning);//��ʾ����
}


/*
 * �ж��Ƿ�Ϊ����
 */
BOOL CClientDlg::IsWord( char * pExp, CString &strWord )
{
	int nCount = strlen(pExp);	//�ַ�����
	int nLen = 0;
	char *pTemp = pExp;
	while (*pTemp == ' ')		//�޳�ǰ��Ŀո�
	{
		nLen++;
		pTemp++;
	}	
	char *pStartWord = pTemp;	//�ַ���ʼλ��

	//����ѭ��������:1.�����ո� 2.�ַ�������
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
	int nNum = pTemp - pStartWord;		//��Ч�ַ�����
	CString strTemp(pStartWord, nNum);	//��ʱ����
	strWord = strTemp;
	return TRUE;
}

/*
 * ��ʼ��ѯ����
 */
void CClientDlg::OnOK() 
{
	OnLookupButton();
}

/*
 * �˳�
 */
void CClientDlg::OnCancel() 
{
	m_bRunning = FALSE;								//���ÿͻ�������״̬
	WaitForSingleObject(m_hWorkerThread, INFINITE);	//�ȴ������߳��˳�
	delete m_pLookupSocket;							//ɾ��CLookupSocketָ��
	m_pLookupSocket = NULL;
	WSACleanup();									//�ͷ��׽�����Դ
	CDialog::OnCancel();							//���ٶԻ���
}
