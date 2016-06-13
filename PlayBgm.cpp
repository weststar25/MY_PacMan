#include "stdafx.h"
#include "PlayBgm.h"


PlayBgm::PlayBgm(LPCTSTR musicName)
	: wDeviceID(0)
{
	open(musicName);
}


PlayBgm::~PlayBgm()
{}

void PlayBgm::open(LPCTSTR musicName)
{
	mciOpen.lpstrElementName = musicName;
	mciOpen.lpstrDeviceType = L"MPEGVideo";

	mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_OPEN_TYPE,
		(DWORD)(LPVOID)&mciOpen);

	wDeviceID = mciOpen.wDeviceID;

	MCI_STATUS_PARMS mciStatusParms;
	mciStatusParms.dwItem = MCI_STATUS_LENGTH;

	mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms);
	m_nTotalTime = (LONG)mciStatusParms.dwReturn;

	return;
}

void PlayBgm::bckMusic()
{
	mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY | MCI_DGV_PLAY_REPEAT, (DWORD)(LPVOID)&mciPlay);
	return;
}

void PlayBgm::play()
{
	mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&mciPlay);
	return;
}

void PlayBgm::stop()
{
	mciSendCommand(wDeviceID, MCI_CLOSE, 0, NULL);
	return;
}

void PlayBgm::SetPosition(DWORD dwPos)
{
	MCI_SEEK_PARMS mciSeekParms;

	mciSeekParms.dwTo = dwPos;

	mciSendCommand(wDeviceID, MCI_SEEK, MCI_TO, (DWORD)(LPVOID)&mciSeekParms);

	play();
}