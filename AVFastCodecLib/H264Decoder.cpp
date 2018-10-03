#include "H264Decoder.h"

H264Decoder::H264Decoder()
{
	m_out_buffer = 0;
	m_width = 0;
	m_height = 0;
	m_timestamp = 0;

	m_wBlocks = 0;
	m_hBlocks = 0;
	m_totalBloks = 0;

	OnDecodeVideo = 0;
}


H264Decoder::~H264Decoder()
{
	if (m_out_buffer)
		delete[] m_out_buffer;

	std::vector<SliceDecPkt>::iterator itr = m_silce.begin();
      while ( itr != m_silce.end())
     {    
		  SliceDecPkt slicePkt = *itr;
		  delete [] slicePkt.out_ffPkt.data;
		  delete slicePkt.decoder;
		  m_silce.erase(itr);
     }
}


void H264Decoder::Init(FnDecodeVideo callback, int width, int height)
{

	OnDecodeVideo = callback;
	
	m_width = width;
	m_height = height;

	m_wBlocks = 4;
	m_hBlocks = 4;

	m_totalBloks = m_hBlocks * m_wBlocks;

	for (int i = 0; i < m_totalBloks; i++)
	{
		SliceDecPkt slicePkt;
		int size = 1024 * 1024;
		slicePkt.out_ffPkt.data = new char[size * 2];
		slicePkt.out_ffPkt.size = size * 2;
		slicePkt.decoder = new CFFMDecWraper();
		slicePkt.decoder->InitDec(m_width / m_wBlocks, m_height / m_hBlocks, AV_CODEC_ID_H264);
		m_silce.push_back(slicePkt);
	}

	std::shared_ptr<CImagePartition> pImgPt(new CImagePartition);
	m_ImagePartition = pImgPt;
	
	m_out_buffer = new char[width * height * 3 * 2];
}

//------------------------------------------------------------------------------------------------------------------
bool  H264Decoder::DecodeSliceVideo(videoFrameSlicePkt* pkt)
{
	auto slice = &m_silce[pkt->slice_idx];
	memset(slice->out_ffPkt.data, 0, slice->out_ffPkt.size);

	VideoPackage in;
	in.data = pkt->data;
	in.size = pkt->size;
	m_timestamp = pkt->timestamp;
	slice->decoder->DecodeVideoEx(&in, &slice->out_ffPkt);

	if (pkt->slice_idx == m_totalBloks-1)
	{
		std::vector<void*> pBuffers;
		for (int i = 0; i < m_totalBloks; i++)
		{
			pBuffers.push_back(m_silce[i].out_ffPkt.data);
		}
		//最后一个片了，开始合成
		m_ImagePartition->ImageUnite(&pBuffers[0], m_wBlocks, m_hBlocks, m_width/m_wBlocks, m_height/m_hBlocks, m_out_buffer);

		if (OnDecodeVideo) {
			VideoPackage p;
			p.data = m_out_buffer;
			p.size = m_height* m_width * 3;
			p.index = 0;
			p.timestamp = m_timestamp;
			p.dTimer = 0;
			OnDecodeVideo(&p);
		}
	}

	return true;
}



