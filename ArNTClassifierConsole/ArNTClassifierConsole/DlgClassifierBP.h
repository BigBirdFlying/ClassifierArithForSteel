#pragma once


// DlgClassifierBP 对话框

class DlgClassifierBP : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierBP)

public:
	DlgClassifierBP(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgClassifierBP();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLASSIFIER_BP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
