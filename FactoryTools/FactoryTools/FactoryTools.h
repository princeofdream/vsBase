
// FactoryTools.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFactoryToolsApp: 
// �йش����ʵ�֣������ FactoryTools.cpp
//

class CFactoryToolsApp : public CWinApp
{
public:
	CFactoryToolsApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFactoryToolsApp theApp;