// ServerView.cpp : implementation of the CServerView class
//

#include "stdafx.h"
#include "Server.h"

#include "ServerDoc.h"
#include "ServerView.h"
#include "EditWordDlg.h"
#include "ServerAddrDlg.h"
#include "ClientSocket.h"
#include "UnknWordDlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CServerApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CServerView

IMPLEMENT_DYNCREATE(CServerView, CListView)

BEGIN_MESSAGE_MAP(CServerView, CListView)
	//{{AFX_MSG_MAP(CServerView)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_COMMAND(ID_ADD_WORD, OnAddWord)
	ON_COMMAND(ID_EDIT_WORD, OnEditWord)
	ON_COMMAND(ID_DEL_WORD, OnDelWord)
	ON_UPDATE_COMMAND_UI(ID_ADD_WORD, OnUpdateAddWord)
	ON_UPDATE_COMMAND_UI(ID_DEL_WORD, OnUpdateDelWord)
	ON_UPDATE_COMMAND_UI(ID_EDIT_WORD, OnUpdateEditWord)
	ON_COMMAND(ID_SERVER_RUN, OnServerRun)
	ON_UPDATE_COMMAND_UI(ID_SERVER_RUN, OnUpdateServerRun)
	ON_WM_DESTROY()
	ON_COMMAND(ID_SERVER_LOOK, OnShowUnKnWordDlg)
	ON_UPDATE_COMMAND_UI(ID_SERVER_LOOK, OnUpdateShowNOTWordDlg)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER_ADDWORD, OnAddUnknWord)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServerView construction/destruction

CServerView::CServerView()
:m_bRunning(FALSE),
m_sListen(INVALID_SOCKET),
m_bShowUnknWordDlg(FALSE),
m_pUnknWordDlg(NULL),
m_hThreadService(NULL)
{
	m_pRecordSet.CreateInstance(__uuidof(Recordset));//������¼��ָ��

}

CServerView::~CServerView()
{
}

BOOL CServerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CServerView drawing

void CServerView::OnDraw(CDC* pDC)
{
	CServerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/*
 * ��ʼ��
 */
void CServerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	InitListCtrlSetting();						//��ʼ���б���ͼ�ؼ�

	UpDateListCtl();							//�����б���ͼ�ؼ�

	InitSocket();								//��ʼ���׽���

	m_pUnknWordDlg = new CUnknWordDlg(this);	//����δ֪����
	m_pUnknWordDlg->Create(CUnknWordDlg::IDD);
	m_bShowUnknWordDlg = FALSE;					//���ش���
	m_pUnknWordDlg->ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CListView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CServerDoc* CServerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CServerDoc)));
	return (CServerDoc*)m_pDocument;
}

#endif //_DEBUG


/*
 * ��ʼ���б���ͼ�ؼ�
 */
void CServerView::InitListCtrlSetting( void )
{
	CListCtrl &listCtl = GetListCtrl();							//�õ��ؼ�

	LONG lStyle = GetWindowLong(listCtl.m_hWnd, GWL_STYLE);		//��ȡ���
	lStyle |= LVS_REPORT;										//�㱨���
	SetWindowLong(listCtl.m_hWnd, GWL_STYLE, lStyle);			//����style

	DWORD dwStyle = ListView_GetExtendedListViewStyle(listCtl);	//�õ���չ���
	dwStyle|= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;			//����ѡ�����������߷��
	ListView_SetExtendedListViewStyle (listCtl, dwStyle);

	int			nCol;	//��
	LV_COLUMN	lvCol;	//�ṹ
	lvCol.mask = LVCF_FMT |LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	CRect clientRect;
	listCtl.GetClientRect(&clientRect);

	//��һ��
	nCol = 0;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Word";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 30;
	listCtl.InsertColumn(nCol, &lvCol);

	//�ڶ���
	nCol = 1;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Meaning1";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 40;
	listCtl.InsertColumn(nCol, &lvCol);

	//������
	nCol = 2;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Meaning2";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 30;
	listCtl.InsertColumn(nCol, &lvCol);
}

