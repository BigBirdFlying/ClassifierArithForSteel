#pragma once


// DlgClassifierBoosting �Ի���

class DlgClassifierBoosting : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierBoosting)

public:
	DlgClassifierBoosting(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgClassifierBoosting();

// �Ի�������
	enum { IDD = IDD_DIALOG_CLASSIFIER_BOOSTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
