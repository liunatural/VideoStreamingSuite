#pragma once
#include <memory>
#include <vector>
#include <AVFastCodecLib.h>
#include "FFMDecWrapper.h"
#include "ImagePartition.h"

struct SliceDecPkt
{
	VideoPackage out_ffPkt;
	CFFMDecWraper* decoder;
};

class H264Decoder : public Decoder
{
public:
	H264Decoder();
	virtual ~H264Decoder();

	void Init(FnDecodeVideo callback, int width, int height);
	bool	DecodeVideo(VideoPackage* pkt) { return false; };//‘›≤ª µœ÷
	bool DecodeSliceVideo(videoFrameSlicePkt* pkt);

private:
	int m_width;
	int m_height;
	int m_timestamp;
	void* m_out_buffer;

	int		m_wBlocks;
	int		m_hBlocks;

	int m_totalBloks;

	std::shared_ptr<CImagePartition> m_ImagePartition;
	std::vector<SliceDecPkt> m_silce;

	FnDecodeVideo OnDecodeVideo;  //callback

};
