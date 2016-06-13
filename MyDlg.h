#if !defined(AFX_SPEECHDLG_H__F20BA307_BA2F_4A58_AC6A_74FADA90E924__INCLUDED_)
#define AFX_SPEECHDLG_H__F20BA307_BA2F_4A58_AC6A_74FADA90E924__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sphelper.h>

#define MAX_EDIT_TEXT   1000
#define GID_DICTATION   0           // Dictation grammar has grammar ID 0

#define WM_RECOEVENT    WM_USER+5      // Window message used for recognition events
// CSpeechDlg 대화 상자입니다.

// CMyDlg 대화 상자입니다.
class CMyPackManView;

class CMyDlg : public CDialog
{
	//DECLARE_DYNAMIC(CMyDlg)

public:
	BOOL OnInitSpeech();

	//Identifies a locale for national language support. 
	//Locale information is used for international string 
	//comparisons and localized member names. 
	LCID m_lcid;

	//Boolean variables
	BOOL m_bReco;
	BOOL m_bSound;

	//Initialize the Recognizer
	CComPtr<ISpRecognizer> cpRecoEngine;

	//The ISpRecoContext interface enables applications to create 
	//different functional views or contexts of the SR engine
	CComPtr<ISpRecoContext> m_cpRecoCtxt;

	//The ISpRecoGrammar interface enables applications to manage 
	//the words and phrases that the SR engine will recognize.
	CComPtr<ISpRecoGrammar>	m_cpDictationGrammar;

	ISpVoice* m_pVoice;

	CMyDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMyDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG1 };
;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	HICON m_hIcon;

	afx_msg void OnRecoEvent();
	afx_msg LRESULT OnRecoEvent(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()
public:
	CMyPackManView* m_pView;
	CString m_Edit;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedCancel();
	virtual void PostNcDestroy();
};
#endif // !defined(AFX_SPEECHDLG_H__F20BA307_BA2F_4A58_AC6A_74FADA90E924__INCLUDED_)