/*
 * �����б���ͼ�ؼ�
 */
void CServerView::UpDateListCtl( void )
{
	CListCtrl &listCtl = GetListCtrl();			//�õ��б���ͼ�ؼ�
	listCtl.DeleteAllItems();					//ɾ������������

	try 
	{
		int nIndex=0;							//��¼���		
		CString strSQL = "select * from EToC";	//��ȡ���м�¼
		m_pRecordSet->Open(_variant_t(strSQL),
							theApp.m_pConnection.GetInterfacePtr(), 
							adOpenDynamic,
							adLockOptimistic,
							adCmdText);
		
		while (!m_pRecordSet->adoEOF)//��¼β
		{
			_variant_t var;			//��ʱ����
			CString strWord;		//����
			CString strMeaning1;	//���ʺ���1
			CString strMeaning2;	//���ʺ���2
			
			//��ȡ��word���ֶ�
			var = m_pRecordSet->GetCollect("word");			
			if (var.vt != VT_NULL)
			{
				strWord = (LPCSTR)_bstr_t(var);
			}else
			{
				strWord = "";
			}

			//��ȡ��meaning1���ֶ�
			var = m_pRecordSet->GetCollect("meaning1");	
			if (var.vt != VT_NULL)
			{
				strMeaning1 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning1 = "";
			}

			//��ȡ��meaning2���ֶ�
			var = m_pRecordSet->GetCollect("meaning2");
			if (var.vt != VT_NULL)
			{
				strMeaning2 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning2 = "";
			}
			//����������			
			listCtl.InsertItem(nIndex, strWord);		//����	
			listCtl.SetItemText(nIndex,1,strMeaning1);	//����1
			listCtl.SetItemText(nIndex,2,strMeaning2);	//����2

			m_pRecordSet->MoveNext();					//�ƶ���¼
			nIndex++;									//��¼��ż�1
		}		
		m_pRecordSet->Close();//�ر�
		
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("�������ݿ������ʱ�����쳣��");
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CServerView message handlers

/*
 * �Ҽ���Ϣ��Ӧ����
 */
void CServerView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;			
	CMenu PopMenu;
	CMenu *pMenu;
	CPoint point;
	
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();//��ȡ���λ��
		point.x =  LOWORD(dwPos);
		point.y =  HIWORD(dwPos);		
		PopMenu.LoadMenu(IDR_POP_MENU);//װ�ز˵���Դ
		pMenu = PopMenu.GetSubMenu (0);
		//��ʾ��ݲ˵�
		pMenu->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON,	
								point.x, point.y, this);
	}
	*pResult = 0;
}

/*
 * ���������Ϣ��Ӧ����
 */
void CServerView::OnAddWord() 
{
	//������ӵ��ʴ���
	CEditWordDlg editWordDlg;
	if (editWordDlg.DoModal() != IDOK)//��ʾ����
	{
		return ;
	}	
	//���ʲ���Ϊ��
	if (editWordDlg.m_strWord.IsEmpty() )
	{
		AfxMessageBox("���ʲ���Ϊ�գ�");
		return;
	}
	
	//��һ�����ʺ��岻��Ϊ��
	if (editWordDlg.m_strMeaning1.IsEmpty())
	{
		AfxMessageBox("���ʺ��岻��Ϊ�գ�");
		return;
	}

	try
	{
		//���ȼ�����ݿ����Ƿ��д˼�¼
		CString strSQL = "select * from EToC where word=";
		CString strWord = "'" + editWordDlg.m_strWord + "'";
		strSQL += strWord;
		//ִ������
		m_pRecordSet->Open((_variant_t)strSQL, theApp.m_pConnection.GetInterfacePtr(),
			adOpenStatic,
			adLockOptimistic,
			adCmdText);
		
		int nCount = m_pRecordSet->GetRecordCount();//��ȡ��¼����
		if (nCount > 0)//���ݿ����д˼�¼
		{
			AfxMessageBox(_T("�õ����Ѵ��������ݿ��С�"));
			m_pRecordSet->Close();			
			return;
		}		
		
		//��Ӽ�¼
		m_pRecordSet->AddNew();
		m_pRecordSet->PutCollect("word", _variant_t(editWordDlg.m_strWord));
		m_pRecordSet->PutCollect("meaning1", _variant_t(editWordDlg.m_strMeaning1));
		m_pRecordSet->PutCollect("meaning2", _variant_t(editWordDlg.m_strMeaning2));
	
		m_pRecordSet->Update();	//��������
		m_pRecordSet->Close();	//�ر�
		
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("�������ݿ������ʱ�����쳣.");
		return;
	}
	
	UpDateListCtl();//������ͼ����
}

