// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__076D3031_15BA_4025_9238_E627A182E374__INCLUDED_)
#define AFX_STDAFX_H__076D3031_15BA_4025_9238_E627A182E374__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls

/**Ӧ�ó�����**/
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#define		SERVERPORT			5560						//�������˿�
#define		MAX_SIZE_BUF		1024						//���ݻ���������
#define		THREAD_SLEEP_TIME	100							//�߳�˯��ʱ��
#define		MAX_WAIT_EVENTS		WSA_MAXIMUM_WAIT_EVENTS		//���ȴ����¼���������
#define		HEADERLEN			(sizeof(PACKETHDR))			//��ͷ����
//���ݰ�ͷ����
#define		ETOC				0							//Ӣ�뺺
#define		CTOE				1							//����Ӣ
typedef struct _packethdr
{
	u_short	type;	//����
	u_short	len;	//���ݰ�����(����)
}PACKETHDR, *PPACKETHDR;

// �̺߳�������
// typedef struct _threadparam
// {
// 	HWND	hServHwnd;		//�����ھ��
// 	BOOL	*pRunning;		//�߳�����״̬
// 	WORD	*pTotalEvent;	//�¼���������
// 	WSAEVENT*pArrEvent;		//�¼���������
// 	SOCKET	*pArrSocket;	//�׽��־������
// 	HANDLE	hEventExit;		//�߳��˳��¼����
// }THREADPARAM, *PTHREADPARAM;


#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__076D3031_15BA_4025_9238_E627A182E374__INCLUDED_)
