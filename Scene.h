#pragma once

enum buttonState{ OFF = 1, ON };

class Scene
{
public:
	/* �׸� ���� ��ü��*/
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

	/* �׸����� ��ǥ�� ���庯�� */
	int mLogoLeft;
	int mMidLeft, mMidRite, mMidTop, mMidBottom;
	int mButtonLeft, mButtonTop;
	int mKeyLeft, mMotionLeft, mSpeechLeft;
	int mSelectTop;
	
	/* �׸� ��ȯ�� ���� */
	int mButtonFlag;
	int mMidFlag;
	int mKeyboardFlag;
	int mMotionFlag;
	int mSpeechFlag;

	/* Window �ʺ�, ����, ��, �� */
	int mCx, mCy;
	int mLeft, mRite;

	Scene();
	~Scene();
	
	void fillBckGround(Graphics *grfx, CRect &rc);
	void initIntroScene(CRect &rc);
	int  moveMid();
};

