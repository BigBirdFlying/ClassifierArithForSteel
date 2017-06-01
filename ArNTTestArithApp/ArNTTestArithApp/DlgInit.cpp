// DlgInit.cpp : 实现文件
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "DlgInit.h"
#include "ArNTTestArithState.h"

// CDlgInit 对话框

IMPLEMENT_DYNAMIC(CDlgInit, CDialog)

CDlgInit::CDlgInit(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInit::IDD, pParent)
{

}

CDlgInit::~CDlgInit()
{
}

void CDlgInit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_lstInit);
}


BEGIN_MESSAGE_MAP(CDlgInit, CDialog)
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(UM_CLOSE_DLG, OnUMCloseDlg)
END_MESSAGE_MAP()


// CDlgInit 消息处理程序

void CDlgInit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: 在此处添加消息处理程序代码
	if(TRUE ==  bShow)
	{
		m_lstInit.AddString(L"程序初始化，请稍后....");
		ONLY_THREAD_INIT_RUN(NULL);
	}
}

LRESULT CDlgInit::OnUMCloseDlg(WPARAM wParam, LPARAM lParam)
{
	
	if(wParam == IDOK)
	{
		Sleep(2000);
		OnOK();
	}else
	{
		Sleep(5000);
		this->OnCancel();
	}
	return 0;
}
//////////////////////////////////////////////////////
//线程函数
IMPL_ONLY_THREAD(CDlgInit)

ArNT_BOOL  CDlgInit::OnThread(LongInt iThreadID, PObjPointer pContext)
{
	ArNTTestArithState* pState = ArNTTestArithState::GetState();
	LONGSTR strError = {0};
	if(ArNTTestArithState::GetState()->Init(strError) == ArNT_FALSE)
	{
		m_lstInit.AddString(strError);
		PostMessage(UM_CLOSE_DLG, (WPARAM)IDCANCEL);
	}
	if(pState->m_bLocalMode == ArNT_FALSE)
	{
		if(pState->WaitInit(strError) == ArNT_FALSE)
		{
			m_lstInit.AddString(strError);	
			PostMessage(UM_CLOSE_DLG, (WPARAM)IDCANCEL);
		}
	}
	m_lstInit.AddString(L"成功完成初始化!");	
	PostMessage(UM_CLOSE_DLG, (WPARAM)IDOK);
	return ArNT_TRUE;
}


