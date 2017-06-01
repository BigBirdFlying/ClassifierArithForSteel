// DlgInit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ArNTTestArithApp.h"
#include "DlgInit.h"
#include "ArNTTestArithState.h"

// CDlgInit �Ի���

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


// CDlgInit ��Ϣ�������

void CDlgInit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);
	// TODO: �ڴ˴������Ϣ����������
	if(TRUE ==  bShow)
	{
		m_lstInit.AddString(L"�����ʼ�������Ժ�....");
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
//�̺߳���
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
	m_lstInit.AddString(L"�ɹ���ɳ�ʼ��!");	
	PostMessage(UM_CLOSE_DLG, (WPARAM)IDOK);
	return ArNT_TRUE;
}


