#pragma once
#include "AVFastCodecLib.h"
#include "xColorSpace.h"

extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}



class FFMEncWrapper
{
public:
	FFMEncWrapper();
	~FFMEncWrapper();

	bool  init(EncContext context);
	bool  EncodeVideo(VideoPackage in_pkt, bool key, VideoPackage& out_pkt);

	int GetTotalFrameNumber() { return m_i_frame; }

private:

	EncContext		m_context;
	AVFrame				*pFrame;
	AVPacket				pkt;
	AVCodecContext		*m_AVCodecContext;
	uint8_t* picture_buf;
	
	int y_size;
	int picture_size;

	int				m_i_frame;
	xColorSpace *m_xColorSpace;
	void* m_yv12_buffer;

};

