#pragma once

enum Cell{ CELLSIZE = 30, ROW = 22, COL = 19 };


struct CHARACTER	{ //�� ĳ������ ��ġ�� ���� ������ ������ ����ü 
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

	CPoint mLeftTop; // �� �»�� ��ǥ(��ü ȭ�� ũ��� �׸��������ʿ�)

	
	Game();
	~Game();

	void initGame(CRect & rc);
	void initMap();
};

