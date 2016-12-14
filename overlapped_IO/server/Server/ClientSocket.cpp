// ClientSocket.cpp: implementation of the CClientSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Server.h"
#include "ClientSocket.h"
#include "ServerView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CServerApp theApp;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientSocket::CClientSocket(SOCKET s, HWND  hServHwnd)
:m_bDead(FALSE)
{
	m_s = s;									//套接字
	ZeroMemory(&m_iIO, sizeof(m_iIO));			//重叠结构
	ZeroMemory(&m_oIO, sizeof(m_oIO));			//重叠结构
	m_pRecordSet.CreateInstance(__uuidof(Recordset));//创建记录集指针
	m_hServHwnd = hServHwnd;

}

CClientSocket::~CClientSocket()
{
	closesocket(m_s);
}

/*
 * 发送数据
 */
BOOL CClientSocket::Send(void)
{
	DWORD	dwBytesRecved;				//接收字节数
	DWORD	dwFlags = 0;				//标志
	
	ZeroMemory(&m_oIO, sizeof(m_oIO));	//重叠结构
	m_oIO.hEvent = WSAEVENT(this);

	WSABUF wsaSendBuf[2];				//WSABUF结构数组
	//定义数据包头
	PACKETHDR hdr;
	hdr.type = ETOC;						//数据包类型
	hdr.len = m_strMeaning.GetLength();		//数据的长度

	wsaSendBuf[0].buf = (char*)&hdr;		//发送数据缓冲区，包头缓冲区
	wsaSendBuf[0].len = HEADERLEN;			//缓冲区的长度， 包头缓冲区长度

	wsaSendBuf[1].buf = (char*)(LPCTSTR)m_strMeaning;	//发送数据包体
	wsaSendBuf[1].len = m_strMeaning.GetLength();		//数据长度

	//异步发送数据，ORoutine为完成例程
	if(SOCKET_ERROR == WSASend(m_s, wsaSendBuf, 2, 
		&dwBytesRecved, dwFlags, &m_oIO, ORoutine))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * 处理数据
 */
void CClientSocket::HandleData( void )
{
	if (m_hdrRecv.len != 0)					//接收包头完毕
	{
		RecvPacket();						//接收数据包体
		ZeroMemory(&m_hdrRecv, HEADERLEN);	//包头置0
	}else									//接收数据体完毕
	{
		ASSERT(m_hdrRecv.len == 0 && m_hdrRecv.type == ETOC);		
		
		//在数据库中查找单词的含义
		m_strMeaning.Empty();
		CString strMeanging1;
		CString strMeanging2;
		Lookup((char*)(LPCSTR)m_strWord, strMeanging1, strMeanging2);

		//打包数据
		m_strMeaning ="<" +	strMeanging1 + ">" + "<" + strMeanging2 +">";

		m_strWord.ReleaseBuffer();
		m_strWord.Empty();

		//向客户端发送数据
 		Send();
		
		//投递另一个异步IO
		RecvHeader();
	}
}

/*
 * 读操作完成例程
 */
void CALLBACK CClientSocket::IRoutine( DWORD Error,  DWORD BytesTransferred,
									  LPWSAOVERLAPPED Overlapped,   DWORD InFlags )
{
	CClientSocket *pClient = NULL;
	pClient = (CClientSocket*)Overlapped->hEvent;	//客户端指针
	//处理数据
	if (Error != 0 || BytesTransferred == 0)		//错误处理
	{
		pClient->m_bDead = TRUE;					//从链表中删除
		return;
	}
	pClient->HandleData();							//处理数据
}

/*
 * 写操作完成例程
 */
void CALLBACK CClientSocket::ORoutine( DWORD Error,  DWORD BytesTransferred,   
									  LPWSAOVERLAPPED Overlapped,   DWORD InFlags )
{
	CClientSocket *pClient = NULL;					//客户端指针
	pClient = (CClientSocket*)Overlapped->hEvent;
	if (Error != 0 || BytesTransferred == 0)		//错误处理
	{
		pClient->m_bDead = TRUE;					//删除该客户端
	}
}

/*
 * 接收数据包头
 */
BOOL CClientSocket::RecvHeader( void )
{
	DWORD	dwBytesRecved;				//接收字节数
	DWORD	dwFlags = 0;				//标志	
		
	ZeroMemory(&m_iIO, sizeof(m_iIO));	//重叠结构
	m_iIO.hEvent = WSAEVENT(this);

	ZeroMemory(&m_hdrRecv, HEADERLEN);	//包头置0

	WSABUF wsaRecv;
	wsaRecv.buf = (char*)&m_hdrRecv;
	wsaRecv.len = HEADERLEN;
	//接收包头
	if(SOCKET_ERROR == WSARecv(m_s, &wsaRecv, 1, 
		&dwBytesRecved, &dwFlags, &m_iIO, IRoutine))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)//异步调用
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * 接收数据包体
 */
BOOL CClientSocket::RecvPacket( void )
{
	DWORD	dwBytesRecved;				//接收字节数
	DWORD	dwFlags = 0;				//标志
			
	ZeroMemory(&m_iIO, sizeof(m_iIO));	//重叠结构
	m_iIO.hEvent = WSAEVENT(this);		//this指针

	WSABUF wsaRecv;						//接收数据缓冲区
	wsaRecv.buf = (char*)m_strWord.GetBuffer(m_hdrRecv.len);
	wsaRecv.len = m_hdrRecv.len;
	//异步接收数据
	if(SOCKET_ERROR == WSARecv(m_s, &wsaRecv, 1, 
		&dwBytesRecved, &dwFlags, &m_iIO, IRoutine))
	{
		int err = WSAGetLastError();
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}
	return TRUE;	
}

/*
 * 查询数据
 */
void CClientSocket::Lookup( char* pWord, CString &strMeaning1, CString &strMeaning2)
{
	BOOL bFinder = TRUE;								//找到该单词

	CString strSQL = "select * from EToC where word = ";//SQL语句
	strSQL += "'";
	CString strTemp = pWord;
	strTemp  += "'";
	strSQL += strTemp;			//select * from EToC where word = '***'

	try
	{
		//查询单词的含义
		m_pRecordSet->Open(_variant_t(strSQL), theApp.m_pConnection.GetInterfacePtr(),
			adOpenStatic  , 
			adLockOptimistic,
			adCmdText);

		int n = m_pRecordSet->GetRecordCount();	//判断是否找到该单词
		if (0 == n)								//该单词未存在于数据库中
		{
				strMeaning1 = "在词典中没有查找到该单词！";
				bFinder = FALSE;
		}else									//该单词存在于数据库中
		{	
			//含义1
			_variant_t var;
			var = m_pRecordSet->GetCollect("meaning1");			
			if (var.vt != VT_NULL)
			{
				strMeaning1 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning1 = "";
			}

			//含义2
			var = m_pRecordSet->GetCollect("meaning2");			
			if (var.vt != VT_NULL)
			{
				strMeaning2 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning2 = "";
			}
		}	
		m_pRecordSet->Close();				//关闭
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
		return ;
	}
	catch(...)
	{
		AfxMessageBox("访问数据库服务器时发生异常。");
		return;
	}

	//发送消息，保存未知单词
	if (FALSE == bFinder)
	{
		SendMessage(m_hServHwnd, WM_USER_ADDWORD, (WPARAM)pWord, NULL);
	}
}