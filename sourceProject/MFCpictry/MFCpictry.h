
// MFCpictry.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCpictryApp:
// �йش����ʵ�֣������ MFCpictry.cpp
//

class CMFCpictryApp : public CWinApp
{
public:
	CMFCpictryApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCpictryApp theApp;