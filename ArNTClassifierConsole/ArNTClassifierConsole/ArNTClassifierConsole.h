// ArNTClassifierConsole.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������

// CArNTClassifierConsoleApp:
// �йش����ʵ�֣������ ArNTClassifierConsole.cpp
//

class CArNTClassifierConsoleApp : public CWinApp
{
public:
	CArNTClassifierConsoleApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��


	DECLARE_MESSAGE_MAP()
};

extern CArNTClassifierConsoleApp theApp;