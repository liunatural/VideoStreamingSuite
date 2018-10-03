#include "x264EncWrapper.h"

x264EncWrapper::x264EncWrapper()
{
	m_i_frame = 0;
	m_header = true;
	m_x264_dll_hModule = LoadLibrary("libx264.dll");
}


x264EncWrapper::~x264EncWrapper()
{
	FreeLibrary(m_x264_dll_hModule);
}


int x264_param_apply_tune(x264_param_t* param, const char* tune)
{
	param->rc.i_lookahead = 0;
	param->i_sync_lookahead = 0;
	param->i_bframe = 0;
	param->b_sliced_threads = 1;
	param->b_vfr_input = 0;
	param->rc.b_mb_tree = 0;
	return 0;
}


namespace x264tunning {
	void tunning_gs(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule);
	void tunning_flash_ex(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule);
	void tunning_flash_ex2(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule);
	void tunning_flash_ex3(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule);
}

//------------------------------------------------------------------------------------------------------------------
bool __stdcall x264EncWrapper::InitEnc(EncContext context)
{
	m_context = context;

	x264_param_t param = { 0 };

	//p->rc.i_qp_constant = atoi(value);
	//p->rc.i_rc_method = X264_RC_CQP;
	my_x264_param_default x264_param_default_fun = (my_x264_param_default)GetProcAddress(m_x264_dll_hModule, "x264_param_default");
	x264_param_default_fun(&param);

	//switch(context.video_type){
	//case codec_x264_flash_ex:
	//	x264tunning::tunning_flash_ex(context, param, m_x264_dll_hModule);
	//	break;
	//case codec_x264_flash_ex2:
	//	x264tunning::tunning_flash_ex2(context, param, m_x264_dll_hModule);
	//	break;
	//case codec_x264_flash_ex3:
	//	x264tunning::tunning_flash_ex3(context, param, m_x264_dll_hModule);
	//	break;
	//default:
	x264tunning::tunning_gs(context, param, m_x264_dll_hModule);
	//	break;
	//};

	param.b_cabac = 1;
	param.i_threads = context.threads;

	param.i_width = context.width;
	param.i_height = context.height;
	param.i_frame_total = 0;
	param.i_fps_num = context.fps;
	param.i_fps_den = 1;

	std::string fun_open_name = "x264_encoder_open_";
	char buffer[16] = { 0 };
	_itoa(X264_BUILD, buffer, 10);
	fun_open_name += buffer;
	my_x264_encoder_open x264_encoder_open_fun = (my_x264_encoder_open)GetProcAddress(m_x264_dll_hModule, fun_open_name.c_str());
	m_h = x264_encoder_open_fun(&param);

	if (!m_h)
	{
		//好像高度不能为奇数
		//MessageBox(NULL, "请检查服务器配置文件中的编码核心数设置！", "X264Shell编码器初始化失败", MB_OK);
		return false;
	}

	my_x264_picture_alloc x264_picture_alloc_fun = (my_x264_picture_alloc)GetProcAddress(m_x264_dll_hModule, "x264_picture_alloc");
	x264_picture_alloc_fun(&m_pic, X264_CSP_I420, param.i_width, param.i_height);

	image_y_size = param.i_width * param.i_height;
	image_u_size = image_y_size / 4;

	m_param = param;

	m_x264_encoder_encode = (my_x264_encoder_encode)GetProcAddress(m_x264_dll_hModule, "x264_encoder_encode");

	m_x264_nal_encode = (my_x264_nal_encode)GetProcAddress(m_x264_dll_hModule, "x264_nal_encode");

	m_yv12_buffer = new char[image_y_size * 1.5];
	m_xColorSpace = new xColorSpace();
	m_xColorSpace->Init(param.i_width, param.i_height, param.i_width);
	return true;
}





bool  x264EncWrapper::EncodeVideo(VideoPackage in_pkt, bool key, VideoPackage& out_pkt)
{
	bool ret = m_xColorSpace->RGB24_YV12((unsigned char*)in_pkt.data, (unsigned char*)m_yv12_buffer);
	if (ret == false)
	{
		return ret;
	}

	x264_nal_t* nal = NULL;
	int i_nal = 0;
	x264_picture_t pic_out = { 0 };

	BYTE* p = (BYTE*)m_yv12_buffer;

	m_pic.img.plane[0] = p;
	m_pic.img.plane[1] = p + image_y_size;
	m_pic.img.plane[2] = p + image_y_size + image_u_size;
	m_pic.img.plane[3] = NULL;
	m_pic.img.i_stride[3] = 0;

	m_i_frame++;
	m_pic.i_pts = (int64_t)m_i_frame * m_param.i_fps_den;

	m_pic.i_type = key ? X264_TYPE_IDR : X264_TYPE_AUTO;

	int size = m_x264_encoder_encode(m_h, &nal, &i_nal, &m_pic, &pic_out);

	if (size < 0)
		return false;


	out_pkt.data = nal[0].p_payload;
	out_pkt.size = size;
	return true;
}



bool __stdcall x264EncWrapper::ReconfigVideoBitrate(int bitrate)
{
	my_x264_encoder_reconfig reconfig = (my_x264_encoder_reconfig)GetProcAddress(m_x264_dll_hModule, "x264_encoder_reconfig");
	m_param.rc.i_bitrate = bitrate / 5 * 4;
	m_param.rc.i_vbv_max_bitrate = bitrate / 5 * 4;
	reconfig(m_h, &m_param);
	return true;
}