#pragma once


// DlgImageShow 对话框

class DlgImageShow : public CDialog
{
	DECLARE_DYNAMIC(DlgImageShow)

public:
	DlgImageShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgImageShow();

// 对话框数据
	enum { IDD = IDD_DIALOG_SHOW_ORGINIMG };

public:
	void ShowImgToMFCFormMat(int ID,cv::Mat src);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
