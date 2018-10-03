#include "stdafx.h"
#include "FFMEncWrapper.h"

#pragma warning(disable:4996)


FFMEncWrapper::FFMEncWrapper()
{
	m_xColorSpace = new xColorSpace();

}


FFMEncWrapper::~FFMEncWrapper()
{
	if (m_xColorSpace)
	{
		delete m_xColorSpace;
		m_xColorSpace = NULL;
	}
}

bool FFMEncWrapper::init(EncContext context)
{
	av_register_all();
	avcodec_register_all();

	AVCodec* avcodec = avcodec_find_encoder(AV_CODEC_ID_H265);  //((AVCodecID)codec_id);// AV_CODEC_ID_H264 //AV_CODEC_ID_H265
	m_AVCodecContext = avcodec_alloc_context3(avcodec);
	
	m_AVCodecContext->bit_rate = context.bit_rate;
	m_AVCodecContext->width = context.width;
	m_AVCodecContext->height = context.height;
	m_AVCodecContext->time_base.num = 1;
	m_AVCodecContext->time_base.den = context.fps;
	m_AVCodecContext->gop_size = 15;
	m_AVCodecContext->max_b_frames = 0;
	m_AVCodecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	m_AVCodecContext->keyint_min = 25;
	//m_AVCodecContext->qmin = 10;
	//m_AVCodecContext->qmax = 51;

	AVDictionary *pDic = 0;
	if (m_AVCodecContext->codec_id == AV_CODEC_ID_H265)
	{
		av_dict_set(&pDic, "x265-params", "qp=20", 0);
		av_dict_set(&pDic, "preset", "ultrafast", 0);
		av_dict_set(&pDic, "tune", "zero-latency", 0);

	}

	if (avcodec_open2(m_AVCodecContext, avcodec, &pDic) < 0)
		return false;

	pFrame = av_frame_alloc();
	picture_size = avpicture_get_size(m_AVCodecContext->pix_fmt, m_AVCodecContext->width, m_AVCodecContext->height);
	picture_buf = (uint8_t *)av_malloc(picture_size);
	avpicture_fill((AVPicture *)pFrame, picture_buf, m_AVCodecContext->pix_fmt, m_AVCodecContext->width, m_AVCodecContext->height);

	y_size = m_AVCodecContext->width * m_AVCodecContext->height;

	m_xColorSpace->Init(m_AVCodecContext->width, m_AVCodecContext->height, m_AVCodecContext->width);



	av_new_packet(&pkt, picture_size);

	return true;

}

bool FFMEncWrapper::EncodeVideo(VideoPackage in_pkt, bool key, VideoPackage& out_pkt)
{

	bool ret = m_xColorSpace->RGB24_YV12((unsigned char*)in_pkt.data, (unsigned char*)picture_buf);
	if (ret == false)
	{
		return ret;
	}


	BYTE* p = (BYTE*)picture_buf;

	pFrame->data[0] = p;						// Y
	pFrame->data[1] = p + y_size;			// U 
	pFrame->data[2] = p + y_size * 5/4;  // V


	
	pFrame->pts = (int64_t)m_i_frame;// * m_AVCodecContext->fpsDenom;
	pFrame->pict_type = AV_PICTURE_TYPE_I;

	int got_picture = 0;
	//Encode
	int retVal = avcodec_encode_video2(m_AVCodecContext, &pkt, pFrame, &got_picture);
	if (retVal < 0) {
		printf("Failed to encode! \n");
		return  false;
	}

	if (got_picture == 1) {
		printf("Succeed to encode frame: %5d\tsize:%5d\n", m_i_frame, pkt.size);
		m_i_frame++; 

		memcpy(out_pkt.data, pkt.data, pkt.size);
		out_pkt.size = pkt.size;
		//pkt.stream_index = video_st->index;
		//ret = av_write_frame(pFormatCtx, &pkt);
		av_packet_unref(&pkt);

		
	}


	return true;


}
