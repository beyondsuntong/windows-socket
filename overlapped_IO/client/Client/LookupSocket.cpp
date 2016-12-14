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
	m_pClientDlg = pDlg;				//������ָ��
	m_s = s;
	ZeroMemory(&m_oIO, sizeof(m_oIO));	//��Ӧ�ڷ����¼����ص��ṹ
	m_oIO.hEvent = hEventOut;
	ZeroMemory(&m_iIO, sizeof(m_iIO));	//��Ӧ�ڽ����¼����ص��ṹ
	m_iIO.hEvent = hEventIn;

	ZeroMemory(&m_hdrRecv, HEADERLEN);	//��ͷ
}
CLookupSocket::~CLookupSocket()
{
	closesocket(m_s);
	WSACloseEvent(m_iIO.hEvent);
	WSACloseEvent(m_oIO.hEvent);
	m_s = INVALID_SOCKET;
}

/*
 * ��������
 */
BOOL CLookupSocket::Send( void )
{
	DWORD	dwNumberOfBytesSent;	//���������ֽ��� 
	DWORD	dwFlags = 0;			//��־
	int		nErrCode;				//����ֵ

	if (m_strWord.IsEmpty())
	{
		return TRUE;
	}

	WSABUF wsaSendBuf[2];
	//�������ݰ�ͷ
	PACKETHDR hdr;
	hdr.type = ETOC;					//���ݰ�����
	hdr.len = m_strWord.GetLength();	//���ݳ���
	wsaSendBuf[0].buf = (char*)&hdr;	//��ͷ������
	wsaSendBuf[0].len = HEADERLEN;		//��ͷ����������

	wsaSendBuf[1].buf = (char*)(LPCTSTR)m_strWord;	//����
	wsaSendBuf[1].len = m_strWord.GetLength();		//���ݳ���

	nErrCode = WSASend(m_s,							//��������
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
			AfxMessageBox("WSASend��������ʧ�ܣ�");
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * ��ѯ���ʺ���
 */
BOOL CLookupSocket::Lookup( CString &strWord )
{
	m_strWord.Empty();
	m_strWord = strWord;
	if (m_strWord.IsEmpty())		//���ʲ���
	{
		return TRUE;
	}
	if (FALSE == Send())			//��������
	{
		return FALSE;
	}
	if (FALSE == RecvHeader())		//�������ݰ�ͷ
	{
		return FALSE;
	}
	return TRUE;
}

/*
 * ��������
 */
void CLookupSocket::HandleData()
{
	if (m_hdrRecv.len != 0 )//�������ݰ�ͷ
	{
		RecvPacket();		//���հ���
		ZeroMemory(&m_hdrRecv, HEADERLEN);
	}else	//�������������
	{
			//��ʾ���ʺ���
		m_pClientDlg->ShowMeaning((TCHAR*)(LPCSTR)m_strMeaning);
		m_strMeaning.ReleaseBuffer();
	}
}

/*
 * ���հ�ͷ
 */
BOOL CLookupSocket::RecvHeader( void )
{
	DWORD	dwBytesRecved;	//�����ֽ���
	DWORD	dwFlags = 0;	//��־
	
	ZeroMemory(&m_hdrRecv, HEADERLEN);//��ͷ��0

	//����������ݻ�����
	WSABUF wsaRecv;
	wsaRecv.buf = (char*)&m_hdrRecv;	
	wsaRecv.len = HEADERLEN;
	//�첽��������
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
 * ���հ���
 */
BOOL CLookupSocket::RecvPacket( void )
{
	DWORD	dwBytesRecved;	//�����ֽ���
	DWORD	dwFlags = 0;	//��־
	
	m_strMeaning.Empty();

	//����������ݻ�����
	WSABUF wsaRecv;
	wsaRecv.buf = (char*)m_strMeaning.GetBuffer(m_hdrRecv.len);
	wsaRecv.len = m_hdrRecv.len;
	//�첽��������
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