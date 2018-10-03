#pragma once

//#if defined ( __cplusplus)
//extern "C"
//{
//#include "x265.h"
//};
//#else
//#include "x265.h"
//#endif

#include "AVFastCodecLib.h"
#include "xColorSpace.h"

class x265EncWrapper
{
public:
	x265EncWrapper();
	virtual ~x265EncWrapper();

	//---------------------------------------------
	bool  init(EncContext context);
	bool  EncodeVideo(VideoPackage in_pkt, bool key, VideoPackage& out_pkt);


private:
	//x265_encoder *m_pHandle = NULL;
	//x265_param *m_param;
	//x265_picture* m_pic;
	int image_y_size;
	int image_u_size;


	EncContext		m_context;
	int				m_i_frame;
	xColorSpace*	m_xColorSpace;
	void* m_yv12_buffer;

	bool m_header;
};
