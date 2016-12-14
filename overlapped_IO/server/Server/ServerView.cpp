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
	m_pRecordSet.CreateInstance(__uuidof(Recordset));//创建记录集指针

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
 * 初始化
 */
void CServerView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	InitListCtrlSetting();						//初始化列表视图控件

	UpDateListCtl();							//更新列表视图控件

	InitSocket();								//初始化套接字

	m_pUnknWordDlg = new CUnknWordDlg(this);	//创建未知窗口
	m_pUnknWordDlg->Create(CUnknWordDlg::IDD);
	m_bShowUnknWordDlg = FALSE;					//隐藏窗口
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
 * 初始化列表视图控件
 */
void CServerView::InitListCtrlSetting( void )
{
	CListCtrl &listCtl = GetListCtrl();							//得到控件

	LONG lStyle = GetWindowLong(listCtl.m_hWnd, GWL_STYLE);		//获取风格
	lStyle |= LVS_REPORT;										//汇报风格
	SetWindowLong(listCtl.m_hWnd, GWL_STYLE, lStyle);			//设置style

	DWORD dwStyle = ListView_GetExtendedListViewStyle(listCtl);	//得到扩展风格
	dwStyle|= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;			//增加选择整行网格线风格
	ListView_SetExtendedListViewStyle (listCtl, dwStyle);

	int			nCol;	//列
	LV_COLUMN	lvCol;	//结构
	lvCol.mask = LVCF_FMT |LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	CRect clientRect;
	listCtl.GetClientRect(&clientRect);

	//第一列
	nCol = 0;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Word";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 30;
	listCtl.InsertColumn(nCol, &lvCol);

	//第二列
	nCol = 1;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Meaning1";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 40;
	listCtl.InsertColumn(nCol, &lvCol);

	//第三列
	nCol = 2;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Meaning2";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 30;
	listCtl.InsertColumn(nCol, &lvCol);
}

/*
 * 更新列表视图控件
 */
void CServerView::UpDateListCtl( void )
{
	CListCtrl &listCtl = GetListCtrl();			//得到列表视图控件
	listCtl.DeleteAllItems();					//删除所有数据项

	try 
	{
		int nIndex=0;							//记录序号		
		CString strSQL = "select * from EToC";	//读取所有记录
		m_pRecordSet->Open(_variant_t(strSQL),
							theApp.m_pConnection.GetInterfacePtr(), 
							adOpenDynamic,
							adLockOptimistic,
							adCmdText);
		
		while (!m_pRecordSet->adoEOF)//记录尾
		{
			_variant_t var;			//临时变量
			CString strWord;		//单词
			CString strMeaning1;	//单词含义1
			CString strMeaning2;	//单词含义2
			
			//读取“word”字段
			var = m_pRecordSet->GetCollect("word");			
			if (var.vt != VT_NULL)
			{
				strWord = (LPCSTR)_bstr_t(var);
			}else
			{
				strWord = "";
			}

			//读取“meaning1”字段
			var = m_pRecordSet->GetCollect("meaning1");	
			if (var.vt != VT_NULL)
			{
				strMeaning1 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning1 = "";
			}

			//读取“meaning2”字段
			var = m_pRecordSet->GetCollect("meaning2");
			if (var.vt != VT_NULL)
			{
				strMeaning2 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning2 = "";
			}
			//插入数据项			
			listCtl.InsertItem(nIndex, strWord);		//名称	
			listCtl.SetItemText(nIndex,1,strMeaning1);	//含义1
			listCtl.SetItemText(nIndex,2,strMeaning2);	//含义2

			m_pRecordSet->MoveNext();					//移动记录
			nIndex++;									//记录序号加1
		}		
		m_pRecordSet->Close();//关闭
		
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("访问数据库服务器时发生异常。");
		return;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CServerView message handlers

/*
 * 右键消息响应函数
 */
void CServerView::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;			
	CMenu PopMenu;
	CMenu *pMenu;
	CPoint point;
	
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();//获取鼠标位置
		point.x =  LOWORD(dwPos);
		point.y =  HIWORD(dwPos);		
		PopMenu.LoadMenu(IDR_POP_MENU);//装载菜单资源
		pMenu = PopMenu.GetSubMenu (0);
		//显示快捷菜单
		pMenu->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON,	
								point.x, point.y, this);
	}
	*pResult = 0;
}

