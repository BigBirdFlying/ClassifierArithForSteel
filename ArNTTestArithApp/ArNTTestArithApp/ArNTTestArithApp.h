// ArNTTestArithApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CArNTTestArithAppApp:
// �йش����ʵ�֣������ ArNTTestArithApp.cpp
//

class CArNTTestArithAppApp : public CWinApp
{
public:
	CArNTTestArithAppApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CArNTTestArithAppApp theApp;