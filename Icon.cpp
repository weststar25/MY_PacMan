#include "stdafx.h"
#include "Icon.h"

Icon::Icon()
	: mPackman(L"res/팩맨.png")
	, mBlinky(L"res/Blinky.png")
	, mInky(L"res/Inky.png")
	, mPinky(L"res/Pinky.png")
	, mClyde(L"res/Clyde.png")
	, mItem(L"res/아이템.png")
	, mSmallCandy(L"res/smallCandy.png")
	, mBigCandy(L"res/bigCandy.png")
	, mPackManForLife(L"res/팩맨forLife.png")
	, mIconWidth(30)
	, mIconHeight(30)
	, mPackmanX(574)
	, mPackmanY(518)
	, mPackmanRow(16)
	, mPackmanCol(9)
	, mLx(0), mRx(0), mUy(0), mDy(0)
	, mGhostEat(0), mItemEat(0)
{
	mBlinkyInfo.pixelX = 574;
	mBlinkyInfo.pixelY = 280;
	mBlinkyInfo.mDy = 0;
	mBlinkyInfo.mUy = 0;
	mBlinkyInfo.mRx = 0;
	mBlinkyInfo.mLx = 0;
	mBlinkyInfo.row = 8;
	mBlinkyInfo.col = 9;
	mBlinkyInfo.direction = 1;
	mBlinkyInfo.position = 0;
	mBlinkyInfo.collision = 0;
	mBlinkyInfo.hunt = 0;

	mInkyInfo.pixelX = 545;
	mInkyInfo.pixelY = 340;
	mInkyInfo.mDy = 0;
	mInkyInfo.mUy = 0;
	mInkyInfo.mRx = 0;
	mInkyInfo.mLx = 0;
	mInkyInfo.row = 10;
	mInkyInfo.col = 8;
	mInkyInfo.direction = 1;
	mInkyInfo.position = 0;
	mInkyInfo.collision = 0;
	mInkyInfo.hunt = 0;

	mPinkyInfo.pixelX = 574;
	mPinkyInfo.pixelY = 340;
	mPinkyInfo.mDy = 0;
	mPinkyInfo.mUy = 0;
	mPinkyInfo.mRx = 0;
	mPinkyInfo.mLx = 0;
	mPinkyInfo.row = 10;
	mPinkyInfo.col = 9;
	mPinkyInfo.direction = 1;
	mPinkyInfo.position = 0;
	mPinkyInfo.collision = 0;
	mPinkyInfo.hunt = 0;

	mClydeInfo.pixelX = 603;
	mClydeInfo.pixelY = 340;
	mClydeInfo.mDy = 0;
	mClydeInfo.mUy = 0;
	mClydeInfo.mRx = 0;
	mClydeInfo.mLx = 0;
	mClydeInfo.row = 10;
	mClydeInfo.col = 10;
	mClydeInfo.direction = 1;
	mClydeInfo.position = 0;
	mClydeInfo.collision = 0;
	mClydeInfo.hunt = 0;
}

Icon::~Icon()
{
}

int Icon::changeDirection(int key)
{
	int direction;

	switch(key) {
	case UP:
		if (mPackmanY % 4 == 0)
			direction = 30;
		else
			direction = 0;
		break;
	case RITE:
		if (mPackmanX % 4 == 0)
			direction = 60;
		else
			direction = 0;
		break;
	case LEFT:
		if (mPackmanX % 4 == 0)
			direction = 90;
		else
			direction = 0;
		break;
	case DOWN:
		if (mPackmanY % 4 == 0)
			direction = 120;
		else
			direction = 0;
		break;

	case STOP:
		direction = 0;
		break;
	}
	return direction;
}

