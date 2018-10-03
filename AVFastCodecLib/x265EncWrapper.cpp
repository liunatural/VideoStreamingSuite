
#include "x265EncWrapper.h"


x265EncWrapper::x265EncWrapper()
{
	//m_pHandle = NULL;
	//m_pic = NULL;
	//m_param = NULL;
	//m_xColorSpace = new xColorSpace();

}


x265EncWrapper::~x265EncWrapper()
{
	//if (m_pHandle) {x265_encoder_close(m_pHandle); m_pHandle = NULL;}
	//if (m_pic) {x265_picture_free(m_pic); m_pic = NULL; }
	//if (m_param) { x265_param_free(m_param);  m_param = NULL; }
	//if (m_xColorSpace) {	delete m_xColorSpace; m_xColorSpace = NULL;}
}

bool x265EncWrapper::init(EncContext context)
{
	//m_context = context;

	//m_param = x265_param_alloc();
	//x265_param_default(m_param);
	//x265_param_default_preset(m_param, "ultrafast", "zerolatency");
	//x265_param_parse(m_param, "bframes", "0");
	//x265_param_parse(m_param, "rc-lookahead", "0");

	////m_param->frameNumThreads = context.threads;
	//m_param->sourceWidth = context.width;
	//m_param->sourceHeight = context.height;
	//m_param->internalCsp = X265_CSP_I420;
	////m_param->totalFrames = 0;
	//m_param->fpsNum = context.fps;
	//m_param->fpsDenom = 1;
	//m_param->bRepeatHeaders = 1;

	//m_param->rc.rateControlMode = X265_RC_ABR;
	////m_param->rc.qpMin = 1;
	////m_param->rc.qpMax = 51;
	////m_param->rc.bitrate = context.bit_rate / 5 * 4;
	////m_param->rc.vbvMaxBitrate = context.max_bit_rate / 5 * 4;

	////m_param->rc.vbvBufferSize = m_param->rc.bitrate / context.codecs;




	//m_pic = x265_picture_alloc();
	//x265_picture_init(m_param, m_pic);

	//m_pHandle = x265_encoder_open(m_param);
	//if (m_pHandle == NULL) {
	//	printf("x265_encoder_open err\n");
	//	return false;
	//}

	//image_y_size = m_param->sourceWidth * m_param->sourceHeight;
	//image_u_size = image_y_size / 4;

	//m_yv12_buffer = new char[image_y_size * 1.5];
	//
	//m_xColorSpace->Init(m_param->sourceWidth, m_param->sourceHeight, m_param->sourceWidth);

	return true;
}

bool x265EncWrapper::EncodeVideo(VideoPackage in_pkt, bool key, VideoPackage & out_pkt)
{
	
	//bool ret = m_xColorSpace->RGB24_YV12((unsigned char*)in_pkt.data, (unsigned char*)m_yv12_buffer);
	//if (ret == false)
	//{
	//	return ret;
	//}

	//x265_nal* nal = NULL;
	//uint32_t i_nal = 0;

	//BYTE* p = (BYTE*)m_yv12_buffer;
 //
	//m_pic->planes[0] = p;
	//m_pic->planes[1] = p + image_y_size;
	//m_pic->planes[2] = p + image_y_size + image_u_size;
	//m_pic->stride[0] = m_param->sourceWidth;
	//m_pic->stride[1] = m_param->sourceWidth / 2;
	//m_pic->stride[2] = m_param->sourceWidth / 2;


	//m_i_frame++;
	//m_pic->pts = (int64_t)m_i_frame * m_param->fpsDenom;

	//m_pic->sliceType = key ? X265_TYPE_IDR : X265_TYPE_AUTO;

	//int size = x265_encoder_encode(m_pHandle, &nal, &i_nal, m_pic, NULL);
	//if (size < 0)
	//	return false;

	//if (i_nal > 0)
	//{
	//	out_pkt.data = nal[0].payload;
	//	out_pkt.size = nal[0].sizeBytes;
	//	return true;
	//}
	
	return false;
}
