// DlgClassifierTREE.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgClassifierTREE.h"


// DlgClassifierTREE 对话框

IMPLEMENT_DYNAMIC(DlgClassifierTREE, CDialog)

DlgClassifierTREE::DlgClassifierTREE(CWnd* pParent /*=NULL*/)
	: CDialog(DlgClassifierTREE::IDD, pParent)
{

}

DlgClassifierTREE::~DlgClassifierTREE()
{
}

void DlgClassifierTREE::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgClassifierTREE, CDialog)
END_MESSAGE_MAP()


// DlgClassifierTREE 消息处理程序

BOOL DlgClassifierTREE::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CString strIn;
	strIn.Format(L"%d",1000);
	SetDlgItemText(IDC_EDIT_TREE_NUM,strIn); 
	strIn.Format(L"%f",0.0001);
	SetDlgItemText(IDC_EDIT_EPSILON,strIn); 
	strIn.Format(L"RTree_Classifier.xml");
	SetDlgItemText(IDC_EDIT_RTREE_CLASSIFIER_FILENAME,strIn); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
