#pragma once
#include "ArNTCommonMFCUI.h"


// CDlgShowPanel �Ի���

class CDlgShowPanel : public CImgPanel
{
	DECLARE_DYNAMIC(CDlgShowPanel)

public:
	CDlgShowPanel(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgShowPanel();

// �Ի�������
	enum { IDD = IDD_SHOWPANEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
