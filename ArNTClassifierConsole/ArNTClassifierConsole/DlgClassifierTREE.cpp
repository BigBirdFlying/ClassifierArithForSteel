// DlgClassifierTREE.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgClassifierTREE.h"


// DlgClassifierTREE �Ի���

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


// DlgClassifierTREE ��Ϣ�������

BOOL DlgClassifierTREE::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString strIn;
	strIn.Format(L"%d",1000);
	SetDlgItemText(IDC_EDIT_TREE_NUM,strIn); 
	strIn.Format(L"%f",0.0001);
	SetDlgItemText(IDC_EDIT_EPSILON,strIn); 
	strIn.Format(L"RTree_Classifier.xml");
	SetDlgItemText(IDC_EDIT_RTREE_CLASSIFIER_FILENAME,strIn); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
