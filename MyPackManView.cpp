
// MyPackManView.cpp : CMyPackManView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MyPackMan.h"
#endif
#include "MainFrm.h"
#include "MyPackManDoc.h"
#include "MyPackManView.h"
#include "MyDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// CMyPackManView

IMPLEMENT_DYNCREATE(CMyPackManView, CView)

BEGIN_MESSAGE_MAP(CMyPackManView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CMyPackManView 생성/소멸

CMyPackManView::CMyPackManView()
	: mGamePage(INTRO)
	, mKey(STOP)
	
{
	::InitializeCriticalSection(&mPackmanEvent);
	::InitializeCriticalSection(&mBlinkyEvent);
	::InitializeCriticalSection(&mInkyEvent);
	::InitializeCriticalSection(&mPinkyEvent);
	::InitializeCriticalSection(&mClydeEvent);
	m_pDlg = NULL;
	m_EditV = _T("");
}

CMyPackManView::~CMyPackManView()
{
	delete mAllBmp;
	delete mAllGrfx;
	::DeleteCriticalSection(&mPackmanEvent);
	::DeleteCriticalSection(&mBlinkyEvent);
	::DeleteCriticalSection(&mInkyEvent);
	::DeleteCriticalSection(&mPinkyEvent);
	::DeleteCriticalSection(&mClydeEvent);
}

BOOL CMyPackManView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// CMyPackManView 그리기
void CMyPackManView::OnDraw(CDC* pDC)
{
	CMyPackManDoc* pDoc = GetDocument();

	pDC->TextOut(10, 300, m_EditV);
}

// CMyPackManView 인쇄
BOOL CMyPackManView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}
void CMyPackManView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}
void CMyPackManView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

// CMyPackManView 진단
#ifdef _DEBUG
void CMyPackManView::AssertValid() const
{
	CView::AssertValid();
}
void CMyPackManView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CMyPackManDoc* CMyPackManView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMyPackManDoc)));
	return (CMyPackManDoc*)m_pDocument;
}
#endif //_DEBUG


// CMyPackManView 메시지 처리기
void CMyPackManView::initRects()
{
	CMyPackManDoc *pDoc = GetDocument();

	mStartRect.SetRect(pDoc->mScene.mButtonLeft, 500, pDoc->mScene.mButtonLeft + pDoc->mScene.mSbuttonD.GetWidth(), 550 + pDoc->mScene.mSbuttonD.GetHeight());
	mKeyboardRect.SetRect(pDoc->mScene.mKeyLeft, pDoc->mScene.mSelectTop, pDoc->mScene.mKeyLeft + pDoc->mScene.mKeyboardD.GetWidth() - 50, pDoc->mScene.mSelectTop + pDoc->mScene.mKeyboardD.GetHeight() - 70);
	mMotionRect.SetRect(pDoc->mScene.mMotionLeft, pDoc->mScene.mSelectTop, pDoc->mScene.mMotionLeft + pDoc->mScene.mKeyboardD.GetWidth() - 50, pDoc->mScene.mSelectTop + pDoc->mScene.mMotionD.GetHeight() - 70);
	mSpeechRect.SetRect(pDoc->mScene.mSpeechLeft, pDoc->mScene.mSelectTop, pDoc->mScene.mSpeechLeft + pDoc->mScene.mKeyboardD.GetWidth() - 50, pDoc->mScene.mSelectTop + pDoc->mScene.mSpeechD.GetHeight() - 70);
}

BOOL CMyPackManView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}

int CMyPackManView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetTimer(1, 1000, nullptr); // 유령들 방향 바꾸기 위한 시간
	SetTimer(2, 500, changeGhost); // 바보 유령 제한 시간 
	SetTimer(3, 1000, dropItem);
	gIntroEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	gSelectEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	gGameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	gDrawEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	gEndingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	return 0;
}

void CMyPackManView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	CMyPackManDoc *pDoc = GetDocument();

	GetClientRect(mWindowRect);

	mAllBmp = new Bitmap(mWindowRect.right, mWindowRect.bottom);
	mAllGrfx = new Graphics(mAllBmp);

	pDoc->mScene.fillBckGround(mAllGrfx, mWindowRect);
	pDoc->mScene.initIntroScene(mWindowRect);

	initRects();

	pDoc->mGame.initGame(mWindowRect);
	SetEvent(gIntroEvent);
	AfxBeginThread(introProc, this);

}

void CMyPackManView::OnMouseMove(UINT nFlags, CPoint point)
{
	CMyPackManDoc *pDoc = GetDocument();

	if (mGamePage == INTRO) {
		
		if (PtInRect(mStartRect, point))
			pDoc->mScene.mButtonFlag = ON;
		else
			pDoc->mScene.mButtonFlag = OFF;
	}
	
	else if (mGamePage == SELECTCONTROL) {
		
		if (PtInRect(mKeyboardRect, point))
			pDoc->mScene.mKeyboardFlag = ON;
		else
			pDoc->mScene.mKeyboardFlag = OFF;

		if (PtInRect(mMotionRect, point))
			pDoc->mScene.mMotionFlag = ON;
		else
			pDoc->mScene.mMotionFlag = OFF;
		
		if (PtInRect(mSpeechRect, point)) 
			pDoc->mScene.mSpeechFlag = ON;
		else
			pDoc->mScene.mSpeechFlag = OFF;
	}

}

void CMyPackManView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMyPackManDoc *pDoc = GetDocument();

	if (mGamePage == INTRO) {
		
		if (mStartRect.PtInRect(point) == FALSE)
			return;
	
		mGamePage = SELECTCONTROL;
		ResetEvent(gIntroEvent);
		SetEvent(gSelectEvent);
		AfxBeginThread(selectControlProc, this); // 형 버전
	}

	else if (mGamePage == SELECTCONTROL) {

		if (PtInRect(mKeyboardRect, point)) {

			mGamePage = KEYBOARD;
			ResetEvent(gSelectEvent);
			SetEvent(gDrawEvent);
			AfxBeginThread(drawProc, this);
			PlaySound(L"res/pacman BGM/intro.wav", NULL, SND_ASYNC);
			Sleep(3000);
			bckSound.bckMusic();
			pDoc->mGame.mReadyFlag = false;
			SetEvent(gGameEvent);

			AfxBeginThread(BlinkyProc, this);
			AfxBeginThread(InkyProc, this);
			AfxBeginThread(PinkyProc, this);
			AfxBeginThread(clydeProc, this);
			AfxBeginThread(packmanProc, this);
		}
		
		if (PtInRect(mMotionRect, point)) {
			
			mGamePage = MOTION;
			ResetEvent(gSelectEvent);
			SetEvent(gDrawEvent);
			AfxBeginThread(drawProc, this);
			PlaySound(L"res/pacman BGM/intro.wav", NULL, SND_ASYNC);
			AfxBeginThread(cameraProc, this);
			Sleep(3000);
			bckSound.bckMusic();
			pDoc->mGame.mReadyFlag = false;
			SetEvent(gGameEvent);

			AfxBeginThread(BlinkyProc, this);
			AfxBeginThread(InkyProc, this);
			AfxBeginThread(PinkyProc, this);
			AfxBeginThread(clydeProc, this);
			AfxBeginThread(packmanProc, this);
		}
		
		if (PtInRect(mSpeechRect, point)) {
	
			mGamePage = SPEECH;
			BOOL ok = FALSE;
			if (m_pDlg != NULL)
				m_pDlg->SetFocus();
			else
			{
				m_pDlg = new CMyDlg();
				m_pDlg->m_pView = this;
				m_pDlg->m_Edit = m_EditV;
				m_pDlg->Create(IDD_DIALOG1);
				ok = m_pDlg->ShowWindow(SW_SHOW);
			}
			
			if (ok == FALSE)
			{
				ResetEvent(gSelectEvent);
				SetEvent(gDrawEvent);
				AfxBeginThread(drawProc, this);
				PlaySound(L"res/pacman BGM/intro.wav", NULL, SND_ASYNC);
				Sleep(3000);
				bckSound.bckMusic();
				pDoc->mGame.mReadyFlag = false;
				SetEvent(gGameEvent);

				AfxBeginThread(BlinkyProc, this);
				AfxBeginThread(InkyProc, this);
				AfxBeginThread(PinkyProc, this);
				AfxBeginThread(clydeProc, this);
				AfxBeginThread(packmanProc, this);
			}
		}
	}
	else if (mGamePage == ENDING)
	{
		ResetEvent(gEndingEvent);

		mGamePage = INTRO;

		pDoc->mGame.mLife = 3;
		pDoc->mGame.mScore = 0;
		pDoc->mGame.initMap();
		pDoc->mGame.mReadyFlag = true;

		SetEvent(gSelectEvent);
	}
	else if (mGamePage == KEYBOARD)
	{
		TRACE(L"%d.%d\n", point.x, point.y);
	}
}

