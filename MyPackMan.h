
// MyPackMan.h : MyPackMan ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CMyPackManApp:
// �� Ŭ������ ������ ���ؼ��� MyPackMan.cpp�� �����Ͻʽÿ�.
//

class CMyPackManApp : public CWinAppEx
{
public:
	CMyPackManApp();

	ULONG_PTR mToken;

	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMyPackManApp theApp;
