#pragma once
#include "AVFastCodecLib.h"
#include "FFMDecWrapper.h"

class H265Decoder: public Decoder
{
public:
	H265Decoder();
	virtual ~H265Decoder();
	void	Init(FnDecodeVideo callback, int width, int height);
	bool	DecodeVideo(VideoPackage* pkt);
	bool	DecodeSliceVideo(videoFrameSlicePkt* pk) { return false; }; //不支持分片解码

private:
	FnDecodeVideo	OnDecodeVideo;
	int						m_width;
	int						m_height;
	int						m_timestamp;

	VideoPackage		m_out_buffer;

	CFFMDecWraper*  decoder;


};

