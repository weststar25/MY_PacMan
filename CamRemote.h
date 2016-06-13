#pragma once
#include "opencv/cv.h"
#include "opencv/highgui.h"
using namespace cv;

class CamRemote
{
public:
	IplImage *mImage;
	IplImage *mGray;
	IplImage *mLabeled;
	CvCapture *mCapture;

	CBlobLabeling mBlob;
	CvPoint mPt1;
	CvPoint mPt2;
	CvScalar mColor;

	CamRemote();
	~CamRemote();
	void loadCam();
	void changeBinary();
	void makeLabled();
	void release();
};

