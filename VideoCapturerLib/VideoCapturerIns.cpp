#include "VideoCapturerIns.h"
#include "ImageManagerIns.h"

VideoCapturerIns::VideoCapturerIns()
{
	imgMgr = 0;
}


VideoCapturerIns::~VideoCapturerIns()
{

	if (imgMgr)
	{
		delete imgMgr;
		imgMgr = 0;
	}
}


ImageManager* VideoCapturerIns::OpenCamera(int imageWidth, int imageHeight, int deviceId /*= 0*/)
{
	imgMgr = new ImageManagerIns();
	
	if (!imgMgr->OpenCamera(imageWidth, imageHeight, deviceId))
	{
		delete imgMgr;
		return 0;
	}

	return imgMgr;

}

