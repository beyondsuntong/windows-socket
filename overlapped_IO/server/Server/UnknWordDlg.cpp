// NotWordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "UnknWordDlg.h"
#include "ServerView.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNotWordDlg dialog


CUnknWordDlg::CUnknWordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUnknWordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNotWordDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_pServView = (CServerView*)pParent;
}


void CUnknWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotWordDlg)
	DDX_Control(pDX, IDC_SERVER_NOWORD_LIST, m_ctlList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUnknWordDlg, CDialog)
	//{{AFX_MSG_MAP(CNotWordDlg)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNotWordDlg message handlers

BOOL CUnknWordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//初始化
	InitListCtrlSetting();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*
 * 初始化
 */
void CUnknWordDlg::InitListCtrlSetting( void )
{

	DWORD dwStyle = ListView_GetExtendedListViewStyle(m_ctlList);//得到扩展风格
	dwStyle|= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;			//增加选择整行网格线风格
	ListView_SetExtendedListViewStyle (m_ctlList, dwStyle);

	int			nCol;	//列
	LV_COLUMN	lvCol;	//结构
	lvCol.mask = LVCF_FMT |LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	CRect clientRect;
	m_ctlList.GetClientRect(&clientRect);

	//第一列
	nCol = 0;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Word";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 70;
	m_ctlList.InsertColumn(nCol, &lvCol);

	//第二列
	nCol = 1;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "查询次数";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 30;
	m_ctlList.InsertColumn(nCol, &lvCol);
}

/*
 * 更新列表视图控件
 */
void CUnknWordDlg::UpdateLstCtrl( void )
{
	m_ctlList.DeleteAllItems();//删除数据项
	POSITION pos = NULL;
	int nIndex = 0;
	//取出所有“键-值”对
	for (pos = m_WordMap.GetStartPosition(); pos != NULL;)
	{
		CString strKey;//键
		int		nValue;//值
		CString strValue;
		m_WordMap.GetNextAssoc(pos, strKey, nValue);

		strValue.Format("%d", nValue);
		m_ctlList.InsertItem(nIndex, strKey);		//插入单词
		m_ctlList.SetItemText(nIndex, 1, strValue);	//插入查询次数
	}		
}


/*
 * 保存未知单词
 */
void CUnknWordDlg::AddWord( CString strWord )
{
	int value = -1;
	//在映射中查找该单词
	BOOL bFinder = m_WordMap.Lookup((LPCSTR)strWord, value);
	if (bFinder)//找到该单词，计数加1
	{		
		m_WordMap.SetAt((LPCSTR)strWord , value + 1);
	}else		//未找到该单词，计数为1
	{
		m_WordMap.SetAt((LPCSTR)strWord , 1);
	}
}


/*
 * 显示与隐藏窗口消息响应函数
 */
void CUnknWordDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (bShow == TRUE)		//显示窗口
	{
		UpdateLstCtrl();	//更新数据
	}
	CDialog::OnShowWindow(bShow, nStatus);	
}
/*
 *关闭窗口
 */
void CUnknWordDlg::OnClose() 
{
	m_pServView->ShowUnKnWordDlg();
}
