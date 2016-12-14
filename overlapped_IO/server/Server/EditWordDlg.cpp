// EditWordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Server.h"
#include "EditWordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditWordDlg dialog


CEditWordDlg::CEditWordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEditWordDlg::IDD, pParent)
	,m_bWordEdit(TRUE)
{
	//{{AFX_DATA_INIT(CEditWordDlg)
	m_strWord = _T("");
	m_strMeaning1 = _T("");
	m_strMeaning2 = _T("");
	//}}AFX_DATA_INIT
}


void CEditWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditWordDlg)
	DDX_Text(pDX, IDC_EDIT_WORD, m_strWord);
	DDV_MaxChars(pDX, m_strWord, 20);
	DDX_Text(pDX, IDC_EDIT_MEANING1, m_strMeaning1);
	DDV_MaxChars(pDX, m_strMeaning1, 64);
	DDX_Text(pDX, IDC_EDIT_MEANING2, m_strMeaning2);
	DDV_MaxChars(pDX, m_strMeaning2, 32);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditWordDlg, CDialog)
	//{{AFX_MSG_MAP(CEditWordDlg)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditWordDlg message handlers

BOOL CEditWordDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	(CEdit*)GetDlgItem(IDC_EDIT_WORD)->EnableWindow(m_bWordEdit);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


