#pragma once


// DlgClassifierCNN �Ի���

class DlgClassifierCNN : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierCNN)

public:
	DlgClassifierCNN(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~DlgClassifierCNN();

// �Ի�������
	enum { IDD = IDD_DIALOG_CLASSIFIER_CNN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
