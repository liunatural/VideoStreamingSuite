#include "H265Decoder.h"


H265Decoder::H265Decoder()
{
	decoder = new CFFMDecWraper();
}


H265Decoder::~H265Decoder()
{
	if (decoder)
	{
		delete decoder;
		decoder = 0;
	}
}

void H265Decoder::Init(FnDecodeVideo callback, int width, int height)
{

	if (m_out_buffer.data)
		return;
	OnDecodeVideo = callback;
	m_width = width;
	m_height = height;
	int size = 1024 * 1024 * 16;

	decoder->InitDec(m_width, m_height, AV_CODEC_ID_H265);

	m_out_buffer.data = new char[width * height * 3 * 2];
	m_out_buffer.size = 0;

}

bool H265Decoder::DecodeVideo(VideoPackage* pkt)
{
	VideoPackage in;
	in.data = pkt->data;
	in.size = pkt->size;
	if (!decoder->DecodeVideoEx(&in, &m_out_buffer))
		return false;
	if (OnDecodeVideo) {
		VideoPackage p;
		p.data = m_out_buffer.data;
		p.size = m_width * m_height * 4;
		p.index = 0;
		p.timestamp = pkt->timestamp;
		p.dTimer = pkt->dTimer;
		OnDecodeVideo(&p);
	}
	return true;

}
