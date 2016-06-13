#pragma once

enum Cell{ CELLSIZE = 30, ROW = 22, COL = 19 };


struct CHARACTER	{ //각 캐릭터의 위치와 색상값 정보를 가지는 구조체 
	int row;
	int col;
	int direction;
};

class Game
{
public:
	Bitmap mMap;
	Image mEnding;

	short (&mMapForPacman)[22][19];
	short (&mMapForGhost)[22][19];

	Gdiplus::Rect mMapArea;
	Gdiplus::Rect mCharArea;

	int mCx, mCy;
	int mRound;
	int mScore;
	int mLife;
	int mHuntCount;
	int mDotCnt;

	bool mReadyFlag;

	CPoint mLeftTop; // 맵 좌상단 좌표(전체 화면 크기와 그림사이즈필요)

	
	Game();
	~Game();

	void initGame(CRect & rc);
	void initMap();
};

