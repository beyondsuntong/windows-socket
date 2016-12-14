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
	m_s = s;									//�׽���
	ZeroMemory(&m_iIO, sizeof(m_iIO));			//�ص��ṹ
	ZeroMemory(&m_oIO, sizeof(m_oIO));			//�ص��ṹ
	m_pRecordSet.CreateInstance(__uuidof(Recordset));//������¼��ָ��
	m_hServHwnd = hServHwnd;

}

CClientSocket::~CClientSocket()
{
	closesocket(m_s);
}

/*
 * ��������
 */
BOOL CClientSocket::Send(void)
{
	DWORD	dwBytesRecved;				//�����ֽ���
	DWORD	dwFlags = 0;				//��־
	
	ZeroMemory(&m_oIO, sizeof(m_oIO));	//�ص��ṹ
	m_oIO.hEvent = WSAEVENT(this);

	WSABUF wsaSendBuf[2];				//WSABUF�ṹ����
	//�������ݰ�ͷ
	PACKETHDR hdr;
	hdr.type = ETOC;						//���ݰ�����
	hdr.len = m_strMeaning.GetLength();		//���ݵĳ���

	wsaSendBuf[0].buf = (char*)&hdr;		//�������ݻ���������ͷ������
	wsaSendBuf[0].len = HEADERLEN;			//�������ĳ��ȣ� ��ͷ����������

	wsaSendBuf[1].buf = (char*)(LPCTSTR)m_strMeaning;	//�������ݰ���
	wsaSendBuf[1].len = m_strMeaning.GetLength();		//���ݳ���

	//�첽�������ݣ�ORoutineΪ�������
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
 * ��������
 */
void CClientSocket::HandleData( void )
{
	if (m_hdrRecv.len != 0)					//���հ�ͷ���
	{
		RecvPacket();						//�������ݰ���
		ZeroMemory(&m_hdrRecv, HEADERLEN);	//��ͷ��0
	}else									//�������������
	{
		ASSERT(m_hdrRecv.len == 0 && m_hdrRecv.type == ETOC);		
		
		//�����ݿ��в��ҵ��ʵĺ���
		m_strMeaning.Empty();
		CString strMeanging1;
		CString strMeanging2;
		Lookup((char*)(LPCSTR)m_strWord, strMeanging1, strMeanging2);

		//�������
		m_strMeaning ="<" +	strMeanging1 + ">" + "<" + strMeanging2 +">";

		m_strWord.ReleaseBuffer();
		m_strWord.Empty();

		//��ͻ��˷�������
 		Send();
		
		//Ͷ����һ���첽IO
		RecvHeader();
	}
}

/*
 * �������������
 */
void CALLBACK CClientSocket::IRoutine( DWORD Error,  DWORD BytesTransferred,
									  LPWSAOVERLAPPED Overlapped,   DWORD InFlags )
{
	CClientSocket *pClient = NULL;
	pClient = (CClientSocket*)Overlapped->hEvent;	//�ͻ���ָ��
	//��������
	if (Error != 0 || BytesTransferred == 0)		//������
	{
		pClient->m_bDead = TRUE;					//��������ɾ��
		return;
	}
	pClient->HandleData();							//��������
}

/*
 * д�����������
 */
void CALLBACK CClientSocket::ORoutine( DWORD Error,  DWORD BytesTransferred,   
									  LPWSAOVERLAPPED Overlapped,   DWORD InFlags )
{
	CClientSocket *pClient = NULL;					//�ͻ���ָ��
	pClient = (CClientSocket*)Overlapped->hEvent;
	if (Error != 0 || BytesTransferred == 0)		//������
	{
		pClient->m_bDead = TRUE;					//ɾ���ÿͻ���
	}
}

/*
 * �������ݰ�ͷ
 */
BOOL CClientSocket::RecvHeader( void )
{
	DWORD	dwBytesRecved;				//�����ֽ���
	DWORD	dwFlags = 0;				//��־	
		
	ZeroMemory(&m_iIO, sizeof(m_iIO));	//�ص��ṹ
	m_iIO.hEvent = WSAEVENT(this);

	ZeroMemory(&m_hdrRecv, HEADERLEN);	//��ͷ��0

	WSABUF wsaRecv;
	wsaRecv.buf = (char*)&m_hdrRecv;
	wsaRecv.len = HEADERLEN;
	//���հ�ͷ
	if(SOCKET_ERROR == WSARecv(m_s, &wsaRecv, 1, 
		&dwBytesRecved, &dwFlags, &m_iIO, IRoutine))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)//�첽����
		{
			return FALSE;
		}
	}
	return TRUE;
}

/*
 * �������ݰ���
 */
BOOL CClientSocket::RecvPacket( void )
{
	DWORD	dwBytesRecved;				//�����ֽ���
	DWORD	dwFlags = 0;				//��־
			
	ZeroMemory(&m_iIO, sizeof(m_iIO));	//�ص��ṹ
	m_iIO.hEvent = WSAEVENT(this);		//thisָ��

	WSABUF wsaRecv;						//�������ݻ�����
	wsaRecv.buf = (char*)m_strWord.GetBuffer(m_hdrRecv.len);
	wsaRecv.len = m_hdrRecv.len;
	//�첽��������
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
 * ��ѯ����
 */
void CClientSocket::Lookup( char* pWord, CString &strMeaning1, CString &strMeaning2)
{
	BOOL bFinder = TRUE;								//�ҵ��õ���

	CString strSQL = "select * from EToC where word = ";//SQL���
	strSQL += "'";
	CString strTemp = pWord;
	strTemp  += "'";
	strSQL += strTemp;			//select * from EToC where word = '***'

	try
	{
		//��ѯ���ʵĺ���
		m_pRecordSet->Open(_variant_t(strSQL), theApp.m_pConnection.GetInterfacePtr(),
			adOpenStatic  , 
			adLockOptimistic,
			adCmdText);

		int n = m_pRecordSet->GetRecordCount();	//�ж��Ƿ��ҵ��õ���
		if (0 == n)								//�õ���δ���������ݿ���
		{
				strMeaning1 = "�ڴʵ���û�в��ҵ��õ��ʣ�";
				bFinder = FALSE;
		}else									//�õ��ʴ��������ݿ���
		{	
			//����1
			_variant_t var;
			var = m_pRecordSet->GetCollect("meaning1");			
			if (var.vt != VT_NULL)
			{
				strMeaning1 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning1 = "";
			}

			//����2
			var = m_pRecordSet->GetCollect("meaning2");			
			if (var.vt != VT_NULL)
			{
				strMeaning2 = (LPCSTR)_bstr_t(var);
			}else
			{
				strMeaning2 = "";
			}
		}	
		m_pRecordSet->Close();				//�ر�
	}
	catch(_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
		return ;
	}
	catch(...)
	{
		AfxMessageBox("�������ݿ������ʱ�����쳣��");
		return;
	}

	//������Ϣ������δ֪����
	if (FALSE == bFinder)
	{
		SendMessage(m_hServHwnd, WM_USER_ADDWORD, (WPARAM)pWord, NULL);
	}
}