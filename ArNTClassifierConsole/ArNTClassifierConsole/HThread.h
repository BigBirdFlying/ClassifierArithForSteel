#pragma once
#include <afxmt.h>

typedef struct tagThreadParam 
{
	HWND hWnd;
	int index;
	int nData;
	BOOL bExit;
	CMutex* pMutex;
	CEvent* pExitEvent;
}ThreadParam;

class HThread
{
public:
	HThread(void);
	~HThread(void);

	CWinThread* pThread;
	tagThreadParam m_ThreadParam;

	int BeginThread(UINT(*ThreadProc)(LPVOID));
	int SuspendThread();
	int ResumeThread();
	int EndThread();
};
