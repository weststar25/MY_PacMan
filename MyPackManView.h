
// MyPackManView.h : CMyPackManView 클래스의 인터페이스
//

#pragma once
#include "CamRemote.h"
#include "MyPackManDoc.h"
class CMyDlg;

/* enum 상수들 */
enum gamePage{ INTRO, SELECTCONTROL, KEYBOARD, MOTION, SPEECH, ENDING };


class CMyPackManView : public CView
{
protected: // serialization에서만 만들어집니다.
	CMyPackManView();
	DECLARE_DYNCREATE(CMyPackManView)

// 특성입니다.
public:
	CMyPackManDoc* GetDocument() const;
	CMyDlg * m_pDlg;
	CString m_EditV;

	/* 이미지 출력용 그래픽 객체 */
	Bitmap *mAllBmp;
	Graphics *mAllGrfx;
	Graphics *mGrfx;

	/* 게임 화면 전환용 변수 */
	int mGamePage;
	
	/* 윈도우 사이즈 및 버튼 전환용 Rect 객체 */
	CRect mWindowRect;
	CRect mStartRect;
	CRect mKeyboardRect;
	CRect mMotionRect;
	CRect mSpeechRect;

	/* Thread 제어 객체 */
	CRITICAL_SECTION mPackmanEvent;
	CRITICAL_SECTION mBlinkyEvent;
	CRITICAL_SECTION mInkyEvent;
	CRITICAL_SECTION mPinkyEvent;
	CRITICAL_SECTION mClydeEvent;


	/* Cam 제어 객체 */
	CamRemote mCam;

	/* keyboard값 저장 변수 */
	int mKey; // 현재 키 값
	int mNextKey; // 다음 키 값 

	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void initRects();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CMyPackManView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
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

#ifndef _DEBUG  // MyPackManView.cpp의 디버그 버전
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