/*
 * 添加数据消息响应函数
 */
void CServerView::OnAddWord() 
{
	//创建添加单词窗口
	CEditWordDlg editWordDlg;
	if (editWordDlg.DoModal() != IDOK)//显示窗口
	{
		return ;
	}	
	//单词不能为空
	if (editWordDlg.m_strWord.IsEmpty() )
	{
		AfxMessageBox("单词不能为空！");
		return;
	}
	
	//第一个单词含义不能为空
	if (editWordDlg.m_strMeaning1.IsEmpty())
	{
		AfxMessageBox("单词含义不能为空！");
		return;
	}

	try
	{
		//首先检查数据库中是否有此记录
		CString strSQL = "select * from EToC where word=";
		CString strWord = "'" + editWordDlg.m_strWord + "'";
		strSQL += strWord;
		//执行命令
		m_pRecordSet->Open((_variant_t)strSQL, theApp.m_pConnection.GetInterfacePtr(),
			adOpenStatic,
			adLockOptimistic,
			adCmdText);
		
		int nCount = m_pRecordSet->GetRecordCount();//获取记录数量
		if (nCount > 0)//数据库中有此记录
		{
			AfxMessageBox(_T("该单词已存在在数据库中。"));
			m_pRecordSet->Close();			
			return;
		}		
		
		//添加记录
		m_pRecordSet->AddNew();
		m_pRecordSet->PutCollect("word", _variant_t(editWordDlg.m_strWord));
		m_pRecordSet->PutCollect("meaning1", _variant_t(editWordDlg.m_strMeaning1));
		m_pRecordSet->PutCollect("meaning2", _variant_t(editWordDlg.m_strMeaning2));
	
		m_pRecordSet->Update();	//更新数据
		m_pRecordSet->Close();	//关闭
		
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("访问数据库服务器时发生异常.");
		return;
	}
	
	UpDateListCtl();//更新视图界面
}

/*
 * 编辑数据消息响应函数
 */
void CServerView::OnEditWord() 
{
	CListCtrl&	listCtl = GetListCtrl ();//得到列表视图控件对象
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	if(nItem == -1) 
	{
		AfxMessageBox("请选择要编辑的纪录");
		return ;
	}

	CString strWord = listCtl.GetItemText(nItem, 0);	//单词
	CString strMeaning1 = listCtl.GetItemText(nItem,1);	//含义1
	CString strMeaning2 = listCtl.GetItemText(nItem,2);	//含义2

	//初始化编辑单词窗口
	CEditWordDlg editWordDlg;
	editWordDlg.m_strWord = strWord;		//单词
	editWordDlg.m_strMeaning1 = strMeaning1;//含义1
	editWordDlg.m_strMeaning2 = strMeaning2;//含义2

	editWordDlg.m_bWordEdit = FALSE;		//单词编辑框无效

	if (IDOK != editWordDlg.DoModal())		//显示窗口
	{
		return;
	}

	//单词含义1不能为空
	if (editWordDlg.m_strMeaning1.IsEmpty())
	{
		AfxMessageBox("单词的含义不能为空！");
		return;
	}

	//修改记录
	try
	{
		//SQL语句
		CString strSQL = "Select * from EToC where word = " ;
		strSQL += "'";
		strSQL += strWord;
		strSQL += "'";
		//执行命令
		m_pRecordSet->Open(_variant_t(strSQL), 
							theApp.m_pConnection.GetInterfacePtr(),
							adOpenDynamic,
							adLockOptimistic,
							adCmdText);
		//修改含义1
		m_pRecordSet->PutCollect("meaning1", _variant_t(editWordDlg.m_strMeaning1));
		//修改含义2
		m_pRecordSet->PutCollect("meaning2", _variant_t(editWordDlg.m_strMeaning2));

		m_pRecordSet->Update();	//更新数据
		m_pRecordSet->Close();	//关闭
	}

	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("访问数据库服务器时发生异常.");
		return;
	}
	
	UpDateListCtl();//更新视图
	
}

/*
 * 删除数据消息响应函数
 */
