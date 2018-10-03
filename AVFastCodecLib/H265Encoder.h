#pragma once
#include "AVFastCodecLib.h"
#include "FFMEncWrapper.h"


class H265Encoder: public Encoder
{
public:
	H265Encoder();
	virtual ~H265Encoder();

	bool Init(FnEncodeVideo callback, int width, int height, double fps, int bit_rate);
	bool EncodeVideo(void* pBuffer, int len, bool key, int timestamp, double dTimer);

	FnEncodeVideo OnEncodeVideo;

private:
	int							m_oring_width;
	int							m_oring_height;
	VideoPackage					in_pkt;
	uint8_t					*pOutBuf;
	FFMEncWrapper		*m_ffm265;
};

