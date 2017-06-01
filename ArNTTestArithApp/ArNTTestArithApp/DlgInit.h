#pragma once

#include "CommonFunc.h"
#include "ArNTCommonClass.h"
#include "afxwin.h"
// CDlgInit �Ի���


#define  UM_CLOSE_DLG     WM_APP + 25

class CDlgInit : public CDialog
{
	DECLARE_DYNAMIC(CDlgInit)

public:
	CDlgInit(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgInit();

// �Ի�������
	enum { IDD = IDD_INIT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg  LRESULT  OnUMCloseDlg(WPARAM wParam, LPARAM lParam);
	CListBox m_lstInit;

private:
	DECL_ONLY_THREAD()
	ArNT_BOOL  OnThread(LongInt iThreadID, PObjPointer pContext);
};
