#include "stdafx.h"
#include "H265Encoder.h"


H265Encoder::H265Encoder()
{
	m_ffm265 = new FFMEncWrapper();
}


H265Encoder::~H265Encoder()
{
	if (m_ffm265)
	{
		delete m_ffm265;
		m_ffm265 = NULL;
	}

}

bool H265Encoder::Init(FnEncodeVideo callback, int width, int height, double fps, int bit_rate)
{
	OnEncodeVideo = callback;
	m_oring_width = width;
	m_oring_height = height;


	EncContext ctx;
	memset(&ctx, 0, sizeof(EncContext));
	ctx.bit_rate = bit_rate;
	ctx.width = m_oring_width;
	ctx.height = m_oring_height;
	ctx.fps = fps;
	ctx.codecs = 6;
	ctx.threads = 6;
	m_ffm265->init(ctx);


	pOutBuf = new uint8_t[m_oring_width*m_oring_height * 4];


	return true;
}

bool H265Encoder::EncodeVideo(void * pBuffer, int len, bool key, int timestamp, double dTimer)
{
	in_pkt.data = pBuffer;
	in_pkt.size = len;
	
	memset(pOutBuf, 0, m_oring_width*m_oring_height * 4);
	VideoPackage out;
	out.data = pOutBuf;

	if (!m_ffm265->EncodeVideo(in_pkt, key, out))
	{
		return false;
	}
	VideoPackage out2;
	out2.size = out.size;
	out2.data = out.data;
	out2.timestamp = timestamp;
	out2.index = m_ffm265->GetTotalFrameNumber();
	out2.dTimer = dTimer;
	if (OnEncodeVideo)
		(*OnEncodeVideo)(&out2);


	return true;
}
