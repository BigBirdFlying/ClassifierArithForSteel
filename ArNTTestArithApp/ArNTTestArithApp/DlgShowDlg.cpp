// DlgShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "DlgShowDlg.h"


// CDlgShowDlg 对话框

IMPLEMENT_DYNAMIC(CDlgShowDlg, CDialog)

CDlgShowDlg::CDlgShowDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgShowDlg::IDD, pParent)
{

}

CDlgShowDlg::~CDlgShowDlg()
{
}

void CDlgShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShowDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgShowDlg 消息处理程序

BOOL CDlgShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_DlgShowPanel.Create(IDD_SHOWPANEL, this);
	m_DlgShowPanel.ShowWindow(SW_SHOW);

	if(m_DlgShowPanel)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_DlgShowPanel.MoveWindow(&rcClient);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgShowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(m_DlgShowPanel)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_DlgShowPanel.MoveWindow(&rcClient);
	}
}
