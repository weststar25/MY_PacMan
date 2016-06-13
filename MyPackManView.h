
// MyPackManView.h : CMyPackManView Ŭ������ �������̽�
//

#pragma once
#include "CamRemote.h"
#include "MyPackManDoc.h"
class CMyDlg;

/* enum ����� */
enum gamePage{ INTRO, SELECTCONTROL, KEYBOARD, MOTION, SPEECH, ENDING };


class CMyPackManView : public CView
{
protected: // serialization������ ��������ϴ�.
	CMyPackManView();
	DECLARE_DYNCREATE(CMyPackManView)

// Ư���Դϴ�.
public:
	CMyPackManDoc* GetDocument() const;
	CMyDlg * m_pDlg;
	CString m_EditV;

	/* �̹��� ��¿� �׷��� ��ü */
	Bitmap *mAllBmp;
	Graphics *mAllGrfx;
	Graphics *mGrfx;

	/* ���� ȭ�� ��ȯ�� ���� */
	int mGamePage;
	
	/* ������ ������ �� ��ư ��ȯ�� Rect ��ü */
	CRect mWindowRect;
	CRect mStartRect;
	CRect mKeyboardRect;
	CRect mMotionRect;
	CRect mSpeechRect;

	/* Thread ���� ��ü */
	CRITICAL_SECTION mPackmanEvent;
	CRITICAL_SECTION mBlinkyEvent;
	CRITICAL_SECTION mInkyEvent;
	CRITICAL_SECTION mPinkyEvent;
	CRITICAL_SECTION mClydeEvent;


	/* Cam ���� ��ü */
	CamRemote mCam;

	/* keyboard�� ���� ���� */
	int mKey; // ���� Ű ��
	int mNextKey; // ���� Ű �� 

	virtual void OnDraw(CDC* pDC);  // �� �並 �׸��� ���� �����ǵǾ����ϴ�.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void initRects();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// �����Դϴ�.
public:
	virtual ~CMyPackManView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // MyPackManView.cpp�� ����� ����
inline CMyPackManDoc* CMyPackManView::GetDocument() const
   { return reinterpret_cast<CMyPackManDoc*>(m_pDocument); }
#endif

UINT __cdecl introProc(void *p);
UINT __cdecl selectControlProc(void *p);
UINT __cdecl packmanProc(void *p);
UINT __cdecl cameraProc(void *p);
UINT __cdecl BlinkyProc(void *p);
UINT __cdecl InkyProc(void *p);
UINT __cdecl PinkyProc(void *p);
UINT __cdecl clydeProc(void *p);
UINT __cdecl drawProc(void *p);
UINT __cdecl endingProc(void *p);
UINT __cdecl speechProc(void *p);

void CALLBACK changeGhost(HWND, UINT, UINT_PTR, DWORD);
void CALLBACK dropItem(HWND, UINT, UINT_PTR, DWORD);
