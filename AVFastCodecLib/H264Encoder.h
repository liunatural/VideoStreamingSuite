#include "x264EncWrapper.h"
#include "ImagePartition.h"
#include <vector>

struct SileceInfo
{
	VideoPackage in_pkt;
	bool in_key_frame;
	x264EncWrapper* _x264;
	encode_slice_info info;
};


struct ImageSliceEncoder
{
	ImageSliceEncoder()
	{
		memset(&in_pkt, 0, sizeof(VideoPackage));
		in_key_frame = false;
		_x264 = 0;
	}

	~ImageSliceEncoder()
	{
		if (_x264)
			delete _x264;
	}

	VideoPackage		in_pkt;
	bool		in_key_frame;
	x264EncWrapper*	_x264;

};



class H264Encoder: public Encoder
{
public:
	H264Encoder();
	~H264Encoder();
	bool		Init(FnEncodeVideo callback, int width, int height, double fps, int bit_rate);
	bool		EncodeVideo(void* pBuffer, int len, bool key, int timestamp, double dTimer);

private:
	FnEncodeVideo	OnEncodeVideo;
	int						m_oring_width;
	int						m_oring_height;
	int	m_wBlocks;
	int	m_hBlocks;

	int	m_index = 0;
	int	m_codec_count;
	volatile int m_codec_x;

	bool m_exit = false;

	ImageSliceEncoder		m_info;

	std::vector<SileceInfo>	m_encoder_info;
	CImagePartition* m_ImagePartition;
};

