#pragma once


// DlgClassifierCNN 对话框

class DlgClassifierCNN : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierCNN)

public:
	DlgClassifierCNN(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgClassifierCNN();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLASSIFIER_CNN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
