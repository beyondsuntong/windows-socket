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
	void	HandleData(void);			//处理数据
	BOOL	Lookup(CString &strWord);	//查询单词含义

protected:
	BOOL	Send(void);					//发送数据
	BOOL	RecvHeader(void);			//接收包头
	BOOL	RecvPacket(void);			//接收包体


private:
	CLookupSocket();

public:
	WSAOVERLAPPED	m_oIO;		//对应于发送事件的重叠结构
	WSAOVERLAPPED	m_iIO;      //对应于接收事件的重叠结构


protected:
	CClientDlg		*m_pClientDlg;				//主窗口指针
	SOCKET			m_s;						//套接字
	CString			m_strWord;					//单词
	CString			m_strMeaning;				//单词含义
	PACKETHDR		m_hdrRecv;					//接收数据包头
};

#endif // !defined(AFX_LOOKUPSOCKET_H__EE6B100A_876F_4784_B839_05E139095BA1__INCLUDED_)