void CMyPackManView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMyPackManDoc *pDoc = GetDocument();

	switch (nChar) {
	case VK_UP:
		if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow - 1][pDoc->mIcon.mPackmanCol] == 0)
			mKey = UP;
		else
			mNextKey = UP;
		break;
	case VK_RIGHT:
		if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol + 1] == 0)
			mKey = RITE;
		else
			mNextKey = RITE;
		break;
	case VK_LEFT:
		if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol - 1] == 0)
			mKey = LEFT;
		else
			mNextKey = LEFT;
		break;
	case VK_DOWN:
		if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow + 1][pDoc->mIcon.mPackmanCol] == 0)
			mKey = DOWN;
		else
			mNextKey = DOWN;
		break;
	case VK_SPACE:
		mKey = STOP;
		break;
	
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CMyPackManView::OnTimer(UINT_PTR nIDEvent)
{
	CMyPackManDoc *pDoc = GetDocument();
	
	if (nIDEvent == 1) {
		pDoc->mIcon.mBlinkyInfo.direction = rand() % 4 + 1;
		pDoc->mIcon.mBlinkyInfo.position = pDoc->mIcon.mBlinkyInfo.direction;
		
		pDoc->mIcon.mInkyInfo.direction = rand() % 4 + 1;
		pDoc->mIcon.mInkyInfo.position = pDoc->mIcon.mInkyInfo.direction;
		
		pDoc->mIcon.mPinkyInfo.direction = rand() % 4 + 1;
		pDoc->mIcon.mPinkyInfo.position = pDoc->mIcon.mPinkyInfo.direction;
		
		pDoc->mIcon.mClydeInfo.direction = rand() % 4 + 1;
		pDoc->mIcon.mClydeInfo.position = pDoc->mIcon.mClydeInfo.direction;
	}

	CView::OnTimer(nIDEvent);
}

UINT __cdecl introProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	Graphics grfx(view->m_hWnd);

	while (view->mGamePage == INTRO) {

		::WaitForSingleObject(gIntroEvent, INFINITE);

		view->mAllGrfx->Clear(Color(0, 0, 0));

		view->mAllGrfx->DrawImage(&pDoc->mScene.mLogo, pDoc->mScene.mLogoLeft, 50);

		if (pDoc->mScene.mMidFlag == 1)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mCharacter, pDoc->mScene.moveMid(), pDoc->mScene.mMidTop+50);
		else
			view->mAllGrfx->DrawImage(&pDoc->mScene.mCharacterR, pDoc->mScene.moveMid(), pDoc->mScene.mMidTop+50);

		if (pDoc->mScene.mButtonFlag == OFF)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mSbuttonU, pDoc->mScene.mButtonLeft, 550, pDoc->mScene.mSbuttonD.GetWidth(), pDoc->mScene.mSbuttonD.GetHeight());
		else
			view->mAllGrfx->DrawImage(&pDoc->mScene.mSbuttonD, pDoc->mScene.mButtonLeft, 550, pDoc->mScene.mSbuttonD.GetWidth(), pDoc->mScene.mSbuttonD.GetHeight());

		grfx.DrawImage(view->mAllBmp, 0, 0);
		Sleep(10);
	}
	return 0;
}

UINT __cdecl selectControlProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	Graphics grfx(view->m_hWnd);
	
	
	while (view->mGamePage == SELECTCONTROL) {

		::WaitForSingleObject(gSelectEvent, INFINITE);

		pDoc->mScene.fillBckGround(view->mAllGrfx, view->mWindowRect);

		if (pDoc->mScene.mKeyboardFlag == 1)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mKeyboardU, pDoc->mScene.mKeyLeft, pDoc->mScene.mSelectTop);
		else
			view->mAllGrfx->DrawImage(&pDoc->mScene.mKeyboardD, pDoc->mScene.mKeyLeft, pDoc->mScene.mSelectTop);

		if (pDoc->mScene.mMotionFlag == 1)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mMotionU, pDoc->mScene.mMotionLeft, pDoc->mScene.mSelectTop);
		else
			view->mAllGrfx->DrawImage(&pDoc->mScene.mMotionD, pDoc->mScene.mMotionLeft, pDoc->mScene.mSelectTop);

		if (pDoc->mScene.mSpeechFlag == 1)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mSpeechU, pDoc->mScene.mSpeechLeft, pDoc->mScene.mSelectTop);
		else
			view->mAllGrfx->DrawImage(&pDoc->mScene.mSpeechD, pDoc->mScene.mSpeechLeft, pDoc->mScene.mSelectTop);


		grfx.DrawImage(view->mAllBmp, 0, 0);
		Sleep(10);
	}
	return 0;
}

