
// Fried.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CFriedApp:
// �� Ŭ������ ������ ���ؼ��� Fried.MFC.cpp�� �����Ͻʽÿ�.
//

class CFriedApp : public CWinApp
{
public:
	CFriedApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CFriedApp theApp;