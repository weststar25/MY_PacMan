#if !defined(AFX_SPEECH_H__57BCFC3A_0DD5_48A8_BD3F_BBF8A41B3402__INCLUDED_)
#define AFX_SPEECH_H__57BCFC3A_0DD5_48A8_BD3F_BBF8A41B3402__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

class CSpeech : public CWinApp
{
	DECLARE_DYNCREATE(CSpeech)

protected:
	CSpeech();           // 동적 만들기에 사용되는 protected 생성자입니다.

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
};
#endif