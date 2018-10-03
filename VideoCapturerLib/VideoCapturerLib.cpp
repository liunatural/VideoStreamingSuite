#include "VideoCapturerIns.h"

VIDEO_CAPTURER_API VideoCapturer*  CreateVideoCaptureService()
{
	return new VideoCapturerIns();
}
