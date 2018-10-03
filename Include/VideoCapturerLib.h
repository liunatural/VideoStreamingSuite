#pragma once
#ifdef VIDEO_CAPTURER_EXPORTS
#define VIDEO_CAPTURER_API __declspec(dllexport)
#else
#define VIDEO_CAPTURER_API __declspec(dllimport)
#endif

class ImageManager
{
public:
	ImageManager() {};
	virtual ~ImageManager() {};

	struct float2
	{
		float X, Y;
		float2(float _x, float _y) { X = _x; Y = _y; }
		float2() { X = Y = 0; }
	};

	//��ȡ����ֵ
	virtual	bool	OpenCamera(int imageWidth, int imageHeight, int deviceId) = 0;
	virtual	bool	GetPixels(void* result, int width, int height) = 0;
	virtual	bool	UpdateFromCamera(ImageManager* camera) = 0;
	virtual	bool  MakeGreenMatting() = 0;
	virtual	float2	ImageSize() = 0;
	virtual	float2	VideoImageSize() = 0;

};

class VideoCapturer
{

public:
	VideoCapturer() {};
	virtual ~VideoCapturer() {};

	//��һ������ͷ
	virtual	ImageManager*	OpenCamera(int imageWidth, int imageHeight, int deviceId = 0) = 0;
};


VIDEO_CAPTURER_API VideoCapturer*  CreateVideoCaptureService();