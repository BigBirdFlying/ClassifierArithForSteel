#pragma once


// DlgHelp 对话框

class DlgHelp : public CDialog
{
	DECLARE_DYNAMIC(DlgHelp)

public:
	DlgHelp(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgHelp();

public:
	int m_nTextX;   // 水平滚动文本的起始点的x坐标   
	int m_nTextY;   // 垂直滚动文本的起始点的y坐标   
	CFont m_newFont;   // 新字体   
	CFont *m_pOldFont; // 选择新字体之前的字体  

// 对话框数据
	enum { IDD = IDD_DIALOG_HELP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