UINT __cdecl packmanProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();


	while (1) {
		::WaitForSingleObject(gGameEvent, INFINITE);
		Sleep(1);

		::EnterCriticalSection(&view->mPackmanEvent);

		pDoc->mIcon.packmanMove(view->mKey, view->mNextKey, pDoc->mGame);

		if (pDoc->mGame.mMapForPacman[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol] == 3) {
			pDoc->mGame.mMapForPacman[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol] = 0;
			pDoc->mGame.mScore += 10;
			pDoc->mGame.mDotCnt--;

			if (pDoc->mGame.mDotCnt == 0)
			{
				pDoc->mIcon.setPosition();
				pDoc->mGame.initMap();
				pDoc->mGame.mReadyFlag = true;
				pDoc->mGame.mRound++;
				Sleep(3000);
				pDoc->mGame.mReadyFlag = false;
			}
			if (pDoc->mGame.mScore == 10000) {
				pDoc->mGame.mLife++;
			}
			PlaySound(L"res/pacman BGM/candy_sub.wav", NULL, SND_ASYNC);
		}

		if (pDoc->mGame.mMapForPacman[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol] == 5) {
			pDoc->mGame.mMapForPacman[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol] = 0;
			pDoc->mGame.mScore += 50;
			pDoc->mGame.mDotCnt--;

			if (pDoc->mGame.mDotCnt == 0)	{

				pDoc->mIcon.setPosition();
				pDoc->mGame.initMap();
				pDoc->mGame.mReadyFlag = true;
				pDoc->mGame.mRound++;
				Sleep(3000);
				pDoc->mGame.mReadyFlag = false;				
			}

			if (pDoc->mGame.mScore == 10000) {
				pDoc->mGame.mLife++;
			}
			
			pDoc->mIcon.mGhostEat = 1;
		}

		if (pDoc->mGame.mMapForPacman[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol] == 7) {
			pDoc->mGame.mMapForPacman[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol] = 0;
			pDoc->mIcon.mItemEat = 1;
			
			PlaySound(L"res/pacman BGM/eatFruit.wav", NULL, SND_ASYNC);

			if (pDoc->mIcon.mItemEat == 1) {
				if (pDoc->mGame.mRound % 4 == 1)
					pDoc->mGame.mScore += 100;
				if (pDoc->mGame.mRound % 4 == 2)
					pDoc->mGame.mScore += 200;
				if (pDoc->mGame.mRound % 4 == 3)
					pDoc->mGame.mScore += 500;
				if (pDoc->mGame.mRound % 4 == 0)
					pDoc->mGame.mScore += 700;
			}

			if (pDoc->mGame.mScore == 10000) {
				pDoc->mGame.mLife++;
			}
		}
		
		if (pDoc->mIcon.mPackmanRow == pDoc->mIcon.mBlinkyInfo.row	&& pDoc->mIcon.mPackmanCol == pDoc->mIcon.mBlinkyInfo.col) {
			if (pDoc->mIcon.mGhostEat == 1) {
				pDoc->mIcon.mBlinkyInfo.hunt = 1;
				pDoc->mGame.mHuntCount += 1;
				if (pDoc->mGame.mHuntCount == 1)
					pDoc->mGame.mScore += 200;
				if (pDoc->mGame.mHuntCount == 2)
					pDoc->mGame.mScore += 400;
				if (pDoc->mGame.mHuntCount == 3)
					pDoc->mGame.mScore += 800;
				if (pDoc->mGame.mHuntCount == 4)
					pDoc->mGame.mScore += 1600;
				PlaySound(L"res/pacman BGM/eatGhost.wav", NULL, SND_ASYNC);
				pDoc->mIcon.resetBlinkyPosition();
				if (pDoc->mGame.mScore == 10000) {
					pDoc->mGame.mLife++;
				}
			}
			else
				pDoc->mIcon.mCollision = 1;
		}

		if (pDoc->mIcon.mPackmanRow == pDoc->mIcon.mInkyInfo.row	&& pDoc->mIcon.mPackmanCol == pDoc->mIcon.mInkyInfo.col) {
			if (pDoc->mIcon.mGhostEat == 1) {
				pDoc->mIcon.mInkyInfo.hunt = 1;
				pDoc->mGame.mHuntCount += 1;
				if (pDoc->mGame.mHuntCount == 1)
					pDoc->mGame.mScore += 200;
				if (pDoc->mGame.mHuntCount == 2)
					pDoc->mGame.mScore += 400;
				if (pDoc->mGame.mHuntCount == 3)
					pDoc->mGame.mScore += 800;
				if (pDoc->mGame.mHuntCount == 4)
					pDoc->mGame.mScore += 1600;
				PlaySound(L"res/pacman BGM/eatGhost.wav", NULL, SND_ASYNC);
				pDoc->mIcon.resetInkyPosition();
				if (pDoc->mGame.mScore == 10000) {
					pDoc->mGame.mLife++;
				}
			}
			else
				pDoc->mIcon.mCollision = 1;
		}

		if (pDoc->mIcon.mPackmanRow == pDoc->mIcon.mPinkyInfo.row	&& pDoc->mIcon.mPackmanCol == pDoc->mIcon.mPinkyInfo.col) {
			if (pDoc->mIcon.mGhostEat == 1) {
				pDoc->mIcon.mPinkyInfo.hunt = 1;
				pDoc->mGame.mHuntCount += 1;
				if (pDoc->mGame.mHuntCount == 1)
					pDoc->mGame.mScore += 200;
				if (pDoc->mGame.mHuntCount == 2)
					pDoc->mGame.mScore += 400;
				if (pDoc->mGame.mHuntCount == 3)
					pDoc->mGame.mScore += 800;
				if (pDoc->mGame.mHuntCount == 4)
					pDoc->mGame.mScore += 1600;
				PlaySound(L"res/pacman BGM/eatGhost.wav", NULL, SND_ASYNC);
				pDoc->mIcon.resetPinkyPosition();
				if (pDoc->mGame.mScore == 10000) {
					pDoc->mGame.mLife++;
				}
			}
			else
				pDoc->mIcon.mCollision = 1;
		}


		if (pDoc->mIcon.mPackmanRow == pDoc->mIcon.mClydeInfo.row	&& pDoc->mIcon.mPackmanCol == pDoc->mIcon.mClydeInfo.col) {
			if (pDoc->mIcon.mGhostEat == 1) {
				pDoc->mIcon.mClydeInfo.hunt = 1;
				pDoc->mGame.mHuntCount += 1;
				if (pDoc->mGame.mHuntCount == 1)
					pDoc->mGame.mScore += 200;
				if (pDoc->mGame.mHuntCount == 2)
					pDoc->mGame.mScore += 400;
				if (pDoc->mGame.mHuntCount == 3)
					pDoc->mGame.mScore += 800;
				if (pDoc->mGame.mHuntCount == 4)
					pDoc->mGame.mScore += 1600;
				PlaySound(L"res/pacman BGM/eatGhost.wav", NULL, SND_ASYNC);
				pDoc->mIcon.resetClydePosition();
				if (pDoc->mGame.mScore == 10000) {
					pDoc->mGame.mLife++;
				}
			}
			else
				pDoc->mIcon.mCollision = 1;
		}

		if (pDoc->mIcon.mCollision == 1)
		{
			pDoc->mIcon.mCollision = 0;
			pDoc->mGame.mLife--;
			pDoc->mIcon.setPosition();
			view->mKey = STOP;
			if (pDoc->mGame.mLife != 0)
				pDoc->mGame.mReadyFlag = true;
			ResetEvent(gGameEvent);
			Sleep(3000);
			pDoc->mGame.mReadyFlag = false;
			SetEvent(gGameEvent);
			
				
		}

		::LeaveCriticalSection(&view->mPackmanEvent);
		
	
		Sleep(15);
	}

	return 0;
}

