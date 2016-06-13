#pragma once

enum buttonState{ OFF = 1, ON };

class Scene
{
public:
	/* 그림 저장 객체들*/
	Image mLogo;
	Image mCharacter;
	Image mCharacterR;
	Image mSbuttonD;
	Image mSbuttonU;
	Image mKeyboardD;
	Image mKeyboardU;
	Image mMotionD;
	Image mMotionU;
	Image mSpeechD;
	Image mSpeechU;

	/* 그림들의 좌표값 저장변수 */
	int mLogoLeft;
	int mMidLeft, mMidRite, mMidTop, mMidBottom;
	int mButtonLeft, mButtonTop;
	int mKeyLeft, mMotionLeft, mSpeechLeft;
	int mSelectTop;
	
	/* 그림 변환용 변수 */
	int mButtonFlag;
	int mMidFlag;
	int mKeyboardFlag;
	int mMotionFlag;
	int mSpeechFlag;

	/* Window 너비, 높이, 좌, 우 */
	int mCx, mCy;
	int mLeft, mRite;

	Scene();
	~Scene();
	
	void fillBckGround(Graphics *grfx, CRect &rc);
	void initIntroScene(CRect &rc);
	int  moveMid();
};

