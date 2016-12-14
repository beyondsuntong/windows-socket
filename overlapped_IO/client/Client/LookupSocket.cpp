// LookupSocket.cpp: implementation of the CLookupSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Client.h"
#include "LookupSocket.h"
#include "ClientDlg.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLookupSocket::CLookupSocket( CClientDlg *pDlg, SOCKET s ,
							 WSAEVENT hEventOut, WSAEVENT hEventIn)
{
	m_pClientDlg = pDlg;				//主窗口指针
	m_s = s;
	ZeroMemory(&m_oIO, sizeof(m_oIO));	//对应于发送事件的重叠结构
	m_oIO.hEvent = hEventOut;
	ZeroMemory(&m_iIO, sizeof(m_iIO));	//对应于接收事件的重叠结构
	m_iIO.hEvent = hEventIn;

	ZeroMemory(&m_hdrRecv, HEADERLEN);	//包头
}
CLookupSocket::~CLookupSocket()
{
	closesocket(m_s);
	WSACloseEvent(m_iIO.hEvent);
	WSACloseEvent(m_oIO.hEvent);
	m_s = INVALID_SOCKET;
}

/*
 * 发送数据
 */
BOOL CLookupSocket::Send( void )
{
	DWORD	dwNumberOfBytesSent;	//发送数据字节数 
	DWORD	dwFlags = 0;			//标志
	int		nErrCode;				//返回值

	if (m_strWord.IsEmpty())
	{
		return TRUE;
	}

	WSABUF wsaSendBuf[2];
	//定义数据包头
	PACKETHDR hdr;
	hdr.type = ETOC;					//数据包类型
	hdr.len = m_strWord.GetLength();	//数据长度
	wsaSendBuf[0].buf = (char*)&hdr;	//包头缓冲区
	wsaSendBuf[0].len = HEADERLEN;		//包头缓冲区长度

	wsaSendBuf[1].buf = (char*)(LPCTSTR)m_strWord;	//包体
	wsaSendBuf[1].len = m_strWord.GetLength();		//数据长度

	nErrCode = WSASend(m_s,							//发送数据
						wsaSendBuf,
						2,
						&dwNumberOfBytesSent,
						dwFlags,
						&m_oIO,
						NULL);
	if (SOCKET_ERROR == nErrCode)
	{
		if (WSA_IO_PENDING == WSAGetLastError())
		{
			return TRUE;
		}else
		{
			AfxMessageBox("WSASend函数调用失败！");
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * 查询单词含义
 */
BOOL CLookupSocket::Lookup( CString &strWord )
{
	m_strWord.Empty();
	m_strWord = strWord;
	if (m_strWord.IsEmpty())		//单词不空
	{
		return TRUE;
	}
	if (FALSE == Send())			//发送数据
	{
		return FALSE;
	}
	if (FALSE == RecvHeader())		//接收数据包头
	{
		return FALSE;
	}
	return TRUE;
}

/*
 * 处理数据
 */
void CLookupSocket::HandleData()
{
	if (m_hdrRecv.len != 0 )//接收数据包头
	{
		RecvPacket();		//接收包体
		ZeroMemory(&m_hdrRecv, HEADERLEN);
	}else	//接收数包体完毕
	{
			//显示单词含义
		m_pClientDlg->ShowMeaning((TCHAR*)(LPCSTR)m_strMeaning);
		m_strMeaning.ReleaseBuffer();
	}
}

/*
 * 接收包头
 */
BOOL CLookupSocket::RecvHeader( void )
{
	DWORD	dwBytesRecved;	//接收字节数
	DWORD	dwFlags = 0;	//标志
	
	ZeroMemory(&m_hdrRecv, HEADERLEN);//包头置0

	//定义接收数据缓冲区
	WSABUF wsaRecv;
	wsaRecv.buf = (char*)&m_hdrRecv;	
	wsaRecv.len = HEADERLEN;
	//异步接收数据
	if(SOCKET_ERROR == WSARecv(m_s, 
								&wsaRecv, 
								1,
								&dwBytesRecved,
								&dwFlags,
								&m_iIO,
								NULL))
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
 * 接收包体
 */
BOOL CLookupSocket::RecvPacket( void )
{
	DWORD	dwBytesRecved;	//接收字节数
	DWORD	dwFlags = 0;	//标志
	
	m_strMeaning.Empty();

	//定义接收数据缓冲区
	WSABUF wsaRecv;
	wsaRecv.buf = (char*)m_strMeaning.GetBuffer(m_hdrRecv.len);
	wsaRecv.len = m_hdrRecv.len;
	//异步接收数据
	if(SOCKET_ERROR == WSARecv(m_s, 
								&wsaRecv, 
								1,
								&dwBytesRecved,
								&dwFlags,
								&m_iIO,
								NULL))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return FALSE;
		}
	}
	return TRUE;
}