UINT __cdecl BlinkyProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	while (1) {
		::WaitForSingleObject(gGameEvent, INFINITE);
		Sleep(1);

		::EnterCriticalSection(&view->mBlinkyEvent);

		if ((pDoc->mIcon.mBlinkyInfo.row == 9 && pDoc->mIcon.mBlinkyInfo.col == 9)
			|| (pDoc->mIcon.mBlinkyInfo.row == 10 && pDoc->mIcon.mBlinkyInfo.col == 9))
			pDoc->mIcon.mBlinkyInfo.direction = UP;

		Sleep(2);

		switch (pDoc->mIcon.mBlinkyInfo.direction)
		{
		case LEFT:
			if (pDoc->mIcon.mBlinkyInfo.row == 10 && pDoc->mIcon.mBlinkyInfo.col == 0) {
				pDoc->mIcon.mBlinkyInfo.pixelX = 845;
				pDoc->mIcon.mBlinkyInfo.pixelY = 338;
				pDoc->mIcon.mBlinkyInfo.col = 18;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row][pDoc->mIcon.mBlinkyInfo.col - 1] == 0
				&& (pDoc->mIcon.mBlinkyInfo.mUy == 0 && pDoc->mIcon.mBlinkyInfo.mDy == 0)) {
				pDoc->mIcon.mBlinkyInfo.pixelX += -2;
				pDoc->mIcon.mBlinkyInfo.mLx += 2;
				if (pDoc->mIcon.mBlinkyInfo.mRx > 0) {
					pDoc->mIcon.mBlinkyInfo.mLx -= pDoc->mIcon.mBlinkyInfo.mRx;
					pDoc->mIcon.mBlinkyInfo.mRx = 0;
				}
				if (pDoc->mIcon.mBlinkyInfo.mLx > 29) {
					pDoc->mIcon.mBlinkyInfo.mLx = 0; // *
					pDoc->mIcon.mBlinkyInfo.col -= 1;
				}
			}
			else
				pDoc->mIcon.mBlinkyInfo.direction = rand() % 4 + 1;
			break;
		case RITE:
			if (pDoc->mIcon.mBlinkyInfo.row == 10 && pDoc->mIcon.mBlinkyInfo.col == 18) {
				pDoc->mIcon.mBlinkyInfo.pixelX = 305;
				pDoc->mIcon.mBlinkyInfo.pixelY = 338;
				pDoc->mIcon.mBlinkyInfo.col = 0;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row][pDoc->mIcon.mBlinkyInfo.col + 1] == 0
				&& (pDoc->mIcon.mBlinkyInfo.mUy == 0 && pDoc->mIcon.mBlinkyInfo.mDy == 0)) {
				pDoc->mIcon.mBlinkyInfo.pixelX += 2;
				pDoc->mIcon.mBlinkyInfo.mRx += 2;
				if (pDoc->mIcon.mBlinkyInfo.mLx > 0) {
					pDoc->mIcon.mBlinkyInfo.mRx -= pDoc->mIcon.mBlinkyInfo.mLx;
					pDoc->mIcon.mBlinkyInfo.mLx = 0;
				}
				if (pDoc->mIcon.mBlinkyInfo.mRx > 29) {
					pDoc->mIcon.mBlinkyInfo.mRx = 0; // *
					pDoc->mIcon.mBlinkyInfo.col += 1;
				}
			}
			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row - 1][pDoc->mIcon.mBlinkyInfo.col] == 0
				&& (pDoc->mIcon.mBlinkyInfo.mLx == 0 && pDoc->mIcon.mBlinkyInfo.mRx == 0)) {
				pDoc->mIcon.mBlinkyInfo.pixelY += -2;
				pDoc->mIcon.mBlinkyInfo.mUy += 2;
				if (pDoc->mIcon.mBlinkyInfo.mDy > 0) {
					pDoc->mIcon.mBlinkyInfo.mUy -= pDoc->mIcon.mBlinkyInfo.mDy;
					pDoc->mIcon.mBlinkyInfo.mDy = 0;
				}
				if (pDoc->mIcon.mBlinkyInfo.mUy > 29) {
					pDoc->mIcon.mBlinkyInfo.mUy = 0; // *
					pDoc->mIcon.mBlinkyInfo.row -= 1;
				}
			}
			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row + 1][pDoc->mIcon.mBlinkyInfo.col] == 0
				&& (pDoc->mIcon.mBlinkyInfo.mLx == 0 && pDoc->mIcon.mBlinkyInfo.mRx == 0)) {
				pDoc->mIcon.mBlinkyInfo.pixelY += 2;
				pDoc->mIcon.mBlinkyInfo.mDy += 2;
				if (pDoc->mIcon.mBlinkyInfo.mUy > 0) {
					pDoc->mIcon.mBlinkyInfo.mDy -= pDoc->mIcon.mBlinkyInfo.mUy;
					pDoc->mIcon.mBlinkyInfo.mUy = 0;
				}
				if (pDoc->mIcon.mBlinkyInfo.mDy > 29) {
					pDoc->mIcon.mBlinkyInfo.mDy = 0; // *
					pDoc->mIcon.mBlinkyInfo.row += 1;
				}
			}
			break;
		}

		pDoc->mIcon.mBlinkyInfo.prevDirection = pDoc->mIcon.mBlinkyInfo.direction;

		switch (pDoc->mIcon.mBlinkyInfo.direction)
		{
		case LEFT:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row][pDoc->mIcon.mBlinkyInfo.col - 1] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mBlinkyInfo.prevDirection, pDoc->mIcon.mBlinkyInfo.prevDirection
				, pDoc->mIcon.mBlinkyInfo.row, pDoc->mIcon.mBlinkyInfo.col);
			break;
		case RITE:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row][pDoc->mIcon.mBlinkyInfo.col + 1] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mBlinkyInfo.prevDirection, pDoc->mIcon.mBlinkyInfo.prevDirection
				, pDoc->mIcon.mBlinkyInfo.row, pDoc->mIcon.mBlinkyInfo.col);

			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row - 1][pDoc->mIcon.mBlinkyInfo.col] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mBlinkyInfo.prevDirection, pDoc->mIcon.mBlinkyInfo.prevDirection
				, pDoc->mIcon.mBlinkyInfo.row, pDoc->mIcon.mBlinkyInfo.col);
			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mBlinkyInfo.row + 1][pDoc->mIcon.mBlinkyInfo.col] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mBlinkyInfo.prevDirection, pDoc->mIcon.mBlinkyInfo.prevDirection
				, pDoc->mIcon.mBlinkyInfo.row, pDoc->mIcon.mBlinkyInfo.col);
			break;
		}
		::LeaveCriticalSection(&view->mBlinkyEvent);
		Sleep(10);
	}

	return 0;
}

UINT __cdecl InkyProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	while (1) {
		::WaitForSingleObject(gGameEvent, INFINITE);
		
		Sleep(5);

		::EnterCriticalSection(&view->mInkyEvent);

		if ((pDoc->mIcon.mInkyInfo.row == 10 && pDoc->mIcon.mInkyInfo.col == 8)
			|| (pDoc->mIcon.mInkyInfo.row == 10 && pDoc->mIcon.mInkyInfo.col == 9)
			|| (pDoc->mIcon.mInkyInfo.row == 9 && pDoc->mIcon.mInkyInfo.col == 9)) {

			if ((pDoc->mIcon.mInkyInfo.row == 9 && pDoc->mIcon.mInkyInfo.col == 9)
				|| (pDoc->mIcon.mInkyInfo.row == 10 && pDoc->mIcon.mInkyInfo.col == 9))
				pDoc->mIcon.mInkyInfo.direction = UP;

			else
				pDoc->mIcon.mInkyInfo.direction = RITE;
		}

		switch (pDoc->mIcon.mInkyInfo.direction)
		{
		case LEFT:
			if (pDoc->mIcon.mInkyInfo.row == 10 && pDoc->mIcon.mInkyInfo.col == 0) {
				pDoc->mIcon.mInkyInfo.pixelX = 845;
				pDoc->mIcon.mInkyInfo.pixelY = 338;
				pDoc->mIcon.mInkyInfo.col = 18;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row][pDoc->mIcon.mInkyInfo.col - 1] == 0
				&& (pDoc->mIcon.mInkyInfo.mUy == 0 && pDoc->mIcon.mInkyInfo.mDy == 0)) {
				pDoc->mIcon.mInkyInfo.pixelX += -2;
				pDoc->mIcon.mInkyInfo.mLx += 2;
				if (pDoc->mIcon.mInkyInfo.mRx > 0) {
					pDoc->mIcon.mInkyInfo.mLx -= pDoc->mIcon.mInkyInfo.mRx;
					pDoc->mIcon.mInkyInfo.mRx = 0;
				}
				if (pDoc->mIcon.mInkyInfo.mLx > 29) {
					pDoc->mIcon.mInkyInfo.mLx = 0; // *
					pDoc->mIcon.mInkyInfo.col -= 1;
				}
			}
			else
				pDoc->mIcon.mInkyInfo.direction = rand() % 4 + 1;

			break;
		case RITE:
			if (pDoc->mIcon.mInkyInfo.row == 10 && pDoc->mIcon.mInkyInfo.col == 18) {
				pDoc->mIcon.mInkyInfo.pixelX = 305;
				pDoc->mIcon.mInkyInfo.pixelY = 338;
				pDoc->mIcon.mInkyInfo.col = 0;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row][pDoc->mIcon.mInkyInfo.col + 1] == 0
				&& (pDoc->mIcon.mInkyInfo.mUy == 0 && pDoc->mIcon.mInkyInfo.mDy == 0)) {
				pDoc->mIcon.mInkyInfo.pixelX += 2;
				pDoc->mIcon.mInkyInfo.mRx += 2;
				if (pDoc->mIcon.mInkyInfo.mLx > 0) {
					pDoc->mIcon.mInkyInfo.mRx -= pDoc->mIcon.mInkyInfo.mLx;
					pDoc->mIcon.mInkyInfo.mLx = 0;
				}
				if (pDoc->mIcon.mInkyInfo.mRx > 29) {
					pDoc->mIcon.mInkyInfo.mRx = 0; // *
					pDoc->mIcon.mInkyInfo.col += 1;
				}
			}
			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row - 1][pDoc->mIcon.mInkyInfo.col] == 0
				&& (pDoc->mIcon.mInkyInfo.mLx == 0 && pDoc->mIcon.mInkyInfo.mRx == 0)) {
				pDoc->mIcon.mInkyInfo.pixelY += -2;
				pDoc->mIcon.mInkyInfo.mUy += 2;
				if (pDoc->mIcon.mInkyInfo.mDy > 0) {
					pDoc->mIcon.mInkyInfo.mUy -= pDoc->mIcon.mInkyInfo.mDy;
					pDoc->mIcon.mInkyInfo.mDy = 0;
				}
				if (pDoc->mIcon.mInkyInfo.mUy > 29) {
					pDoc->mIcon.mInkyInfo.mUy = 0; // *
					pDoc->mIcon.mInkyInfo.row -= 1;
				}
			}
			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row + 1][pDoc->mIcon.mInkyInfo.col] == 0
				&& (pDoc->mIcon.mInkyInfo.mLx == 0 && pDoc->mIcon.mInkyInfo.mRx == 0)) {
				pDoc->mIcon.mInkyInfo.pixelY += 2;
				pDoc->mIcon.mInkyInfo.mDy += 2;
				if (pDoc->mIcon.mInkyInfo.mUy > 0) {
					pDoc->mIcon.mInkyInfo.mDy -= pDoc->mIcon.mInkyInfo.mUy;
					pDoc->mIcon.mInkyInfo.mUy = 0;
				}
				if (pDoc->mIcon.mInkyInfo.mDy > 29) {
					pDoc->mIcon.mInkyInfo.mDy = 0; // *
					pDoc->mIcon.mInkyInfo.row += 1;
				}
			}
			break;
		}

		pDoc->mIcon.mInkyInfo.prevDirection = pDoc->mIcon.mInkyInfo.direction;

		switch (pDoc->mIcon.mInkyInfo.direction)
		{
		case LEFT:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row][pDoc->mIcon.mInkyInfo.col - 1] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mInkyInfo.prevDirection, pDoc->mIcon.mInkyInfo.prevDirection
				, pDoc->mIcon.mInkyInfo.row, pDoc->mIcon.mInkyInfo.col);

			break;
		case RITE:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row][pDoc->mIcon.mInkyInfo.col + 1] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mInkyInfo.prevDirection, pDoc->mIcon.mInkyInfo.prevDirection
				, pDoc->mIcon.mInkyInfo.row, pDoc->mIcon.mInkyInfo.col);

			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row - 1][pDoc->mIcon.mInkyInfo.col] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mInkyInfo.prevDirection, pDoc->mIcon.mInkyInfo.prevDirection
				, pDoc->mIcon.mInkyInfo.row, pDoc->mIcon.mInkyInfo.col);

			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mInkyInfo.row + 1][pDoc->mIcon.mInkyInfo.col] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mInkyInfo.prevDirection, pDoc->mIcon.mInkyInfo.prevDirection
				, pDoc->mIcon.mInkyInfo.row, pDoc->mIcon.mInkyInfo.col);

			break;
		}
		::LeaveCriticalSection(&view->mInkyEvent);
		Sleep(20);
	}
	return 0;
}

