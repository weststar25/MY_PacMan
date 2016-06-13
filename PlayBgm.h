#pragma once
#pragma comment(lib, "winmm.lib")
#include <mmsystem.h>
#include <Digitalv.h>

class PlayBgm
{
public:
	MCI_OPEN_PARMS   mciOpen;
	MCI_PLAY_PARMS   mciPlay;
	MCI_STATUS_PARMS mciStatus;

	LONG m_nTotalTime;

	UINT wDeviceID;

	PlayBgm(LPCTSTR musicName = NULL);
	~PlayBgm();

	void open(LPCTSTR musicName);
	void bckMusic();
	void play();
	void stop();
	void SetPosition(DWORD dwPos);
};

