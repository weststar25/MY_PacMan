// SpeechApp.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyPackMan.h"
#include "Speech.h"
#include "MyDlg.h"
#include "sapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CSpeech, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

IMPLEMENT_DYNCREATE(CSpeech, CWinApp)

CSpeech::CSpeech()
{
}

BOOL CSpeech::InitInstance()
{
	AfxOleInit();

	AfxEnableControlContainer();
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
	return FALSE;
}

int CSpeech::ExitInstance()
{
	return CWinApp::ExitInstance();
}