UINT __cdecl PinkyProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();


	while (1) {

		::WaitForSingleObject(gGameEvent, INFINITE);
		Sleep(1);

		::EnterCriticalSection(&view->mPinkyEvent);

		if ((pDoc->mIcon.mPinkyInfo.row == 9 && pDoc->mIcon.mPinkyInfo.col == 9)
			|| (pDoc->mIcon.mPinkyInfo.row == 10 && pDoc->mIcon.mPinkyInfo.col == 9))
			pDoc->mIcon.mPinkyInfo.direction = UP;


		switch (pDoc->mIcon.mPinkyInfo.direction)
		{
		case LEFT:
			if (pDoc->mIcon.mPinkyInfo.row == 10 && pDoc->mIcon.mPinkyInfo.col == 0) {
				pDoc->mIcon.mPinkyInfo.pixelX = 845;
				pDoc->mIcon.mPinkyInfo.pixelY = 338;
				pDoc->mIcon.mPinkyInfo.col = 18;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row][pDoc->mIcon.mPinkyInfo.col - 1] == 0
				&& (pDoc->mIcon.mPinkyInfo.mUy == 0 && pDoc->mIcon.mPinkyInfo.mDy == 0)) {
				pDoc->mIcon.mPinkyInfo.pixelX += -2;
				pDoc->mIcon.mPinkyInfo.mLx += 2;
				if (pDoc->mIcon.mPinkyInfo.mRx > 0) {
					pDoc->mIcon.mPinkyInfo.mLx -= pDoc->mIcon.mPinkyInfo.mRx;
					pDoc->mIcon.mPinkyInfo.mRx = 0;
				}
				if (pDoc->mIcon.mPinkyInfo.mLx > 29) {
					pDoc->mIcon.mPinkyInfo.mLx = 0; // *
					pDoc->mIcon.mPinkyInfo.col -= 1;
				}
			}
			else
				pDoc->mIcon.mPinkyInfo.direction = rand() % 4 + 1;

			break;
		case RITE:
			if (pDoc->mIcon.mPinkyInfo.row == 10 && pDoc->mIcon.mPinkyInfo.col == 18) {
				pDoc->mIcon.mPinkyInfo.pixelX = 305;
				pDoc->mIcon.mPinkyInfo.pixelY = 338;
				pDoc->mIcon.mPinkyInfo.col = 0;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row][pDoc->mIcon.mPinkyInfo.col + 1] == 0
				&& (pDoc->mIcon.mPinkyInfo.mUy == 0 && pDoc->mIcon.mPinkyInfo.mDy == 0)) {
				pDoc->mIcon.mPinkyInfo.pixelX += 2;
				pDoc->mIcon.mPinkyInfo.mRx += 2;
				if (pDoc->mIcon.mPinkyInfo.mLx > 0) {
					pDoc->mIcon.mPinkyInfo.mRx -= pDoc->mIcon.mPinkyInfo.mLx;
					pDoc->mIcon.mPinkyInfo.mLx = 0;
				}
				if (pDoc->mIcon.mPinkyInfo.mRx > 29) {
					pDoc->mIcon.mPinkyInfo.mRx = 0; // *
					pDoc->mIcon.mPinkyInfo.col += 1;
				}
			}
			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row - 1][pDoc->mIcon.mPinkyInfo.col] == 0
				&& (pDoc->mIcon.mPinkyInfo.mLx == 0 && pDoc->mIcon.mPinkyInfo.mRx == 0)) {
				pDoc->mIcon.mPinkyInfo.pixelY += -2;
				pDoc->mIcon.mPinkyInfo.mUy += 2;
				if (pDoc->mIcon.mPinkyInfo.mDy > 0) {
					pDoc->mIcon.mPinkyInfo.mUy -= pDoc->mIcon.mPinkyInfo.mDy;
					pDoc->mIcon.mPinkyInfo.mDy = 0;
				}
				if (pDoc->mIcon.mPinkyInfo.mUy > 29) {
					pDoc->mIcon.mPinkyInfo.mUy = 0; // *
					pDoc->mIcon.mPinkyInfo.row -= 1;
				}
			}
			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row + 1][pDoc->mIcon.mPinkyInfo.col] == 0
				&& (pDoc->mIcon.mPinkyInfo.mLx == 0 && pDoc->mIcon.mPinkyInfo.mRx == 0)) {
				pDoc->mIcon.mPinkyInfo.pixelY += 2;
				pDoc->mIcon.mPinkyInfo.mDy += 2;
				if (pDoc->mIcon.mPinkyInfo.mUy > 0) {
					pDoc->mIcon.mPinkyInfo.mDy -= pDoc->mIcon.mPinkyInfo.mUy;
					pDoc->mIcon.mPinkyInfo.mUy = 0;
				}
				if (pDoc->mIcon.mPinkyInfo.mDy > 29) {
					pDoc->mIcon.mPinkyInfo.mDy = 0; // *
					pDoc->mIcon.mPinkyInfo.row += 1;
				}
			}
			break;
		}

		pDoc->mIcon.mPinkyInfo.prevDirection = pDoc->mIcon.mPinkyInfo.direction;

		switch (pDoc->mIcon.mPinkyInfo.direction)
		{
		case LEFT:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row][pDoc->mIcon.mPinkyInfo.col - 1] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mPinkyInfo.prevDirection, pDoc->mIcon.mPinkyInfo.prevDirection
				, pDoc->mIcon.mPinkyInfo.row, pDoc->mIcon.mPinkyInfo.col);

			break;
		case RITE:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row][pDoc->mIcon.mPinkyInfo.col + 1] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mPinkyInfo.prevDirection, pDoc->mIcon.mPinkyInfo.prevDirection
				, pDoc->mIcon.mPinkyInfo.row, pDoc->mIcon.mPinkyInfo.col);

			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row - 1][pDoc->mIcon.mPinkyInfo.col] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mPinkyInfo.prevDirection, pDoc->mIcon.mPinkyInfo.prevDirection
				, pDoc->mIcon.mPinkyInfo.row, pDoc->mIcon.mPinkyInfo.col);

			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPinkyInfo.row + 1][pDoc->mIcon.mPinkyInfo.col] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mPinkyInfo.prevDirection, pDoc->mIcon.mPinkyInfo.prevDirection
				, pDoc->mIcon.mPinkyInfo.row, pDoc->mIcon.mPinkyInfo.col);

			break;
		}
		Sleep(10);
		::LeaveCriticalSection(&view->mPinkyEvent);
	}
	return 0;
}

