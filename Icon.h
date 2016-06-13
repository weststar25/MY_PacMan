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
	/* 캐릭터들 그림*/
	Image mPackman;
	Image mBlinky;
	Image mInky;
	Image mPinky;
	Image mClyde;
	Image mItem;
	Image mSmallCandy;
	Image mBigCandy;
	Image mPackManForLife;

	/* 캐릭터들 크기 변수 */
	int mIconWidth, mIconHeight;

	/* 팩맨 row, col 값 */
	int mPackmanRow, mPackmanCol;

	/* 팩맨 이동용 좌표 변수 */
	int mPackmanX, mPackmanY;
	int mLx, mRx, mUy, mDy;

	/* 귀신들 좌표 정보 */
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