/*
 * �༭������Ϣ��Ӧ����
 */
void CServerView::OnEditWord() 
{
	CListCtrl&	listCtl = GetListCtrl ();//�õ��б���ͼ�ؼ�����
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	if(nItem == -1) 
	{
		AfxMessageBox("��ѡ��Ҫ�༭�ļ�¼");
		return ;
	}

	CString strWord = listCtl.GetItemText(nItem, 0);	//����
	CString strMeaning1 = listCtl.GetItemText(nItem,1);	//����1
	CString strMeaning2 = listCtl.GetItemText(nItem,2);	//����2

	//��ʼ���༭���ʴ���
	CEditWordDlg editWordDlg;
	editWordDlg.m_strWord = strWord;		//����
	editWordDlg.m_strMeaning1 = strMeaning1;//����1
	editWordDlg.m_strMeaning2 = strMeaning2;//����2

	editWordDlg.m_bWordEdit = FALSE;		//���ʱ༭����Ч

	if (IDOK != editWordDlg.DoModal())		//��ʾ����
	{
		return;
	}

	//���ʺ���1����Ϊ��
	if (editWordDlg.m_strMeaning1.IsEmpty())
	{
		AfxMessageBox("���ʵĺ��岻��Ϊ�գ�");
		return;
	}

	//�޸ļ�¼
	try
	{
		//SQL���
		CString strSQL = "Select * from EToC where word = " ;
		strSQL += "'";
		strSQL += strWord;
		strSQL += "'";
		//ִ������
		m_pRecordSet->Open(_variant_t(strSQL), 
							theApp.m_pConnection.GetInterfacePtr(),
							adOpenDynamic,
							adLockOptimistic,
							adCmdText);
		//�޸ĺ���1
		m_pRecordSet->PutCollect("meaning1", _variant_t(editWordDlg.m_strMeaning1));
		//�޸ĺ���2
		m_pRecordSet->PutCollect("meaning2", _variant_t(editWordDlg.m_strMeaning2));

		m_pRecordSet->Update();	//��������
		m_pRecordSet->Close();	//�ر�
	}

	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("�������ݿ������ʱ�����쳣.");
		return;
	}
	
	UpDateListCtl();//������ͼ
	
}

/*
 * ɾ��������Ϣ��Ӧ����
 */
void CServerView::OnDelWord() 
{
	CListCtrl&	listCtl = GetListCtrl ();//�õ��б���ͼ�ؼ�����
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	if(nItem == -1) 
	{
		AfxMessageBox("��ѡ��Ҫɾ���ļ�¼");
		return ;
	}

	try
	{
		//SQL���
		CString strSQL = "Select * from EToC where word = " ;
		CString strWord = listCtl.GetItemText(nItem, 0);		
		strSQL += "'";
		strSQL += strWord;
		strSQL += "'";

		//ɾ��ǰ��ȷ��
		if ( (AfxMessageBox("ȷʵҪɾ�� " +
			strWord+"��\n��ɾ�����ɻָ���", 
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)) == IDYES ) 
		{
			//ִ������
			m_pRecordSet->Open((_variant_t)strSQL,
								theApp.m_pConnection.GetInterfacePtr(),
								adOpenDynamic,
								adLockOptimistic,
								adCmdText);
			
			m_pRecordSet->Delete(adAffectCurrent);	//ɾ������
			m_pRecordSet->Update();					//��������
			m_pRecordSet->Close();					//�ر�
		}		
	}

	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("�������ݿ������ʱ�����쳣.");
		return;
	}
	
	UpDateListCtl();//������ͼ
}

