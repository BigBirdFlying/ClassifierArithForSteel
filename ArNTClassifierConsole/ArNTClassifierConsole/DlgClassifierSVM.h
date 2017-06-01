#pragma once


// DlgClassifierSVM 对话框

class DlgClassifierSVM : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierSVM)

public:
	DlgClassifierSVM(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgClassifierSVM();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLASSIFIER_SVM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
