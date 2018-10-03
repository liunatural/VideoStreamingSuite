#pragma once
extern "C"
{
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
}


#include <windows.h>

class xColorSpace 
{
public:
	xColorSpace();
	~xColorSpace();
	bool  Init(int SrcWidth, int SrcHeight, int DstPitch);
	bool  RGB24_YV12(unsigned char* SrcBuffer, unsigned char* DstBuffer);

	
private:
	int		m_SrcWidth;
	int		m_SrcHeight;
	int		m_DstPitch;
	int		m_t2;
	int		m_t1;
	BYTE*	m_RGBbuffer;
	int		m_RgbSize;

	AVFrame *pFrameYUV;
	AVCodecContext* m_AVCodecContext;
	SwsContext*		m_swsContext;


};
