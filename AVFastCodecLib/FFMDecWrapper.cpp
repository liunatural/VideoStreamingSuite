
#include "FFMDecWrapper.h"

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "swscale.lib")
#pragma comment(lib,"avutil.lib")
#pragma  warning(disable:4996)


CFFMDecWraper::CFFMDecWraper()
{
	m_Buffer = NULL;
	m_to_release = false;
	m_BufferSize = 0;
}


CFFMDecWraper::~CFFMDecWraper()
{

	if (m_Buffer)
	{
		delete[] m_Buffer;
		m_Buffer = NULL;
	}
	if (m_to_release)
	{
		sws_freeContext(m_swsContext);
	}
}

//------------------------------------------------------------------------------------------------------------------
bool __stdcall CFFMDecWraper::InitDec(int width, int height, int codec_id)
{
	
	av_log_set_level(AV_LOG_DEBUG);
	m_RGBDataSize = width * (height) * 4;
	m_BufferSize = width * (height + 15) * 4;

	m_Buffer = new char[m_BufferSize];

	av_register_all();
	avcodec_register_all();
	//---------------------------------------------------------------------
	AVCodec* avcodec = avcodec_find_decoder((AVCodecID)codec_id);// AV_CODEC_ID_H264 //AV_CODEC_ID_H265
	m_AVCodecContext = avcodec_alloc_context3(avcodec);

	m_AVCodecContext->width = width;
	m_AVCodecContext->height = height;

	m_AVCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
	m_AVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	m_AVCodecContext->time_base.num = 1;
	m_AVCodecContext->time_base.den = 30;
	m_AVCodecContext->framerate.num = 30;
	m_AVCodecContext->framerate.den = 1;
	m_AVCodecContext->color_range = AVCOL_RANGE_MPEG;


	AVDictionary *pDic = 0;
	if (m_AVCodecContext->codec_id == AV_CODEC_ID_H265)
	{
		av_dict_set(&pDic, "x265-params", "qp=20", 0);
		av_dict_set(&pDic, "preset", "ultrafast", 0);
		av_dict_set(&pDic, "tune", "zero-latency", 0);
	}

	if (avcodec_open2(m_AVCodecContext, avcodec, &pDic) < 0)
		return false;

	//返回从YUV色彩空间转换成RGB色彩空间的上下文
	m_swsContext = sws_getContext(m_AVCodecContext->width, m_AVCodecContext->height, AV_PIX_FMT_YUV420P,
		m_AVCodecContext->width, m_AVCodecContext->height, AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

	m_to_release = true;

	m_dec_picture = av_frame_alloc();

	return true;
}

bool __stdcall CFFMDecWraper::DecodeVideo(VideoPackage* pkt)
{
	int size = 0;
	__int64 ts = 0;

	memset(m_Buffer, 0, m_BufferSize);
	decoder_frame(pkt->data, pkt->size, m_Buffer, &size, &ts);

	VideoPackage dec_pkt = { 0 };
	dec_pkt.data = m_Buffer;

	return true;
}

bool __stdcall CFFMDecWraper::DecodeAudio(VideoPackage* pkt)
{
	return true;
}

bool __stdcall CFFMDecWraper::DecodeVideoEx(VideoPackage* pkt, VideoPackage* out_pkt)
{
	int size = 0;
	__int64 ts = 0;

	memset(m_Buffer, 0, m_BufferSize);

	if (0 != decoder_frame(pkt->data, pkt->size, m_Buffer, &size, &ts))
		return false;
	
	memcpy(out_pkt->data, m_Buffer, m_RGBDataSize);

	return true;
}

int CFFMDecWraper::decoder_frame(void* data, int size, char* out_buffer, int* out_size, __int64* time_stamp)
{
	int	got_picture_ptr = 0;

	AVPacket avpkt;
	av_init_packet(&avpkt);
	avpkt.data = (uint8_t*)data;
	avpkt.size = size;

	int ret = avcodec_decode_video2(m_AVCodecContext, m_dec_picture, &got_picture_ptr, &avpkt);

	if (ret < 0)
	{
		av_log(NULL, AV_LOG_ERROR, "avcodec_decode_video2 error!\n");
		return ret;
	}

	if (!got_picture_ptr)
		return ret;

	int out_stride[4] = { 0 };
	out_stride[0] = m_AVCodecContext->width * 3;
	
	uint8_t* dst[4];
	dst[0] = (uint8_t*)out_buffer;

#ifndef ANDROID
	sws_scale(m_swsContext, m_dec_picture->data, m_dec_picture->linesize, 0, m_AVCodecContext->height, dst, out_stride);
#else
	int h264_width = m_AVCodecContext->width;
	int h264_height = m_AVCodecContext->height;
	//int i,j,k ;
	for (int i = 0; i < h264_height; i++)
		memcpy(out_buffer + h264_width*i, m_dec_picture.data[0] + m_dec_picture.linesize[0] * i, h264_width);
	//LOGD("%s-->%d\n",__FUNCTION__,__LINE__);
	for (int j = 0; j < h264_height / 2; j++)
		memcpy(out_buffer + h264_width*h264_height + h264_width / 2 * j, m_dec_picture.data[1] + m_dec_picture.linesize[1] * j, h264_width / 2);
	for (int k = 0; k < h264_height / 2; k++)
		memcpy(out_buffer + h264_width*h264_height + h264_width / 2 * (h264_height / 2) + h264_width / 2 * k, m_dec_picture.data[2] + m_dec_picture.linesize[2] * k, h264_width / 2);
#endif

	*time_stamp = m_dec_picture->pts;
	return 0;
}
