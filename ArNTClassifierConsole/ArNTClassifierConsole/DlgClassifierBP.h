#pragma once


// DlgClassifierBP �Ի���

class DlgClassifierBP : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierBP)

public:
	DlgClassifierBP(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgClassifierBP();

// �Ի�������
	enum { IDD = IDD_DIALOG_CLASSIFIER_BP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
