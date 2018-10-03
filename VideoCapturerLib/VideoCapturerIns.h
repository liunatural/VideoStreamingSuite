#pragma once
#include "VideoCapturerLib.h"
#include "ImageManagerIns.h"

class VideoCapturerIns : public VideoCapturer
{
public:
	VideoCapturerIns();
	~VideoCapturerIns();

	virtual	ImageManager*	OpenCamera(int imageWidth, int imageHeight, int deviceId = 0);

private:
	ImageManagerIns* imgMgr;

};