UINT __cdecl clydeProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	while (1) {
		
		::WaitForSingleObject(gGameEvent, INFINITE);
		Sleep(10);

		::EnterCriticalSection(&view->mClydeEvent);

		if ((pDoc->mIcon.mClydeInfo.row == 10 && pDoc->mIcon.mClydeInfo.col == 10)
			|| (pDoc->mIcon.mClydeInfo.row == 10 && pDoc->mIcon.mClydeInfo.col == 9)
			|| (pDoc->mIcon.mClydeInfo.row == 9 && pDoc->mIcon.mClydeInfo.col == 9)) {

			if ((pDoc->mIcon.mClydeInfo.row == 9 && pDoc->mIcon.mClydeInfo.col == 9)
				|| (pDoc->mIcon.mClydeInfo.row == 10 && pDoc->mIcon.mClydeInfo.col == 9))
				pDoc->mIcon.mClydeInfo.direction = UP;

			else
				pDoc->mIcon.mClydeInfo.direction = LEFT;
		}

		switch (pDoc->mIcon.mClydeInfo.direction)
		{
		case LEFT:
			if (pDoc->mIcon.mClydeInfo.row == 10 && pDoc->mIcon.mClydeInfo.col == 0) {
				pDoc->mIcon.mClydeInfo.pixelX = 845;
				pDoc->mIcon.mClydeInfo.pixelY = 338;
				pDoc->mIcon.mClydeInfo.col = 18;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row][pDoc->mIcon.mClydeInfo.col - 1] == 0
				&& (pDoc->mIcon.mClydeInfo.mUy == 0 && pDoc->mIcon.mClydeInfo.mDy == 0)) {
				pDoc->mIcon.mClydeInfo.pixelX += -2;
				pDoc->mIcon.mClydeInfo.mLx += 2;
				if (pDoc->mIcon.mClydeInfo.mRx > 0) {
					pDoc->mIcon.mClydeInfo.mLx -= pDoc->mIcon.mClydeInfo.mRx;
					pDoc->mIcon.mClydeInfo.mRx = 0;
				}
				if (pDoc->mIcon.mClydeInfo.mLx > 29) {
					pDoc->mIcon.mClydeInfo.mLx = 0; // *
					pDoc->mIcon.mClydeInfo.col -= 1;
				}
			}
			else
				pDoc->mIcon.mClydeInfo.direction = rand() % 4 + 1;

			break;
		case RITE:
			if (pDoc->mIcon.mClydeInfo.row == 10 && pDoc->mIcon.mClydeInfo.col == 18) {
				pDoc->mIcon.mClydeInfo.pixelX = 305;
				pDoc->mIcon.mClydeInfo.pixelY = 338;
				pDoc->mIcon.mClydeInfo.col = 0;
			}
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row][pDoc->mIcon.mClydeInfo.col + 1] == 0
				&& (pDoc->mIcon.mClydeInfo.mUy == 0 && pDoc->mIcon.mClydeInfo.mDy == 0)) {
				pDoc->mIcon.mClydeInfo.pixelX += 2;
				pDoc->mIcon.mClydeInfo.mRx += 2;
				if (pDoc->mIcon.mClydeInfo.mLx > 0) {
					pDoc->mIcon.mClydeInfo.mRx -= pDoc->mIcon.mClydeInfo.mLx;
					pDoc->mIcon.mClydeInfo.mLx = 0;
				}
				if (pDoc->mIcon.mClydeInfo.mRx > 29) {
					pDoc->mIcon.mClydeInfo.mRx = 0; // *
					pDoc->mIcon.mClydeInfo.col += 1;
				}
			}
			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row - 1][pDoc->mIcon.mClydeInfo.col] == 0
				&& (pDoc->mIcon.mClydeInfo.mLx == 0 && pDoc->mIcon.mClydeInfo.mRx == 0)) {
				pDoc->mIcon.mClydeInfo.pixelY += -2;
				pDoc->mIcon.mClydeInfo.mUy += 2;
				if (pDoc->mIcon.mClydeInfo.mDy > 0) {
					pDoc->mIcon.mClydeInfo.mUy -= pDoc->mIcon.mClydeInfo.mDy;
					pDoc->mIcon.mClydeInfo.mDy = 0;
				}
				if (pDoc->mIcon.mClydeInfo.mUy > 29) {
					pDoc->mIcon.mClydeInfo.mUy = 0; // *
					pDoc->mIcon.mClydeInfo.row -= 1;
				}
			}
			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row + 1][pDoc->mIcon.mClydeInfo.col] == 0 
				&& (pDoc->mIcon.mClydeInfo.mLx == 0 && pDoc->mIcon.mClydeInfo.mRx == 0)) {
				pDoc->mIcon.mClydeInfo.pixelY += 2;
				pDoc->mIcon.mClydeInfo.mDy += 2;
				if (pDoc->mIcon.mClydeInfo.mUy > 0) {
					pDoc->mIcon.mClydeInfo.mDy -= pDoc->mIcon.mClydeInfo.mUy;
					pDoc->mIcon.mClydeInfo.mUy = 0;
				}
				if (pDoc->mIcon.mClydeInfo.mDy > 29) {
					pDoc->mIcon.mClydeInfo.mDy = 0; // *
					pDoc->mIcon.mClydeInfo.row += 1;
				}
			}
			break;
		}

		pDoc->mIcon.mClydeInfo.prevDirection = pDoc->mIcon.mClydeInfo.direction;
		
		switch (pDoc->mIcon.mClydeInfo.direction)
		{
		case LEFT:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row][pDoc->mIcon.mClydeInfo.col - 1] == 0) 
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mClydeInfo.prevDirection, pDoc->mIcon.mClydeInfo.prevDirection
				, pDoc->mIcon.mClydeInfo.row, pDoc->mIcon.mClydeInfo.col);

			break;
		case RITE:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row][pDoc->mIcon.mClydeInfo.col + 1] == 0) 
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mClydeInfo.prevDirection, pDoc->mIcon.mClydeInfo.prevDirection
				, pDoc->mIcon.mClydeInfo.row, pDoc->mIcon.mClydeInfo.col);
		
			break;
		case UP:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row - 1][pDoc->mIcon.mClydeInfo.col] == 0)
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mClydeInfo.prevDirection, pDoc->mIcon.mClydeInfo.prevDirection
				, pDoc->mIcon.mClydeInfo.row, pDoc->mIcon.mClydeInfo.col);

			break;
		case DOWN:
			if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mClydeInfo.row + 1][pDoc->mIcon.mClydeInfo.col] == 0) 
				pDoc->mIcon.ghostMove(pDoc->mGame, pDoc->mIcon.mClydeInfo.prevDirection, pDoc->mIcon.mClydeInfo.prevDirection
				, pDoc->mIcon.mClydeInfo.row, pDoc->mIcon.mClydeInfo.col);

			break;
		}
		::LeaveCriticalSection(&view->mClydeEvent);
		Sleep(15);
	}
	return 0;
}

