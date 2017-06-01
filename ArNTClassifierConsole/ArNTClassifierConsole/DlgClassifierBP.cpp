// DlgClassifierBP.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgClassifierBP.h"


// DlgClassifierBP 对话框

IMPLEMENT_DYNAMIC(DlgClassifierBP, CDialog)

DlgClassifierBP::DlgClassifierBP(CWnd* pParent /*=NULL*/)
	: CDialog(DlgClassifierBP::IDD, pParent)
{

}

DlgClassifierBP::~DlgClassifierBP()
{
}

void DlgClassifierBP::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgClassifierBP, CDialog)
END_MESSAGE_MAP()


// DlgClassifierBP 消息处理程序

BOOL DlgClassifierBP::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString strIn;
	strIn.Format(L"%d",30);
	SetDlgItemText(IDC_EDIT_HIDE_BODE_NUM,strIn); 
	strIn.Format(L"%d",5000);
	SetDlgItemText(IDC_EDIT_ITER_NUM,strIn); 
	strIn.Format(L"%f",0.001);
	SetDlgItemText(IDC_EDIT_EPSILON,strIn); 
	strIn.Format(L"BP_Classifier.xml");
	SetDlgItemText(IDC_EDIT_BP_CLASSIFIER_FILENAME,strIn); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
