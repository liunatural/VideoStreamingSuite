#include "xColorSpace.h"

#pragma  warning(disable:4996)

#pragma comment(lib, "avutil.lib")


xColorSpace::xColorSpace()
{

}

xColorSpace::~xColorSpace()
{
}

bool xColorSpace::Init(int SrcWidth, int SrcHeight, int DstPitch)
{
	m_SrcWidth	= SrcWidth;
	m_SrcHeight	= SrcHeight;
	m_DstPitch	= DstPitch;
	m_t2		= m_SrcWidth * m_SrcHeight / 4 * 5;
	m_t1		= m_SrcWidth * m_SrcHeight;
	m_RgbSize	= SrcWidth * SrcHeight * 3;
	m_RGBbuffer	= new BYTE[m_RgbSize];

	return true;
}



bool  xColorSpace::RGB24_YV12(unsigned char* SrcBuffer, unsigned char* DstBuffer)
{
	bool ret = false;

	if (SrcBuffer == NULL || DstBuffer == NULL)
	{
		return  ret;
	}

	AVPicture pFrameBGR, pFrameYUV;

	avpicture_fill(&pFrameBGR, (uint8_t*)SrcBuffer, AV_PIX_FMT_BGR24, m_SrcWidth, m_SrcHeight);
	avpicture_fill(&pFrameYUV, (uint8_t*)DstBuffer, AV_PIX_FMT_YUV420P, m_SrcWidth, m_SrcHeight);

	//U,V»¥»»
	uint8_t * ptmp = pFrameYUV.data[1];
	pFrameYUV.data[1] = pFrameYUV.data[2];
	pFrameYUV.data[2] = ptmp;

	struct SwsContext* imgCtx = NULL;
	imgCtx = sws_getContext(m_SrcWidth, m_SrcHeight, AV_PIX_FMT_BGR24, m_SrcWidth, m_SrcHeight, AV_PIX_FMT_YUV420P, SWS_BICUBIC, 0, 0, 0);

	if (imgCtx != NULL)
	{
		int sliceHeight = sws_scale(imgCtx, pFrameBGR.data, pFrameBGR.linesize, 0, m_SrcHeight, pFrameYUV.data, pFrameYUV.linesize);
	}

	if (imgCtx)
	{
		sws_freeContext(imgCtx);
		imgCtx = NULL;
		ret =  true;
	}

	return ret;

}