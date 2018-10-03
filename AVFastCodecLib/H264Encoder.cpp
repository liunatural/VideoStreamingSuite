#include "H264Encoder.h"

H264Encoder::H264Encoder()
{
	OnEncodeVideo = 0;
	m_oring_width = 0;
	m_oring_height = 0;
	memset(&m_info, 0, sizeof(m_info));
}

H264Encoder::~H264Encoder()
{
}

bool H264Encoder::Init(FnEncodeVideo callback, int width, int height, double fps, int bit_rate)
{
	OnEncodeVideo = callback;
	m_oring_width = width;
	m_oring_height = height;
	m_ImagePartition = new CImagePartition();
	m_wBlocks = 4;
	m_hBlocks = 4;
	m_codec_count = m_hBlocks * m_wBlocks;

	for (int i = 0; i < m_codec_count; i++)
	{
		SileceInfo info;
		int size = 1024 * 1024 * 4;
		info.in_pkt.data =  new char[size];
		info.in_pkt.size = size;

		info.info.count = m_codec_count;
		info.info.index = i;
		info._x264 = new x264EncWrapper();

		EncContext ctx;
		memset(&ctx, 0, sizeof(EncContext));
		ctx.bit_rate = bit_rate;// m_codec_count;
		ctx.width = m_oring_width / m_wBlocks;
		ctx.height = m_oring_height / m_hBlocks;
		ctx.fps = fps;
		//ctx.max_bit_rate = bit_rate;
		ctx.codecs = 6;
		ctx.threads = 6;
		info._x264->InitEnc(ctx);

		m_encoder_info.push_back(info);
	}
	return true;
}

bool H264Encoder::EncodeVideo(void* pBuffer, int len, bool key, int timestamp, double dTimer)
{
	
	std::vector<void*> out_buffer;
	for (int i = 0; i < m_codec_count; i++)
	{
		SileceInfo* ti = &m_encoder_info[i];
		ti->in_key_frame = key;
		out_buffer.push_back(ti->in_pkt.data);
	}

	bool r = m_ImagePartition->Partition(pBuffer, m_oring_width, m_oring_height, &out_buffer[0], m_wBlocks, m_hBlocks);
	if (false == r)
	{
		return false;
	}

	for (int i = 0; i < m_codec_count; i++)
	{
		SileceInfo* info = &m_encoder_info[i];
		info->in_key_frame = key;

		VideoPackage out;
		if (!info->_x264->EncodeVideo(info->in_pkt, key, out))
		{
			return false;
		}

		if (OnEncodeVideo)
		{
			VideoPackage out2;
			out2.size = out.size;
			out2.data = out.data;
			out2.timestamp = timestamp;
			out2.index = i;
			out2.dTimer = dTimer;
			(*OnEncodeVideo)(&out2);
		}
	}

	return true;
}