void Icon::packmanMove(int key, int nextKey, Game &bg)
{


	switch (key) {
	case UP:
		if ((bg.mMapForPacman[mPackmanRow - 1][mPackmanCol] == 0 || bg.mMapForPacman[mPackmanRow - 1][mPackmanCol] == 3 
			|| bg.mMapForPacman[mPackmanRow - 1][mPackmanCol] == 5) && (mLx <= 5 && mRx <= 5)) {

			mPackmanY += -2;
			mUy += 2;

			if (mDy > 0) {
				mUy -= mDy;
				mDy = 0;
			}
			if (mUy > 29) {
				mUy = 0; // *
				mPackmanRow -= 1;
			}
		}
		break;
	case RITE:
		if (mPackmanRow == 10 && mPackmanCol == 18) {
			mPackmanX = 304;
			mPackmanY = 338;
			mPackmanCol = 0;
		}
		if ((bg.mMapForPacman[mPackmanRow][mPackmanCol + 1] == 0 || bg.mMapForPacman[mPackmanRow][mPackmanCol + 1] == 3
			|| bg.mMapForPacman[mPackmanRow][mPackmanCol + 1] == 5 || bg.mMapForPacman[mPackmanRow][mPackmanCol + 1] == 7) && (mUy <= 5 && mDy <= 5)) {

			mPackmanX += 2;
			mRx += 2;
			
			if (mLx > 0) {
				mRx -= mLx;
				mLx = 0;
			}
			if (mRx > 29) {
				mRx = 0; // *
				mPackmanCol += 1;
			}
		}
		break;
	case LEFT:
		if (mPackmanRow == 10 && mPackmanCol == 0) {
			mPackmanX = 844;
			mPackmanY = 338;
			mLx = 0;
			mPackmanCol = 18;
		}
		if ((bg.mMapForPacman[mPackmanRow][mPackmanCol - 1] == 0 || bg.mMapForPacman[mPackmanRow][mPackmanCol - 1] == 3
			|| bg.mMapForPacman[mPackmanRow][mPackmanCol - 1] == 5 || bg.mMapForPacman[mPackmanRow][mPackmanCol - 1] == 7) && (mUy <= 5 && mDy <= 5)) {

			mPackmanX += -2;
			mLx += 2;

			if (mRx > 0) {
				mLx -= mRx;
				mRx = 0;
			}
			if (mLx > 29) {			
				mLx = 0; // *
				mPackmanCol -= 1;
			}
		}
		break;
	case DOWN:
		if ((bg.mMapForPacman[mPackmanRow + 1][mPackmanCol] == 0 || bg.mMapForPacman[mPackmanRow + 1][mPackmanCol] == 3
			|| bg.mMapForPacman[mPackmanRow + 1][mPackmanCol] == 5) && (mLx <= 5 && mRx <= 5)) {

			mPackmanY += 2;
			mDy += 2;
		
			if (mUy > 0) {
				mDy -= mUy;
				mUy = 0;
			}
			if (mDy > 29) {
				mDy = 0; // *
				mPackmanRow += 1;
			}
		}
		break;
	case STOP:
		mPackmanX = mPackmanX;
		mPackmanY = mPackmanY;
		mPackmanRow = mPackmanRow;
		mPackmanCol = mPackmanCol;
		mLx = mLx;
		mRx = mRx;
		mUy = mUy;
		mDy = mDy;
		break;
	}
}

void Icon::ghostMove(Game &bg, int direction, int prevDirection, int row, int col)
{
	while (1)	{
		// 이전에 움직였던 방향의 반대 방향이면 다시 뽑기
		// 와리가리 방지
		if ((direction + 2) % 4 == prevDirection && direction != LEFT)
			continue;
		else if (direction + 2 == prevDirection)
			continue;

		//선택한 방향에 벽있는가? 없으면 다시 뽑고, 아니면 리턴
		switch (direction)
		{
		case UP:
			if (bg.mMapForGhost[row - 1][col] == 1) {
				break;
			}
			else
				return;
		case DOWN:
			if (bg.mMapForGhost[row + 1][col] == 1) {
				break;
			}
			else
				return;
		case LEFT:
			if (bg.mMapForGhost[row][col - 1] == 1) {
				break;
			}
			else
				return;
		case RITE:
			if (bg.mMapForGhost[row][col + 1] == 1) {
				break;
			}
			else
				return;
		}
	}
}

