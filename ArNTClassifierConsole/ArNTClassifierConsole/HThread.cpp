#include "StdAfx.h"
#include "HThread.h"

HThread::HThread(void)
{
	pThread=NULL;
	m_ThreadParam.pExitEvent = new CEvent(FALSE, TRUE);
}

HThread::~HThread(void)
{
	if (pThread != NULL)
	{
		m_ThreadParam.pExitEvent->SetEvent();
		::WaitForSingleObject(pThread->m_hThread, INFINITE);
		delete pThread;
		pThread = NULL;
	}
	if (m_ThreadParam.pExitEvent != NULL)
	{
		delete m_ThreadParam.pExitEvent;
		m_ThreadParam.pExitEvent = NULL;
	}
}

int HThread::BeginThread(UINT(*ThreadProc)(LPVOID))
{	
	pThread = AfxBeginThread(ThreadProc, &m_ThreadParam, THREAD_PRIORITY_ABOVE_NORMAL, 0, CREATE_SUSPENDED);
	if(pThread==NULL)
	{
		return 0;
	}
	else
	{
		pThread->m_bAutoDelete = FALSE;
		return 1;
	}		
}

int HThread::SuspendThread()
{	
	if (pThread != NULL)
	{
		//挂起线程
		pThread->SuspendThread();
		return 1;
	}
	return 0;
}

int HThread::ResumeThread() 
{
	if (pThread != NULL)
	{
		//恢复线程
		pThread->ResumeThread();
		return 1;
	}
	return 0;
}

int HThread::EndThread() 
{
	if (pThread != NULL)
	{
		m_ThreadParam.bExit = TRUE;
		//等待线程结束
		::WaitForSingleObject(pThread->m_hThread, INFINITE);

		delete pThread;
		pThread = NULL;
		return 1;
	}
	return 0;
}