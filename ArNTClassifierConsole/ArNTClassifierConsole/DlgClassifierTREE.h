#pragma once


// DlgClassifierTREE �Ի���

class DlgClassifierTREE : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierTREE)

public:
	DlgClassifierTREE(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgClassifierTREE();

// �Ի�������
	enum { IDD = IDD_DIALOG_CLASSIFIER_TREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
