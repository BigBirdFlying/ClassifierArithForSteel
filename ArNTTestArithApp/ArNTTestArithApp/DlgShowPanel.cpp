// DlgShowPanel.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "DlgShowPanel.h"


// CDlgShowPanel �Ի���

IMPLEMENT_DYNAMIC(CDlgShowPanel, CImgPanel)

CDlgShowPanel::CDlgShowPanel(CWnd* pParent /*=NULL*/)
	: CImgPanel(CDlgShowPanel::IDD, pParent)
{

}

CDlgShowPanel::~CDlgShowPanel()
{
}

void CDlgShowPanel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgShowPanel, CImgPanel)
END_MESSAGE_MAP()


// CDlgShowPanel ��Ϣ�������
