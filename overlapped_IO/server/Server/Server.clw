; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CEditWordDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "Server.h"
LastPage=0

ClassCount=7
Class1=CServerApp
Class2=CServerDoc
Class3=CServerView
Class4=CMainFrame

ResourceCount=6
Resource1=IDD_NOWORD_DLG
Resource2=IDR_POP_MENU
Resource3=IDD_ABOUTBOX
Class5=CAboutDlg
Class6=CEditWordDlg
Resource4=IDD_WORD_DLG
Resource5=IDR_MAINFRAME
Class7=CNotWordDlg
Resource6=IDD_SERVER_ADDRESS_DLG (English (U.S.))

[CLS:CServerApp]
Type=0
HeaderFile=Server.h
ImplementationFile=Server.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC

[CLS:CServerDoc]
Type=0
HeaderFile=ServerDoc.h
ImplementationFile=ServerDoc.cpp
Filter=N

[CLS:CServerView]
Type=0
HeaderFile=ServerView.h
ImplementationFile=ServerView.cpp
Filter=C
BaseClass=CListView
VirtualFilter=VWC
LastObject=CServerView


[CLS:CMainFrame]
Type=0
HeaderFile=MainFrm.h
ImplementationFile=MainFrm.cpp
Filter=T




[CLS:CAboutDlg]
Type=0
HeaderFile=Server.cpp
ImplementationFile=Server.cpp
Filter=D
LastObject=CAboutDlg

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[MNU:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_ADD_WORD
Command2=ID_EDIT_WORD
Command3=ID_DEL_WORD
Command4=ID_SERVER_RUN
Command5=ID_APP_EXIT
Command6=ID_SERVER_LOOK
Command7=ID_APP_ABOUT
CommandCount=7

[ACL:IDR_MAINFRAME]
Type=1
Class=CMainFrame
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_UNDO
Command5=ID_EDIT_CUT
Command6=ID_EDIT_COPY
Command7=ID_EDIT_PASTE
Command8=ID_EDIT_UNDO
Command9=ID_EDIT_CUT
Command10=ID_EDIT_COPY
Command11=ID_EDIT_PASTE
Command12=ID_NEXT_PANE
Command13=ID_PREV_PANE
CommandCount=13

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_ADD_WORD
Command2=ID_EDIT_WORD
Command3=ID_DEL_WORD
Command4=ID_SERVER_RUN
Command5=ID_SERVER_LOOK
CommandCount=5

[MNU:IDR_POP_MENU]
Type=1
Class=?
Command1=ID_ADD_WORD
Command2=ID_EDIT_WORD
Command3=ID_DEL_WORD
CommandCount=3

[DLG:IDD_WORD_DLG]
Type=1
Class=CEditWordDlg
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT_WORD,edit,1350631552
Control5=IDC_EDIT_MEANING1,edit,1350631552
Control6=IDC_EDIT_MEANING2,edit,1350631552
Control7=IDC_STATIC,button,1342177287
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_STATIC,button,1342177287

[CLS:CEditWordDlg]
Type=0
HeaderFile=EditWordDlg.h
ImplementationFile=EditWordDlg.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=CEditWordDlg

[DLG:IDD_SERVER_ADDRESS_DLG (English (U.S.))]
Type=1
Class=?
ControlCount=7
Control1=IDC_SERVER_IPADDRESS,SysIPAddress32,1342242816
Control2=IDC_SERVER_PORT_EDIT,edit,1350639744
Control3=IDOK,button,1342242816
Control4=IDCANCEL,button,1342242816
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,button,1342177287
Control7=IDC_STATIC,static,1342308352

[DLG:IDD_NOWORD_DLG]
Type=1
Class=CNotWordDlg
ControlCount=1
Control1=IDC_SERVER_NOWORD_LIST,SysListView32,1350633477

[CLS:CNotWordDlg]
Type=0
HeaderFile=NotWordDlg.h
ImplementationFile=NotWordDlg.cpp
BaseClass=CDialog
Filter=D
LastObject=CNotWordDlg
VirtualFilter=dWC

