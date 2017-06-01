// DlgShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "DlgShowDlg.h"


// CDlgShowDlg �Ի���

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


// CDlgShowDlg ��Ϣ�������

BOOL CDlgShowDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_DlgShowPanel.Create(IDD_SHOWPANEL, this);
	m_DlgShowPanel.ShowWindow(SW_SHOW);

	if(m_DlgShowPanel)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_DlgShowPanel.MoveWindow(&rcClient);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDlgShowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if(m_DlgShowPanel)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_DlgShowPanel.MoveWindow(&rcClient);
	}
}
