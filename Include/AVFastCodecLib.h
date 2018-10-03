#pragma once
#include <string.h>
#include <functional>

#ifdef FASTCODEC_EXPORTS
#define FASTCODEC_API __declspec(dllexport)
#else
#define FASTCODEC_API __declspec(dllimport)
#endif

#pragma pack(push, 1)
struct VideoPackage
{
	void				*data;
	int					size;
	unsigned int	index;
	double			dTimer;
	int					timestamp;
};

struct videoFrameSlicePkt
{
	void			*data;
	int				size;
	short			slice_count;
	short			slice_idx;
	int				timestamp;//ms
	
	int to_buffer(void* buffer)
	{
		auto pkt = (videoFrameSlicePkt*)buffer;
		pkt->timestamp = this->timestamp;
		pkt->size = this->size;
		pkt->slice_count = this->slice_count;
		pkt->slice_idx = this->slice_idx;
		memcpy(&pkt->data, this->data, this->size);
		return sizeof(videoFrameSlicePkt) + size;
	}

	void from_buffer(void* buffer)
	{
		auto res = (videoFrameSlicePkt*)buffer;
		*this = *res;
		data = &res->data;
	}
};

#pragma pack(pop)

struct encode_slice_info
{
	std::uint8_t	index;	//分片索引
	std::uint8_t	count;	//总分片数量
};

struct EncContext
{
	int width;
	int height;
	double fps;
	int threads;				//编码时使用的线程数
	int codecs;					//编码时使用的编码器数量(编码器实例)
	int bit_rate;				//单位kbps
	int max_bit_rate;		//单位kbps
	int i_qp_constant;		//画面质量
	int min_key_frame;	//最小关键帧
	int max_key_frame;	//最大关键帧

	bool limit_i_size;		//限制I帧大小
};

typedef void(__stdcall* FnEncodeVideo)(VideoPackage* pkt);
typedef void(__stdcall*FnDecodeVideo)(VideoPackage* pkt);

class Encoder
{
public:
	virtual bool EncodeVideo(void* pBuffer, int len, bool key, int timestamp, double dTimer) = 0;
};


class Decoder
{
public:
	virtual bool DecodeSliceVideo(videoFrameSlicePkt* pkt) = 0;
	virtual bool DecodeVideo(VideoPackage* pkt) = 0;
};


class AVFastCodecService
{
public:
	virtual Encoder* CreateH264Encoder(FnEncodeVideo callback, int width, int height, double fps, int bit_rate) = 0;
	virtual Encoder* CreateH265Encoder(FnEncodeVideo callback, int width, int height, double fps, int bit_rate) = 0;
	virtual Decoder* CreateH264Decoder(FnDecodeVideo callback, int width, int height) = 0;
	virtual Decoder* CreateH265Decoder(FnDecodeVideo callback, int width, int height) = 0;
};


FASTCODEC_API AVFastCodecService*  CreateAVFastCodecService();