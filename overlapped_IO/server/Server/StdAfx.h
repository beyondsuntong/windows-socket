// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__26186446_7693_4EC3_A75E_79C5ADC120EB__INCLUDED_)
#define AFX_STDAFX_H__26186446_7693_4EC3_A75E_79C5ADC120EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls

/*应用程序定义*/
#include  <afxtempl.h> 
#include "winsock2.h"
#pragma comment(lib, "ws2_32.lib")
#import "c:\program files\common files\system\ado\msado15.dll" no_namespace rename("EOF", "adoEOF") 

#define		SERVERPORT			5560						//服务器端口
#define		THREAD_SLEEP_TIME	100							//线程睡眠时间
#define		HEADERLEN			(sizeof(PACKETHDR))			//包头长度
#define		WM_USER_ADDWORD		(WM_USER + 100)				//添加单词

//数据包头声明
#define		ETOC				0							//英译汉
#define		CTOE				1							//汉译英
typedef struct _packethdr
{
	u_short	type;	//类型
	u_short	len;	//数据包长度(包体)
}PACKETHDR, *PPACKETHDR;

class CServerView;
typedef struct _threadparam
{
	HWND		hServHwnd;	//主窗口句柄
	CServerView *pServView;	//主窗口指针
}THREADPARAM, *PTHREADPARAM;



#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__26186446_7693_4EC3_A75E_79C5ADC120EB__INCLUDED_)