/*
 * ���°�ť���˵���
 */
void CServerView::OnUpdateAddWord(CCmdUI* pCmdUI) 
{
	CListCtrl&	listCtl = GetListCtrl ();
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	pCmdUI->Enable( nItem != -1);

}

/*
 * ���°�ť���˵���
 */
void CServerView::OnUpdateDelWord(CCmdUI* pCmdUI) 
{
	CListCtrl&	listCtl = GetListCtrl ();
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	pCmdUI->Enable( nItem != -1);
	
}

/*
 * ���°�ť���˵���
 */
void CServerView::OnUpdateEditWord(CCmdUI* pCmdUI) 
{
	CListCtrl&	listCtl = GetListCtrl ();
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	pCmdUI->Enable( nItem != -1);
}

/*
 * ��������
 */
void CServerView::OnServerRun() 
{
	CServerAddrDlg  servAddrDlg;//������������ַ�Ի���
	if (IDOK != servAddrDlg.DoModal())
	{
		return;
	}
	
	SOCKADDR_IN	servAddr;		//��������ַ

	//���׽���
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERVERPORT);	
	if (bind(m_sListen, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("���׽��ִ��� %d\n", WSAGetLastError());
		closesocket(m_sListen);
		WSACleanup();
		return ;
	}

	//����
	if(listen(m_sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("���׽��ִ��� %d\n", WSAGetLastError());
		closesocket(m_sListen);
		WSACleanup();
		return ;
	}

	//���÷�����Ϊ����״̬
	m_bRunning = TRUE;

	//�����׽���Ϊ������ģʽ
	u_long ulUnBlocking = 1;
	int ret = ioctlsocket(m_sListen, FIONBIO , &ulUnBlocking);
	if(SOCKET_ERROR == ret)
	{
		printf("ioctlsocket %d\n", WSAGetLastError());
		closesocket(m_sListen);
		WSACleanup();
		return ;
	}

	//�����̲߳���
	PTHREADPARAM pParam = new THREADPARAM;
	pParam->hServHwnd = this->GetSafeHwnd();
	pParam->pServView = this;

	//���������߳�
	DWORD dwThreadId;
	m_hThreadService = CreateThread(NULL, 0, ServiceThread, pParam, 0, &dwThreadId);

}

/*
 * ���°�ť���˵���
 */
void CServerView::OnUpdateServerRun(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bRunning);
}


/*
 * ��ʼ���׽���
 */
BOOL CServerView::InitSocket( void )
{
	int			reVal;			//����ֵ
	WSADATA wsaData;			//��ʼ���׽��ֶ�̬��	
	if ((reVal = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("��ʼ���׽��ֶ�̬�����%d\n", WSAGetLastError());
		return FALSE;
	}
	
	//���������׽���
	if ((m_sListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 
					0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("���������׽��ִ���%d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	} 	
	return TRUE;
}

/*
 * �����̺߳���
 */
DWORD WINAPI CServerView::ServiceThread( void *pParam )
{
	PTHREADPARAM	pThreadParam = (PTHREADPARAM)pParam;
	HWND			hwnd = pThreadParam->hServHwnd;		//�����ھ��
	CServerView		*pServer = pThreadParam->pServView;	//CServerView��ָ��
	
	SOCKET sAccept = INVALID_SOCKET;					//�����׽���
	SOCKET sListen = pServer->m_sListen;				//�����׽���	
	CObList clientList;									//�ͻ�������
	
	while (pServer->m_bRunning)
	{
		//���ܿͻ�����������
		SOCKADDR addr;
		int len = sizeof(addr);
		sAccept = accept(sListen, (SOCKADDR*)&addr, &len);	
		if (INVALID_SOCKET != sAccept)
		{
			//�����ͻ��˶���
			CClientSocket * pClientS = new CClientSocket(sAccept, hwnd);
			if(FALSE == pClientS->RecvHeader())//��������
			{
				//��������
				delete pClientS;
				pClientS = NULL;
			}
			clientList.AddTail(pClientS);	
		}
		
		//˯��
		DWORD  dwRet = SleepEx(THREAD_SLEEP_TIME, TRUE);
		if (WAIT_IO_COMPLETION == dwRet)//������̱�����
		{
			
		} else if ( 0 == dwRet)			//�������ó�ʱ
		{
			//������Դ
			POSITION pos1 = NULL;
			POSITION pos2 = NULL;
			for (pos1 = clientList.GetHeadPosition(); (pos2 = pos1) != NULL;)
			{
				CClientSocket *pClientS = (CClientSocket*)clientList.GetNext(pos1);
				if (TRUE == pClientS->m_bDead )	//��ͻ��˵������Ѿ��˿�
				{
					clientList.RemoveAt(pos2);	//ɾ���ڵ�
					delete pClientS;			//ɾ��ָ��
					pClientS = NULL;
				}
			}
		}		
	}

	//�������߳��˳�ʱ��������Դ��ɾ���ͻ�����������ýڵ㡣
	POSITION pos = NULL;
	for (pos = clientList.GetHeadPosition(); pos != NULL;)
	{
		CClientSocket *pClientS = (CClientSocket*)clientList.GetNext(pos);
		delete pClientS;
		pClientS = NULL;
	}
	clientList.RemoveAll();

	delete pThreadParam;//ɾ���̲߳���
	pThreadParam = NULL;
	return 0;
}

/*
 * WM_DESTROY��Ϣ��Ӧ����
 */
void CServerView::OnDestroy() 
{
	CListView::OnDestroy();
	
	m_bRunning = FALSE;								//���÷���������״̬
	
	WaitForSingleObject(m_hThreadService, INFINITE);//�ȴ������߳��˳�
	CloseHandle(m_hThreadService);					//�ͷ���Դ
	if (m_pUnknWordDlg)								//����δ֪���ʶԻ���
	{
		delete m_pUnknWordDlg;
		m_pUnknWordDlg = NULL;
	}
	closesocket(m_sListen);							//�رռ����׽���
	WSACleanup();									//�ͷ��׽�����Դ
}

/*
 * ���δ֪����
 */
HRESULT CServerView::OnAddUnknWord(WPARAM wParam, LPARAM lParam)
{
	CString strNoWord((char*)wParam);		//δ֪����
	if (m_pUnknWordDlg)						//δ֪���ʴ���ָ��
	{
		m_pUnknWordDlg->AddWord(strNoWord);//���δ֪����
	}
	return 0;
}

/*
 * �鿴δ֪���ʴ���
 */
void CServerView::OnShowUnKnWordDlg() 
{
	if (m_pUnknWordDlg)					//CUnknWordDlgָ��
	{
		if(TRUE == m_bShowUnknWordDlg)	//���ش���
		{			
			m_pUnknWordDlg->ShowWindow(SW_HIDE);
			m_bShowUnknWordDlg = FALSE;
		}else							//��ʾ����
		{			
			m_pUnknWordDlg->ShowWindow(SW_SHOW);
			m_bShowUnknWordDlg = TRUE;
		}
		
	}else
	{}	
}

/*
 * ���°�ť���˵���
 */
void CServerView::OnUpdateShowNOTWordDlg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bShowUnknWordDlg);	
}

/*
 * ��ʾ������δ֪���ʴ���
 */
void CServerView::ShowUnKnWordDlg( void )
{
	OnShowUnKnWordDlg();
}
