#include "stdafx.h"
#include "Scene.h"


Scene::Scene()
	: mLogo(L"res/로고.png")
	, mCharacter(L"res/캐릭터.png")
	, mCharacterR(L"res/캐릭터 반전.png")
	, mSbuttonU(L"res/시작버튼U.png")
	, mSbuttonD(L"res/시작버튼D.png")
	, mKeyboardD(L"res/자판D.png")
	, mKeyboardU(L"res/자판U.png")
	, mMotionD(L"res/동작D.png")
	, mMotionU(L"res/동작U.png")
	, mSpeechD(L"res/음성D.png")
	, mSpeechU(L"res/음성U.png")
	, mButtonFlag(OFF)
	, mMidFlag(OFF)
	, mKeyboardFlag(OFF)
	, mMotionFlag(OFF)
	, mSpeechFlag(OFF)
{
}

Scene::~Scene()
{
}
void Scene::fillBckGround(Graphics *grfx, CRect &rc)
{
	SolidBrush blackBrush(Color(0, 0, 0));
	grfx->FillRectangle(&blackBrush, 0, 0, rc.right, rc.bottom);
}
void Scene::initIntroScene(CRect &rc)
{
	mCx = rc.Width();
	mCy = rc.Height();
	mLeft = rc.left;
	mRite = rc.right;

	mLogoLeft = (rc.Width() - mLogo.GetWidth()) / 2;
	mMidLeft = (rc.Width() - mCharacter.GetWidth()) / 2;
	mMidRite = mMidLeft + mCharacter.GetWidth() - 200;
	mMidTop = (rc.Height() - mCharacter.GetHeight()) / 2;
	mButtonLeft = (rc.Width() - mSbuttonD.GetWidth()) / 2;

	int selectOffset = ((mCx - (mKeyboardD.GetWidth() + mMotionD.GetWidth() + mSpeechD.GetWidth())) / 3) - 10;
	mSelectTop = (rc.Height() - mCharacter.GetHeight()) / 3;
	mKeyLeft = selectOffset;
	mMotionLeft = selectOffset * 2 + mKeyboardD.GetWidth();
	mSpeechLeft = mMotionLeft + selectOffset + mMotionD.GetWidth();
}
int Scene::moveMid()
{
	if (mMidLeft >= mRite)
		mMidFlag = 2;
	else if (mMidRite <= mLeft)
		mMidFlag = 1;

	if (mMidFlag == 1 && mMidLeft < mRite) {

		mMidLeft += 5;
		mMidRite += 5;
	}
	if (mMidFlag == 2 && mMidRite > mLeft) {

		mMidLeft -= 5;
		mMidRite -= 5;
	}
	
	return mMidLeft;
}