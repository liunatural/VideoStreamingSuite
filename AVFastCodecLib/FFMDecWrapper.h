#pragma once
#include "AVFastCodecLib.h"


extern "C"
{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include <libavutil/avutil.h>
}

//-------------------------------------------------

class CFFMDecWraper 
{
public:
	CFFMDecWraper();
	virtual ~CFFMDecWraper();

	bool  InitDec(int width, int height, int codec_id);
	bool  DecodeVideo(VideoPackage* pkt);
	bool  DecodeAudio(VideoPackage* pkt);
	bool  DecodeVideoEx(VideoPackage* pkt, VideoPackage* out_pkt);

private:
	int decoder_frame(void* data, int size, char* out_buffer, int* out_size, __int64* time_stamp);

private:

	int							m_BufferSize;
	int							m_RGBDataSize;
	bool						m_to_release;

	AVFrame				*m_dec_picture;
	AVCodecContext		*m_AVCodecContext;
	SwsContext			*m_swsContext;
	char						*m_Buffer;
};

