#include "stdafx.h"
#include "CamRemote.h"


CamRemote::CamRemote()
{
}


CamRemote::~CamRemote()
{
}

void CamRemote::loadCam()
{
	cvGrabFrame(mCapture);
	mImage = cvRetrieveFrame(mCapture);
}

void CamRemote::changeBinary()
{
	mGray = cvCreateImage(cvGetSize(mImage), 8, 1);
	cvCvtColor(mImage, mGray, CV_RGB2GRAY);
	cvThreshold(mGray, mGray, 127.0, 255.0, CV_THRESH_BINARY | CV_THRESH_OTSU);
	cvFlip(mGray, mGray, 1);
}
void CamRemote::makeLabled()
{
	cvThreshold(mGray, mGray, 1.0, 255.0, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	mLabeled = cvCreateImage(cvGetSize(mImage), 8, 3);
	cvCvtColor(mGray, mLabeled, CV_GRAY2BGR);

	mBlob.SetParam(mGray, 100);
	mBlob.DoLabeling();

	int nMaxWidth = mGray->width * 7 / 10;
	int nMaxHeight = mGray->height * 7 / 10;
	mBlob.BlobSmallSizeConstraint(100, 100);
	mBlob.BlobBigSizeConstraint(350, 480);

	for (int i = 0; i < mBlob.m_nBlobs; i++) {
		mPt1 = cvPoint(mBlob.m_recBlobs[i].x, mBlob.m_recBlobs[i].y);
		mPt2 = cvPoint(mPt1.x + mBlob.m_recBlobs[i].width, mPt1.y + mBlob.m_recBlobs[i].height);
		mColor = cvScalar(0, 0, 255);
		cvDrawRect(mLabeled, mPt1, mPt2, mColor, 2);
	}
}

void CamRemote::release()
{
	cvReleaseImage(&mGray);
	cvReleaseImage(&mLabeled);
}