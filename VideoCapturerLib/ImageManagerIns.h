#pragma once
#include "VideoCapturerLib.h"

#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>// Image processing methods for the CPU
#include <opencv2/imgcodecs.hpp>// Read images

// CUDA structures and methods
#include <opencv2/cudaarithm.hpp>
#include <opencv2/cudafilters.hpp>

class ImageManagerIns :	public ImageManager
{
public:
	ImageManagerIns();
	virtual ~ImageManagerIns();

	virtual	bool	OpenCamera(int imageWidth, int imageHeight, int deviceId);
	//ªÒ»°œÒÀÿ÷µ
	virtual	bool	GetPixels(void* result, int width, int height);
	virtual	bool	UpdateFromCamera(ImageManager* camera);
	virtual	bool	MakeGreenMatting();
	virtual	float2	ImageSize();
	virtual	float2	VideoImageSize();

private:
	void	Destroy();
	cv::Mat replace_and_blend(cv::Mat &frame, cv::Mat &mask);
	bool MakeGreenMatting(cv::Mat &source, cv::Mat &result);
private:
	cv::VideoCapture*				m_capture;
	cv::Mat								m_image;
	int									m_videoWidth, m_videoHeight;

};

