#pragma once
#include "AVFastCodecLib.h"
#include "xColorSpace.h"

extern "C"
{
#include "x264.h"
}


typedef void(*my_x264_param_default)(x264_param_t* param);
typedef x264_t* (*my_x264_encoder_open)(x264_param_t* param);
typedef int(*my_x264_picture_alloc)(x264_picture_t *pic, int i_csp, int i_width, int i_height);
typedef int(*my_x264_encoder_encode)(x264_t *, x264_nal_t **pp_nal, int *pi_nal, x264_picture_t *pic_in, x264_picture_t *pic_out);
typedef void(*my_x264_nal_encode)(x264_t *h, uint8_t *dst, x264_nal_t *nal);
typedef int(*my_x264_encoder_reconfig)(x264_t *, x264_param_t *);


class x264EncWrapper
{
public:
	x264EncWrapper();
	virtual ~x264EncWrapper();

private:
	x264_t* m_h;
	x264_picture_t m_pic;
	int image_y_size;
	int image_u_size;

	x264_param_t	m_param;

	EncContext		m_context;
	int				m_i_frame;
	xColorSpace*	m_xColorSpace;
	void* m_yv12_buffer;
	HMODULE			m_x264_dll_hModule;
	my_x264_encoder_encode m_x264_encoder_encode;
	my_x264_nal_encode		m_x264_nal_encode;
	bool m_header;


public:
	//---------------------------------------------
	bool  InitEnc(EncContext context);
	bool  EncodeVideo(VideoPackage in_pkt, bool key, VideoPackage& out_pkt);
	bool  ReconfigVideoBitrate(int bitrate);
};