void CServerView::OnDelWord() 
{
	CListCtrl&	listCtl = GetListCtrl ();//得到列表视图控件对象
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	if(nItem == -1) 
	{
		AfxMessageBox("请选择要删除的纪录");
		return ;
	}

	try
	{
		//SQL语句
		CString strSQL = "Select * from EToC where word = " ;
		CString strWord = listCtl.GetItemText(nItem, 0);		
		strSQL += "'";
		strSQL += strWord;
		strSQL += "'";

		//删除前的确认
		if ( (AfxMessageBox("确实要删除 " +
			strWord+"吗？\n该删除不可恢复！", 
			MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2)) == IDYES ) 
		{
			//执行命令
			m_pRecordSet->Open((_variant_t)strSQL,
								theApp.m_pConnection.GetInterfacePtr(),
								adOpenDynamic,
								adLockOptimistic,
								adCmdText);
			
			m_pRecordSet->Delete(adAffectCurrent);	//删除数据
			m_pRecordSet->Update();					//更新数据
			m_pRecordSet->Close();					//关闭
		}		
	}

	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}
	catch(...)
	{
		AfxMessageBox("访问数据库服务器时发生异常.");
		return;
	}
	
	UpDateListCtl();//更新视图
}

/*
 * 更新按钮，菜单项
 */
void CServerView::OnUpdateAddWord(CCmdUI* pCmdUI) 
{
	CListCtrl&	listCtl = GetListCtrl ();
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	pCmdUI->Enable( nItem != -1);

}

/*
 * 更新按钮，菜单项
 */
void CServerView::OnUpdateDelWord(CCmdUI* pCmdUI) 
{
	CListCtrl&	listCtl = GetListCtrl ();
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	pCmdUI->Enable( nItem != -1);
	
}

/*
 * 更新按钮，菜单项
 */
void CServerView::OnUpdateEditWord(CCmdUI* pCmdUI) 
{
	CListCtrl&	listCtl = GetListCtrl ();
	int nItem = listCtl.GetNextItem (-1, LVNI_SELECTED);
	pCmdUI->Enable( nItem != -1);
}

/*
 * 启动服务
 */
void CServerView::OnServerRun() 
{
	CServerAddrDlg  servAddrDlg;//创建服务器地址对话框
	if (IDOK != servAddrDlg.DoModal())
	{
		return;
	}
	
	SOCKADDR_IN	servAddr;		//服务器地址

	//绑定套接字
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERVERPORT);	
	if (bind(m_sListen, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		printf("绑定套接字错误 %d\n", WSAGetLastError());
		closesocket(m_sListen);
		WSACleanup();
		return ;
	}

	//监听
	if(listen(m_sListen, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("绑定套接字错误 %d\n", WSAGetLastError());
		closesocket(m_sListen);
		WSACleanup();
		return ;
	}

	//设置服务器为运行状态
	m_bRunning = TRUE;

	//设置套结字为非阻塞模式
	u_long ulUnBlocking = 1;
	int ret = ioctlsocket(m_sListen, FIONBIO , &ulUnBlocking);
	if(SOCKET_ERROR == ret)
	{
		printf("ioctlsocket %d\n", WSAGetLastError());
		closesocket(m_sListen);
		WSACleanup();
		return ;
	}

	//创建线程参数
	PTHREADPARAM pParam = new THREADPARAM;
	pParam->hServHwnd = this->GetSafeHwnd();
	pParam->pServView = this;

	//创建服务线程
	DWORD dwThreadId;
	m_hThreadService = CreateThread(NULL, 0, ServiceThread, pParam, 0, &dwThreadId);

}

/*
 * 更新按钮，菜单项
 */
void CServerView::OnUpdateServerRun(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bRunning);
}


/*
 * 初始化套结字
 */
