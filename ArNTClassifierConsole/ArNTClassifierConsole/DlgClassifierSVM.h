#pragma once


// DlgClassifierSVM �Ի���

class DlgClassifierSVM : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierSVM)

public:
	DlgClassifierSVM(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgClassifierSVM();

// �Ի�������
	enum { IDD = IDD_DIALOG_CLASSIFIER_SVM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
