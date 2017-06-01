#pragma once


// DlgClassifierTREE 对话框

class DlgClassifierTREE : public CDialog
{
	DECLARE_DYNAMIC(DlgClassifierTREE)

public:
	DlgClassifierTREE(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgClassifierTREE();

// 对话框数据
	enum { IDD = IDD_DIALOG_CLASSIFIER_TREE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