UINT __cdecl drawProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	Graphics grfx(view->m_hWnd);
	Sleep(1);

	while (1) {

		::WaitForSingleObject(gDrawEvent, INFINITE);

		pDoc->mScene.fillBckGround(view->mAllGrfx, view->mWindowRect);

		view->mAllGrfx->DrawImage(&pDoc->mGame.mMap, pDoc->mGame.mLeftTop.x, pDoc->mGame.mLeftTop.y);

		Gdiplus::Rect logoRc(pDoc->mGame.mCx - 250, 100, 200, 100);
		view->mAllGrfx->DrawImage(&pDoc->mScene.mLogo, logoRc);

		Gdiplus::FontFamily fontFamily(L"굴림체");
		Gdiplus::Font ft(&fontFamily, 30, FontStyleRegular, UnitPixel);

		RectF rf(pDoc->mGame.mCx - 250, 300, 150, 100);
		RectF rf2(pDoc->mGame.mCx - 250, 350, 150, 100);
		RectF rf3(pDoc->mGame.mCx - 250, 450, 150, 100);
		RectF rf4(pDoc->mGame.mCx - 150, 600, 60, 60);
		RectF rf5(pDoc->mGame.mLeftTop.x - 220, pDoc->mGame.mLeftTop.y + 170, 150, 300);
		RectF rf7(pDoc->mGame.mLeftTop.x - 220, pDoc->mGame.mLeftTop.y + 170, 150, 300);
		RectF rf8(pDoc->mGame.mLeftTop.x - 220, pDoc->mGame.mLeftTop.y + 170, 150, 300);

		SolidBrush B(Color(255, 255, 255));
		StringFormat stringFormat;
		stringFormat.SetAlignment(StringAlignmentFar);

		CString score;
		score.Format(L"%d", pDoc->mGame.mScore);

		view->mAllGrfx->DrawString(L"SCORE", -1, &ft, rf, &stringFormat, &B);
		view->mAllGrfx->DrawString(score, -1, &ft, rf2, &stringFormat, &B);
		view->mAllGrfx->DrawString(L"LIVES", -1, &ft, rf3, &stringFormat, &B);
		view->mAllGrfx->DrawImage(&pDoc->mIcon.mItem, rf4, 0, (pDoc->mGame.mRound % 4) * 30, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);

		if (view->mGamePage == KEYBOARD)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mKeyboardD, rf5, 0, 0, pDoc->mScene.mKeyboardU.GetWidth(), pDoc->mScene.mKeyboardU.GetHeight(), UnitPixel);

		if (view->mGamePage == MOTION)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mMotionD, rf7, 0, 0, pDoc->mScene.mMotionD.GetWidth(), pDoc->mScene.mMotionD.GetHeight(), UnitPixel);

		if (view->mGamePage == SPEECH)
			view->mAllGrfx->DrawImage(&pDoc->mScene.mSpeechD, rf8, 0, 0, pDoc->mScene.mSpeechD.GetWidth(), pDoc->mScene.mSpeechD.GetHeight(), UnitPixel);


		for (int i = 1; i < pDoc->mGame.mLife; i++)
			view->mAllGrfx->DrawImage(&pDoc->mIcon.mPackManForLife, pDoc->mGame.mCx - 100 - (i * 35), 500, 30, 30);

		for (int i = 0; i < 22; i++) {
			for (int j = 0; j < 19; j++) {
				if (pDoc->mGame.mMapForPacman[12][9] == 7)
					view->mAllGrfx->DrawImage(&pDoc->mIcon.mItem, Gdiplus::Rect(pDoc->mGame.mLeftTop.x + (9 * 30), pDoc->mGame.mLeftTop.y + (12 * 30), pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight), 0, (pDoc->mGame.mRound % 4) * 30, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);
				if (pDoc->mGame.mMapForPacman[12][9] == 0)
					view->mAllGrfx->DrawImage(&pDoc->mIcon.mItem, Gdiplus::Rect(pDoc->mGame.mLeftTop.x + (9 * 30), pDoc->mGame.mLeftTop.y + (12 * 30), pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight), 0, 0, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);

				if (pDoc->mGame.mMapForPacman[i][j] == 3)
					view->mAllGrfx->DrawImage(&pDoc->mIcon.mSmallCandy, Gdiplus::Rect(pDoc->mGame.mLeftTop.x + (j * 30), pDoc->mGame.mLeftTop.y + (i * 30), pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight), 0, 0, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);
				if (pDoc->mGame.mMapForPacman[i][j] == 5)
					view->mAllGrfx->DrawImage(&pDoc->mIcon.mBigCandy, Gdiplus::Rect(pDoc->mGame.mLeftTop.x + (j * 30), pDoc->mGame.mLeftTop.y + (i * 30), pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight), 0, 0, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);

			}
		}

		view->mAllGrfx->DrawImage(&pDoc->mIcon.mPackman,
			Gdiplus::Rect(pDoc->mIcon.mPackmanX, pDoc->mIcon.mPackmanY, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight),
			0, pDoc->mIcon.changeDirection(view->mKey),
			pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);


		view->mAllGrfx->DrawImage(&pDoc->mIcon.mBlinky,
			Gdiplus::Rect(pDoc->mIcon.mBlinkyInfo.pixelX, pDoc->mIcon.mBlinkyInfo.pixelY, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight),
			0, (pDoc->mIcon.mBlinkyInfo.position - 1) * 30,
			pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);

		view->mAllGrfx->DrawImage(&pDoc->mIcon.mInky,
			Gdiplus::Rect(pDoc->mIcon.mInkyInfo.pixelX, pDoc->mIcon.mInkyInfo.pixelY, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight),
			0, (pDoc->mIcon.mInkyInfo.position - 1) * 30,
			pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);

		view->mAllGrfx->DrawImage(&pDoc->mIcon.mPinky,
			Gdiplus::Rect(pDoc->mIcon.mPinkyInfo.pixelX, pDoc->mIcon.mPinkyInfo.pixelY, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight),
			0, (pDoc->mIcon.mPinkyInfo.position - 1) * 30,
			pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);



		view->mAllGrfx->DrawImage(&pDoc->mIcon.mClyde,
			Gdiplus::Rect(pDoc->mIcon.mClydeInfo.pixelX, pDoc->mIcon.mClydeInfo.pixelY, pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight),
			0, (pDoc->mIcon.mClydeInfo.position - 1) * 30,
			pDoc->mIcon.mIconWidth, pDoc->mIcon.mIconHeight, UnitPixel);

		B.SetColor(Color(255, 0, 0));
		stringFormat.SetAlignment(StringAlignmentCenter);
		stringFormat.SetLineAlignment(StringAlignmentCenter);

		RectF rf6(pDoc->mGame.mCx / 2 - 150, pDoc->mGame.mCy / 2 + 27, 300, 50);

		if (pDoc->mGame.mReadyFlag)
			view->mAllGrfx->DrawString(L"READY", -1, &ft, rf6, &stringFormat, &B);
		if (pDoc->mGame.mLife == 0)
		{
			Sleep(1000);
			ResetEvent(gGameEvent);
			ResetEvent(gDrawEvent);
			view->mGamePage = ENDING;
			SetEvent(gEndingEvent);
			AfxBeginThread(endingProc, view);
			return 0;
		}

		grfx.DrawImage(view->mAllBmp, 0, 0);
		Sleep(10);
	}
	return 0;
}

UINT __cdecl endingProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	Graphics grfx(view->m_hWnd);
	Gdiplus::FontFamily fontFamily(L"굴림체");
	Gdiplus::Font ft(&fontFamily, 30, FontStyleRegular, UnitPixel);
	RectF rf(pDoc->mGame.mCx / 2 - 150, pDoc->mGame.mCy / 2 + 150, 300, 100);
	SolidBrush B(Color(255, 255, 255));
	StringFormat stringFormat;
	stringFormat.SetAlignment(StringAlignmentCenter);
	stringFormat.SetLineAlignment(StringAlignmentCenter);

	int i = 0;
	while (true)
	{
		WaitForSingleObject(gEndingEvent, INFINITE);
		view->mAllGrfx->DrawImage(&pDoc->mGame.mEnding, 0, -50, pDoc->mScene.mCx, pDoc->mScene.mCy);
		if (i % 2 == 0)
			view->mAllGrfx->DrawString(L"PRESS ANY KEY", -1, &ft, rf, &stringFormat, &B);
		i++;

		grfx.DrawImage(view->mAllBmp, 0, 0);

		Sleep(300);
	}

	return 0;
}

