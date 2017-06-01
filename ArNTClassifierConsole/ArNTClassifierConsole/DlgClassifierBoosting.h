#pragma once


// DlgClassifierBoosting 对话框

class DlgClassifierBoosting : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierBoosting)

public:
	DlgClassifierBoosting(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgClassifierBoosting();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLASSIFIER_BOOSTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
