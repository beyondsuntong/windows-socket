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
	
	//��ʼ��
	InitListCtrlSetting();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/*
 * ��ʼ��
 */
void CUnknWordDlg::InitListCtrlSetting( void )
{

	DWORD dwStyle = ListView_GetExtendedListViewStyle(m_ctlList);//�õ���չ���
	dwStyle|= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;			//����ѡ�����������߷��
	ListView_SetExtendedListViewStyle (m_ctlList, dwStyle);

	int			nCol;	//��
	LV_COLUMN	lvCol;	//�ṹ
	lvCol.mask = LVCF_FMT |LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	CRect clientRect;
	m_ctlList.GetClientRect(&clientRect);

	//��һ��
	nCol = 0;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "Word";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 70;
	m_ctlList.InsertColumn(nCol, &lvCol);

	//�ڶ���
	nCol = 1;
	lvCol.iSubItem = nCol;
	lvCol.pszText = "��ѯ����";
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = (float)clientRect.Width()/100 * 30;
	m_ctlList.InsertColumn(nCol, &lvCol);
}

/*
 * �����б���ͼ�ؼ�
 */
void CUnknWordDlg::UpdateLstCtrl( void )
{
	m_ctlList.DeleteAllItems();//ɾ��������
	POSITION pos = NULL;
	int nIndex = 0;
	//ȡ�����С���-ֵ����
	for (pos = m_WordMap.GetStartPosition(); pos != NULL;)
	{
		CString strKey;//��
		int		nValue;//ֵ
		CString strValue;
		m_WordMap.GetNextAssoc(pos, strKey, nValue);

		strValue.Format("%d", nValue);
		m_ctlList.InsertItem(nIndex, strKey);		//���뵥��
		m_ctlList.SetItemText(nIndex, 1, strValue);	//�����ѯ����
	}		
}


/*
 * ����δ֪����
 */
void CUnknWordDlg::AddWord( CString strWord )
{
	int value = -1;
	//��ӳ���в��Ҹõ���
	BOOL bFinder = m_WordMap.Lookup((LPCSTR)strWord, value);
	if (bFinder)//�ҵ��õ��ʣ�������1
	{		
		m_WordMap.SetAt((LPCSTR)strWord , value + 1);
	}else		//δ�ҵ��õ��ʣ�����Ϊ1
	{
		m_WordMap.SetAt((LPCSTR)strWord , 1);
	}
}


/*
 * ��ʾ�����ش�����Ϣ��Ӧ����
 */
void CUnknWordDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (bShow == TRUE)		//��ʾ����
	{
		UpdateLstCtrl();	//��������
	}
	CDialog::OnShowWindow(bShow, nStatus);	
}
/*
 *�رմ���
 */
void CUnknWordDlg::OnClose() 
{
	m_pServView->ShowUnKnWordDlg();
}
