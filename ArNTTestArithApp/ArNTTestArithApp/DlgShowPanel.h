#pragma once
#include "ArNTCommonMFCUI.h"


// CDlgShowPanel 对话框

class CDlgShowPanel : public CImgPanel
{
	DECLARE_DYNAMIC(CDlgShowPanel)

public:
	CDlgShowPanel(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgShowPanel();

// 对话框数据
	enum { IDD = IDD_SHOWPANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
