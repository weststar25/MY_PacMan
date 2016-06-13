// MyDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "Speech.h"
//#include "MyPackMan.h"
#include "MyDlg.h"
#include <sphelper.h>
//#include "afxdialogex.h"
#include "resource.h"
#include "MainFrm.h"
#include "MyPackManView.h"
#include "MyPackMan.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// CMyDlg 대화 상자입니다.

//IMPLEMENT_DYNAMIC(CMyDlg, CDialog)

CMyDlg::CMyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyDlg::IDD, pParent)
	, m_Edit(_T(""))
{
	//m_edit = _T("");
	m_pVoice = NULL;
}

CMyDlg::~CMyDlg()
{
}

void CMyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STR, m_Edit);
}


BEGIN_MESSAGE_MAP(CMyDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CMyDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL, &CMyDlg::OnBnClickedCancel)
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECOEVENT, OnRecoEvent)
END_MESSAGE_MAP()


// CMyDlg 메시지 처리기입니다.
BOOL CMyDlg::OnInitSpeech()
{
	HRESULT hr = S_OK;

	hr = cpRecoEngine.CoCreateInstance(CLSID_SpInprocRecognizer);

	if (SUCCEEDED(hr))
	{
		hr = cpRecoEngine->CreateRecoContext(&m_cpRecoCtxt);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_cpRecoCtxt->SetNotifyWindowMessage(m_hWnd, WM_RECOEVENT, 0, 0);
	}

	if (SUCCEEDED(hr))
	{
		const ULONGLONG ullInterest = SPFEI(SPEI_RECOGNITION);
		hr = m_cpRecoCtxt->SetInterest(ullInterest, ullInterest);
	}

	CComPtr<ISpAudio> cpAudio;
	hr = SpCreateDefaultObjectFromCategoryId(SPCAT_AUDIOIN, &cpAudio);

	hr = cpRecoEngine->SetInput(cpAudio, TRUE);
	hr = cpRecoEngine->SetRecoState(SPRST_ACTIVE);

	if (SUCCEEDED(hr))
		hr = m_cpRecoCtxt->CreateGrammar(GID_DICTATION, &m_cpDictationGrammar);

	if (SUCCEEDED(hr))
		hr = m_cpDictationGrammar->LoadDictation(NULL, SPLO_STATIC);

	if (SUCCEEDED(hr))
		hr = m_cpDictationGrammar->SetDictationState(SPRS_ACTIVE);

	if (FAILED(hr))
		m_cpDictationGrammar.Release();

	return (hr == S_OK);
}

LRESULT CMyDlg::OnRecoEvent(WPARAM /*wparam*/, LPARAM /*lparam*/)
{
	//MessageBox("a");
	OnRecoEvent();
	return 0;
}


void CMyDlg::OnRecoEvent()
{
	//CMyPackManApp *pApp = (CMyPackManApp *)AfxGetApp();   //View -> App
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();  //View -> MainFrm
	CMyPackManView *pView = (CMyPackManView *)pMain->GetActiveView();

	USES_CONVERSION;
	CSpEvent event;
	//MessageBox(L"A");
	while (event.GetFrom(m_cpRecoCtxt) == S_OK)
	{

		switch (event.eEventId)
		{
		case SPEI_RECOGNITION:
		{
			m_bReco = TRUE;
			static const WCHAR wszUnrecognized[] = L"fail";

			CSpDynamicString dstrText;
			if (FAILED(event.RecoResult()->GetText(SP_GETWHOLEPHRASE, SP_GETWHOLEPHRASE, TRUE,
				&dstrText, NULL)))
			{
				dstrText = wszUnrecognized;
			}

			dstrText.Append(L" ");

			m_Edit = dstrText;

			//if (m_Edit == "up " || m_Edit == "Up " || m_Edit == "Down " || m_Edit == "down " || m_Edit == "left " || m_Edit == "Left " || m_Edit == "Right " || m_Edit == "right "
			//	|| m_Edit == "let " || m_Edit == "light " || m_Edit == "night ")
			//	pView->m_EditV = m_Edit;
			////::SendDlgItemMessage(m_hWnd, IDC_EDIT_DICT, EM_REPLACESEL, TRUE, (LPARAM)W2T(dstrText));
			//UpdateData(TRUE);
			//m_pView->Invalidate();

			if (m_Edit == "down " || m_Edit == "Down ")
				m_pView->mKey = DOWN;
			else if (m_Edit == "up " || m_Edit == "Up ")
				m_pView->mKey = UP;
			else if (m_Edit == "one " || m_Edit == "One ")
				m_pView->mKey = LEFT;
			else if (m_Edit == "two " || m_Edit == "Two ")
				m_pView->mKey = RITE;

			UpdateData(TRUE);
			//UpdateData(FALSE);

		}
		break;
		}
	}
}

void CMyDlg::OnBnClickedButton1()
{
	//UpdateData(TRUE);
	//m_pView->m_Edit = m_Edit;
	//m_pView->Invalidate();
	OnInitSpeech();
}


void CMyDlg::OnBnClickedCancel()
{
	DestroyWindow();
	CDialog::OnCancel();
}


void CMyDlg::PostNcDestroy()
{
	m_pView->m_pDlg = NULL;
	delete this;
	CDialog::PostNcDestroy();
}
