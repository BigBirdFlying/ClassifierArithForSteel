#pragma once
#include "DlgShowPanel.h"

// CDlgShowDlg 对话框

class CDlgShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CDlgShowDlg)

public:
	CDlgShowDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgShowDlg();

// 对话框数据
	enum { IDD = IDD_SHOWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	CDlgShowPanel		m_DlgShowPanel;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