void Icon::setPosition()
{
	mPackmanX = 574;
	mPackmanY = 518;
	mPackmanRow = 16;
	mPackmanCol = 9;
	mLx = mRx = mUy = mDy = 0;
	mGhostEat = mItemEat = 0;
	
	mBlinkyInfo.pixelX = 574;
	mBlinkyInfo.pixelY = 280;
	mBlinkyInfo.mDy = 0;
	mBlinkyInfo.mUy = 0;
	mBlinkyInfo.mRx = 0;
	mBlinkyInfo.mLx = 0;
	mBlinkyInfo.row = 8;
	mBlinkyInfo.col = 9;
	mBlinkyInfo.direction = 1;
	mBlinkyInfo.position = 0;

	mInkyInfo.pixelX = 545;
	mInkyInfo.pixelY = 340;
	mInkyInfo.mDy = 0;
	mInkyInfo.mUy = 0;
	mInkyInfo.mRx = 0;
	mInkyInfo.mLx = 0;
	mInkyInfo.row = 10;
	mInkyInfo.col = 8;
	mInkyInfo.direction = 1;
	mInkyInfo.position = 0;

	mPinkyInfo.pixelX = 574;
	mPinkyInfo.pixelY = 340;
	mPinkyInfo.mDy = 0;
	mPinkyInfo.mUy = 0;
	mPinkyInfo.mRx = 0;
	mPinkyInfo.mLx = 0;
	mPinkyInfo.row = 10;
	mPinkyInfo.col = 9;
	mPinkyInfo.direction = 1;
	mPinkyInfo.position = 0;

	mClydeInfo.pixelX = 603;
	mClydeInfo.pixelY = 340;
	mClydeInfo.mDy = 0;
	mClydeInfo.mUy = 0;
	mClydeInfo.mRx = 0;
	mClydeInfo.mLx = 0;
	mClydeInfo.row = 10;
	mClydeInfo.col = 10;
	mClydeInfo.direction = 1;
	mClydeInfo.position = 0;
}

void Icon::resetBlinkyPosition()
{
	mBlinkyInfo.pixelX = 574;
	mBlinkyInfo.pixelY = 280;
	mBlinkyInfo.mDy = 0;
	mBlinkyInfo.mUy = 0;
	mBlinkyInfo.mRx = 0;
	mBlinkyInfo.mLx = 0;
	mBlinkyInfo.row = 8;
	mBlinkyInfo.col = 9;
	mBlinkyInfo.direction = 1;
	mBlinkyInfo.position = 0;
}

void Icon::resetInkyPosition()
{
	mInkyInfo.pixelX = 545;
	mInkyInfo.pixelY = 340;
	mInkyInfo.mDy = 0;
	mInkyInfo.mUy = 0;
	mInkyInfo.mRx = 0;
	mInkyInfo.mLx = 0;
	mInkyInfo.row = 10;
	mInkyInfo.col = 8;
	mInkyInfo.direction = 1;
	mInkyInfo.position = 0;
}

void Icon::resetPinkyPosition()
{
	mPinkyInfo.pixelX = 574;
	mPinkyInfo.pixelY = 340;
	mPinkyInfo.mDy = 0;
	mPinkyInfo.mUy = 0;
	mPinkyInfo.mRx = 0;
	mPinkyInfo.mLx = 0;
	mPinkyInfo.row = 10;
	mPinkyInfo.col = 9;
	mPinkyInfo.direction = 1;
	mPinkyInfo.position = 0;
}

void Icon::resetClydePosition()
{
	mClydeInfo.pixelX = 603;
	mClydeInfo.pixelY = 340;
	mClydeInfo.mDy = 0;
	mClydeInfo.mUy = 0;
	mClydeInfo.mRx = 0;
	mClydeInfo.mLx = 0;
	mClydeInfo.row = 10;
	mClydeInfo.col = 10;
	mClydeInfo.direction = 1;
	mClydeInfo.position = 0;
}