UINT __cdecl cameraProc(void *p)
{
	CMyPackManView *view = (CMyPackManView *)p;
	CMyPackManDoc *pDoc = view->GetDocument();

	IplImage *image = 0;
	IplImage *av = 0, *sgm = 0;
	IplImage *lower = 0, *upper = 0, *tmp = 0;
	IplImage *dst = 0, *msk = 0;
	IplImage *skin = 0, *mask = 0;
	IplImage *sub_skin = 0;
	CvCapture *capture = 0;
	CBlobLabeling blob;
	CvPoint pt1;
	CvPoint pt2;

	//::WaitForSingleObject(gGameEvent, INFINITE);

	capture = cvCaptureFromCAM(0);
	image = cvQueryFrame(capture);

	av = cvCreateImage(cvGetSize(image), IPL_DEPTH_32F, 3);
	sgm = cvCreateImage(cvGetSize(image), 32, 3);
	lower = cvCreateImage(cvGetSize(image), 32, 3);
	upper = cvCreateImage(cvGetSize(image), 32, 3);
	tmp = cvCreateImage(cvGetSize(image), 32, 3);
	dst = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 3);
	msk = cvCreateImage(cvGetSize(image), 8, 1);
	skin = cvCreateImage(cvGetSize(image), 8, 1);
	mask = cvCreateImage(cvGetSize(image), 8, 3);



	int i, INIT_TIME = 100;

	cvSetZero(av);
	for (i = 0; i<INIT_TIME; i++)
	{
		image = cvQueryFrame(capture);
		cvAcc(image, av);
	}
	cvConvertScale(av, av, 1.0 / INIT_TIME);

	cvSetZero(sgm);
	for (i = 0; i<INIT_TIME; i++)
	{
		image = cvQueryFrame(capture);
		cvConvert(image, tmp);
		cvSub(tmp, av, tmp);
		cvPow(tmp, tmp, 2.0);// 제곱
		cvPow(tmp, tmp, 0.5);//루트
		cvAcc(tmp, sgm);
	}
	cvConvertScale(sgm, sgm, 1.0 / INIT_TIME);
	
	cvNamedWindow("camRemote", CV_WINDOW_AUTOSIZE);

	while (1)
	{
		image = cvQueryFrame(capture);

		double B_PARAM = 1.0 / 30.0;
		double T_PARAM = 1.0 / 100.0;
		double zeta = 10.0;

		cvConvert(image, tmp);

		cvSub(av, sgm, lower);
		cvSubS(lower, cvScalarAll(zeta), lower);
		cvAdd(av, sgm, upper);
		cvAddS(upper, cvScalarAll(zeta), upper);
		cvInRange(tmp, lower, upper, msk);//마스크 영상 만들기

		cvSub(tmp, av, tmp);
		cvPow(tmp, tmp, 2.0);//제곱
		cvPow(tmp, tmp, 0.5);//루트
		cvRunningAvg(image, av, 0.02, msk);
		cvRunningAvg(tmp, sgm, 0.02, msk);

		cvNot(msk, msk);
		cvRunningAvg(tmp, sgm, 0.01, msk);

		cvCopy(msk, skin, 0);
		cvErode(skin, skin, 0, 1);
		cvErode(skin, skin, 0, 1);
		cvDilate(skin, skin, 0, 1);
		cvDilate(skin, skin, 0, 1);
		cvFlip(skin, skin, 1);

		cvCvtColor(skin, mask, 8);

		blob.SetParam(skin, 100);
		blob.DoLabeling();

		for (int i = 0; i < blob.m_nBlobs; i++) {

			blob.BlobSmallSizeConstraint(100, 100);
			blob.BlobBigSizeConstraint(200, 480);
			CvRect hand;
			hand.x = 0; hand.y = 0; hand.width = 0; hand.height = 0;

			hand.x = blob.m_recBlobs[i].x;
			hand.y = blob.m_recBlobs[i].y;
			hand.width = blob.m_recBlobs[i].width;
			hand.height = blob.m_recBlobs[i].height;
			if (hand.width < 170 && hand.height > 190)
				hand.height = 190;

			if (hand.width > 170 && hand.height > 190) {
				hand.width = hand.width;
				hand.height = 190;
			}
			pt1.x = hand.x;
			pt1.y = hand.y;
			pt2.x = hand.x + hand.width;
			pt2.y = hand.y + hand.height;

			if (0 <= hand.y && 30 >= hand.y) {
				if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow - 1][pDoc->mIcon.mPackmanCol] == 0)
					view->mKey = UP;
			}

			if (image->width >= (hand.x + hand.width) && image->width - 30 <= (hand.x + hand.width)) {
				if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol + 1] == 0)
					view->mKey = RITE;
			}
			if (image->height >= (hand.y + hand.height) && image->height - 30 <= (hand.y + hand.height)) {
				if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow + 1][pDoc->mIcon.mPackmanCol] == 0)
					view->mKey = DOWN;
			}
			if (0 <= hand.x && 30 >= hand.x) {
				if (pDoc->mGame.mMapForGhost[pDoc->mIcon.mPackmanRow][pDoc->mIcon.mPackmanCol - 1] == 0)
					view->mKey = LEFT;
			}

			cvDrawRect(mask, pt1, pt2, CV_RGB(255, 0, 0), 2);
		}




		cvShowImage("camRemote", mask);

		if (cvWaitKey(10) == 27)
			break;
	}

	cvReleaseImage(&av);
	cvReleaseImage(&sgm);
	cvReleaseImage(&lower);
	cvReleaseImage(&upper);
	cvReleaseImage(&tmp);
	cvReleaseImage(&dst);
	cvReleaseImage(&msk);
	cvReleaseImage(&skin);

	cvReleaseCapture(&capture);

	return 0;
}

void CALLBACK changeGhost(HWND, UINT, UINT_PTR, DWORD)
{
	static int ghostSec = 1;

	CMainFrame *pMain = (CMainFrame *)theApp.GetMainWnd();
	CMyPackManView *pView = (CMyPackManView *)pMain->GetActiveView();
	CMyPackManDoc *pDoc = (CMyPackManDoc *)pView->GetDocument();

	

	if (pDoc->mIcon.mGhostEat == 1) {
		bckSound.stop();
		huntSound.bckMusic();
		if (ghostSec == 20) {
			huntSound.stop();
			bckSound.SetPosition(0);

			pDoc->mIcon.mGhostEat = 0;
			ghostSec = 0;

			pDoc->mIcon.mBlinkyInfo.hunt = 0;
			pDoc->mIcon.mInkyInfo.hunt = 0;
			pDoc->mIcon.mPinkyInfo.hunt = 0;
			pDoc->mIcon.mClydeInfo.hunt = 0;
			pDoc->mGame.mHuntCount = 0;
			return;
		}

		if (ghostSec > 15 && ghostSec % 2 == 1) {
			
			if (pDoc->mIcon.mBlinkyInfo.hunt == 0)
				pDoc->mIcon.mBlinkyInfo.position = 6;
			
			if (pDoc->mIcon.mInkyInfo.hunt == 0)
				pDoc->mIcon.mInkyInfo.position = 6;
			
			if (pDoc->mIcon.mPinkyInfo.hunt == 0)
				pDoc->mIcon.mPinkyInfo.position = 6;
			
			if (pDoc->mIcon.mClydeInfo.hunt == 0)
				pDoc->mIcon.mClydeInfo.position = 6;
		}
		
		else {
			if (pDoc->mIcon.mBlinkyInfo.hunt == 0)
				pDoc->mIcon.mBlinkyInfo.position = 5;

			if (pDoc->mIcon.mInkyInfo.hunt == 0)
				pDoc->mIcon.mInkyInfo.position = 5;
			
			if (pDoc->mIcon.mPinkyInfo.hunt == 0)
				pDoc->mIcon.mPinkyInfo.position = 5;
			
			if (pDoc->mIcon.mClydeInfo.hunt == 0)
				pDoc->mIcon.mClydeInfo.position = 5;
		}

		ghostSec++;
	}
}

void CALLBACK dropItem(HWND, UINT, UINT_PTR, DWORD)
{
	static int itemDropSec = 1;
	static int itemKeepSec = 1;
	static int itemSec = 1;

	CMainFrame *pMain = (CMainFrame *)theApp.GetMainWnd();
	CMyPackManView *pView = (CMyPackManView *)pMain->GetActiveView();
	CMyPackManDoc *pDoc = (CMyPackManDoc *)pView->GetDocument();


	if (itemSec % ((rand()%10+1)*3) == 0)
		pDoc->mGame.mMapForPacman[12][9] = 7;
	if (itemSec % ((rand()%10+1)*6) == 0)
		pDoc->mGame.mMapForPacman[12][9] = 0;

	itemSec++;
}
