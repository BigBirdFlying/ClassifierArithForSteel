#pragma once
#include "DlgShowPanel.h"

// CDlgShowDlg �Ի���

class CDlgShowDlg : public CDialog
{
	DECLARE_DYNAMIC(CDlgShowDlg)

public:
	CDlgShowDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgShowDlg();

// �Ի�������
	enum { IDD = IDD_SHOWDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:

	CDlgShowPanel		m_DlgShowPanel;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
