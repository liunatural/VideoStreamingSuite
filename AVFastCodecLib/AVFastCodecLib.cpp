#include "AVFastCodecLib.h"
#include "H264Decoder.h"
#include "H264Encoder.h"
#include "H265Encoder.h"
#include "H265Decoder.h"



class AVFastCodecServiceImpl : public AVFastCodecService
{
	
	Encoder* CreateH264Encoder(FnEncodeVideo callback, int width, int height, double fps, int bit_rate)
	{
		H264Encoder* enc = new H264Encoder();
		if (!enc->Init(callback, width, height, fps, bit_rate))
		{
			delete enc;
			return 0;
		}
		return enc;

	}
	Encoder* CreateH265Encoder(FnEncodeVideo callback, int width, int height, double fps, int bit_rate)
	{

		H265Encoder* enc = new H265Encoder();
		if (!enc->Init(callback, width, height, fps, bit_rate))
		{
			delete enc;
			return 0;
		}
		return enc;
	}
	

	Decoder* CreateH264Decoder(FnDecodeVideo callback, int width, int height)
	{
		H264Decoder* decoder = new H264Decoder();
		decoder->Init(callback, width, height);
		return decoder;
	}

	Decoder* CreateH265Decoder(FnDecodeVideo callback, int width, int height)
	{

		H265Decoder* decoder = new H265Decoder();
		decoder->Init(callback, width, height);
		return decoder;
	}
};


FASTCODEC_API AVFastCodecService* CreateAVFastCodecService()
{
	return new AVFastCodecServiceImpl();
}
