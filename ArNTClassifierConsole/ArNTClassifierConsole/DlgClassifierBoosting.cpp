// DlgClassifierBoosting.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgClassifierBoosting.h"


// DlgClassifierBoosting �Ի���

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


// DlgClassifierBoosting ��Ϣ�������

BOOL DlgClassifierBoosting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString strIn;
	strIn.Format(L"%d",10);
	SetDlgItemText(IDC_EDIT_WEAK_CLASSIFIER_NUM,strIn); 
	strIn.Format(L"%f",0.001);
	SetDlgItemText(IDC_EDIT_EPSILON,strIn); 
	strIn.Format(L"Boosting_Classifier.xml");
	SetDlgItemText(IDC_EDIT_BOOSTING_CLASSIFIER_FILENAME,strIn); 

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
