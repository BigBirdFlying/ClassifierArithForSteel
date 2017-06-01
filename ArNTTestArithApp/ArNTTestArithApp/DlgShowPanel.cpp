// DlgShowPanel.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "DlgShowPanel.h"


// CDlgShowPanel 对话框

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


// CDlgShowPanel 消息处理程序