BOOL CServerView::InitSocket( void )
{
	int			reVal;			//返回值
	WSADATA wsaData;			//初始化套接字动态库	
	if ((reVal = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("初始化套接字动态库错误%d\n", WSAGetLastError());
		return FALSE;
	}
	
	//创建监听套接字
	if ((m_sListen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 
					0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		printf("创建监听套接字错误%d\n", WSAGetLastError());
		WSACleanup();
		return FALSE;
	} 	
	return TRUE;
}

/*
 * 服务线程函数
 */
DWORD WINAPI CServerView::ServiceThread( void *pParam )
{
	PTHREADPARAM	pThreadParam = (PTHREADPARAM)pParam;
	HWND			hwnd = pThreadParam->hServHwnd;		//主窗口句柄
	CServerView		*pServer = pThreadParam->pServView;	//CServerView类指针
	
	SOCKET sAccept = INVALID_SOCKET;					//接受套接字
	SOCKET sListen = pServer->m_sListen;				//监听套接字	
	CObList clientList;									//客户端链表
	
	while (pServer->m_bRunning)
	{
		//接受客户端连接请求
		SOCKADDR addr;
		int len = sizeof(addr);
		sAccept = accept(sListen, (SOCKADDR*)&addr, &len);	
		if (INVALID_SOCKET != sAccept)
		{
			//创建客户端对象
			CClientSocket * pClientS = new CClientSocket(sAccept, hwnd);
			if(FALSE == pClientS->RecvHeader())//接收数据
			{
				//发生错误
				delete pClientS;
				pClientS = NULL;
			}
			clientList.AddTail(pClientS);	
		}
		
		//睡眠
		DWORD  dwRet = SleepEx(THREAD_SLEEP_TIME, TRUE);
		if (WAIT_IO_COMPLETION == dwRet)//完成例程被调用
		{
			
		} else if ( 0 == dwRet)			//函数调用超时
		{
			//清理资源
			POSITION pos1 = NULL;
			POSITION pos2 = NULL;
			for (pos1 = clientList.GetHeadPosition(); (pos2 = pos1) != NULL;)
			{
				CClientSocket *pClientS = (CClientSocket*)clientList.GetNext(pos1);
				if (TRUE == pClientS->m_bDead )	//与客户端的连接已经端口
				{
					clientList.RemoveAt(pos2);	//删除节点
					delete pClientS;			//删除指针
					pClientS = NULL;
				}
			}
		}		
	}

	//服务器线程退出时，清理资源。删除客户端链表的所用节点。
	POSITION pos = NULL;
	for (pos = clientList.GetHeadPosition(); pos != NULL;)
	{
		CClientSocket *pClientS = (CClientSocket*)clientList.GetNext(pos);
		delete pClientS;
		pClientS = NULL;
	}
	clientList.RemoveAll();

	delete pThreadParam;//删除线程参数
	pThreadParam = NULL;
	return 0;
}

/*
 * WM_DESTROY消息响应函数
 */
void CServerView::OnDestroy() 
{
	CListView::OnDestroy();
	
	m_bRunning = FALSE;								//设置服务器运行状态
	
	WaitForSingleObject(m_hThreadService, INFINITE);//等待服务线程退出
	CloseHandle(m_hThreadService);					//释放资源
	if (m_pUnknWordDlg)								//销毁未知单词对话框
	{
		delete m_pUnknWordDlg;
		m_pUnknWordDlg = NULL;
	}
	closesocket(m_sListen);							//关闭监听套接字
	WSACleanup();									//释放套接字资源
}

/*
 * 添加未知单词
 */
HRESULT CServerView::OnAddUnknWord(WPARAM wParam, LPARAM lParam)
{
	CString strNoWord((char*)wParam);		//未知单词
	if (m_pUnknWordDlg)						//未知单词窗口指针
	{
		m_pUnknWordDlg->AddWord(strNoWord);//添加未知单词
	}
	return 0;
}

/*
 * 查看未知单词窗口
 */
void CServerView::OnShowUnKnWordDlg() 
{
	if (m_pUnknWordDlg)					//CUnknWordDlg指针
	{
		if(TRUE == m_bShowUnknWordDlg)	//隐藏窗口
		{			
			m_pUnknWordDlg->ShowWindow(SW_HIDE);
			m_bShowUnknWordDlg = FALSE;
		}else							//显示窗口
		{			
			m_pUnknWordDlg->ShowWindow(SW_SHOW);
			m_bShowUnknWordDlg = TRUE;
		}
		
	}else
	{}	
}

/*
 * 更新按钮，菜单项
 */
void CServerView::OnUpdateShowNOTWordDlg(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!m_bShowUnknWordDlg);	
}

/*
 * 显示与隐藏未知单词窗口
 */
void CServerView::ShowUnKnWordDlg( void )
{
	OnShowUnKnWordDlg();
}
