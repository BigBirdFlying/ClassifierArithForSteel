// DlgClassifierSVM.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTClassifierConsole.h"
#include "DlgClassifierSVM.h"


// DlgClassifierSVM �Ի���

IMPLEMENT_DYNAMIC(DlgClassifierSVM, CDialog)

DlgClassifierSVM::DlgClassifierSVM(CWnd* pParent /*=NULL*/)
	: CDialog(DlgClassifierSVM::IDD, pParent)
{
	
}

DlgClassifierSVM::~DlgClassifierSVM()
{
}

void DlgClassifierSVM::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgClassifierSVM, CDialog)
END_MESSAGE_MAP()


// DlgClassifierSVM ��Ϣ�������

BOOL DlgClassifierSVM::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString strIn;
	strIn.Format(L"%d",10000);
	SetDlgItemText(IDC_EDIT_ITER_NUM,strIn); 
	strIn.Format(L"%f",0.00001);
	SetDlgItemText(IDC_EDIT_EPSILON,strIn); 
	strIn.Format(L"%f",1.0);
	SetDlgItemText(IDC_EDIT_SVM_CLASSIFIER_C,strIn); 
	strIn.Format(L"%f",0.01);
	SetDlgItemText(IDC_EDIT_SVM_CLASSIFIER_GAMMA,strIn); 
	strIn.Format(L"SVM_Classifier.xml");
	SetDlgItemText(IDC_EDIT_SVM_CLASSIFIER_FILENAME,strIn);

	((CComboBox*)GetDlgItem(IDC_COMBO_SVM_CLASSIFIER_KEN))->InsertString(0,L"SVM::RBF");
	((CComboBox*)GetDlgItem(IDC_COMBO_SVM_CLASSIFIER_KEN))->InsertString(1,L"SVM::LINEAR");
	((CComboBox*)GetDlgItem(IDC_COMBO_SVM_CLASSIFIER_KEN))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
