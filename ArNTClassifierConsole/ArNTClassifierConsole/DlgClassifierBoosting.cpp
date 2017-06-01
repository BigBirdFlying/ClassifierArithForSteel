// DlgClassifierBoosting.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgClassifierBoosting.h"


// DlgClassifierBoosting 对话框

IMPLEMENT_DYNAMIC(DlgClassifierBoosting, CDialog)

DlgClassifierBoosting::DlgClassifierBoosting(CWnd* pParent /*=NULL*/)
	: CDialog(DlgClassifierBoosting::IDD, pParent)
{

}

DlgClassifierBoosting::~DlgClassifierBoosting()
{
}

void DlgClassifierBoosting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgClassifierBoosting, CDialog)
END_MESSAGE_MAP()


// DlgClassifierBoosting 消息处理程序

BOOL DlgClassifierBoosting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString strIn;
	strIn.Format(L"%d",10);
	SetDlgItemText(IDC_EDIT_WEAK_CLASSIFIER_NUM,strIn); 
	strIn.Format(L"%f",0.001);
	SetDlgItemText(IDC_EDIT_EPSILON,strIn); 
	strIn.Format(L"Boosting_Classifier.xml");
	SetDlgItemText(IDC_EDIT_BOOSTING_CLASSIFIER_FILENAME,strIn); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
