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

/**应用程序定义**/
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#define		SERVERPORT			5560						//服务器端口
#define		MAX_SIZE_BUF		1024						//数据缓冲区长度
#define		THREAD_SLEEP_TIME	100							//线程睡眠时间
#define		MAX_WAIT_EVENTS		WSA_MAXIMUM_WAIT_EVENTS		//最多等待的事件对象数量
#define		HEADERLEN			(sizeof(PACKETHDR))			//包头长度
//数据包头声明
#define		ETOC				0							//英译汉
#define		CTOE				1							//汉译英
typedef struct _packethdr
{
	u_short	type;	//类型
	u_short	len;	//数据包长度(包体)
}PACKETHDR, *PPACKETHDR;

// 线程函数参数
// typedef struct _threadparam
// {
// 	HWND	hServHwnd;		//主窗口句柄
// 	BOOL	*pRunning;		//线程运行状态
// 	WORD	*pTotalEvent;	//事件对象数量
// 	WSAEVENT*pArrEvent;		//事件对象数组
// 	SOCKET	*pArrSocket;	//套接字句柄数组
// 	HANDLE	hEventExit;		//线程退出事件句柄
// }THREADPARAM, *PTHREADPARAM;


#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__076D3031_15BA_4025_9238_E627A182E374__INCLUDED_)
