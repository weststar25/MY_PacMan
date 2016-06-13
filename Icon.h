#pragma once
#include "Game.h"

enum diretion{ STOP, UP, RITE, DOWN, LEFT };

struct GHOST {
	int row, col;
	int pixelX, pixelY;
	int mLx, mRx, mUy, mDy;
	int collision;
	int direction;
	int prevDirection;
	int position;
	int hunt;
};

class Icon
{
public:
	/* ĳ���͵� �׸�*/
	Image mPackman;
	Image mBlinky;
	Image mInky;
	Image mPinky;
	Image mClyde;
	Image mItem;
	Image mSmallCandy;
	Image mBigCandy;
	Image mPackManForLife;

	/* ĳ���͵� ũ�� ���� */
	int mIconWidth, mIconHeight;

	/* �Ѹ� row, col �� */
	int mPackmanRow, mPackmanCol;

	/* �Ѹ� �̵��� ��ǥ ���� */
	int mPackmanX, mPackmanY;
	int mLx, mRx, mUy, mDy;

	/* �ͽŵ� ��ǥ ���� */
	GHOST mBlinkyInfo;
	GHOST mInkyInfo;
	GHOST mPinkyInfo;
	GHOST mClydeInfo;

	int mGhostEat;
	int mItemEat;
	int mCollision;


	Icon();
	~Icon();

	int changeDirection(int key);
	void packmanMove(int key, int nextKey, Game &bg);
	void ghostMove(Game &bg, int direction, int prevDirection, int row, int col);
	void setPosition();
	void resetBlinkyPosition();
	void resetInkyPosition();
	void resetPinkyPosition();
	void resetClydePosition();
};

