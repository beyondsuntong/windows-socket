// LookupSocket.h: interface for the CLookupSocket class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOOKUPSOCKET_H__EE6B100A_876F_4784_B839_05E139095BA1__INCLUDED_)
#define AFX_LOOKUPSOCKET_H__EE6B100A_876F_4784_B839_05E139095BA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CClientDlg;
//struct PACKETHDR;
class CLookupSocket : public CObject  
{
public:
	CLookupSocket(CClientDlg *pDlg, SOCKET s, WSAEVENT hEventOut, WSAEVENT hEventIn);
	virtual ~CLookupSocket();


public:
	void	HandleData(void);			//��������
	BOOL	Lookup(CString &strWord);	//��ѯ���ʺ���

protected:
	BOOL	Send(void);					//��������
	BOOL	RecvHeader(void);			//���հ�ͷ
	BOOL	RecvPacket(void);			//���հ���


private:
	CLookupSocket();

public:
	WSAOVERLAPPED	m_oIO;		//��Ӧ�ڷ����¼����ص��ṹ
	WSAOVERLAPPED	m_iIO;      //��Ӧ�ڽ����¼����ص��ṹ


protected:
	CClientDlg		*m_pClientDlg;				//������ָ��
	SOCKET			m_s;						//�׽���
	CString			m_strWord;					//����
	CString			m_strMeaning;				//���ʺ���
	PACKETHDR		m_hdrRecv;					//�������ݰ�ͷ
};

#endif // !defined(AFX_LOOKUPSOCKET_H__EE6B100A_876F_4784_B839_05E139095BA1__INCLUDED_)
