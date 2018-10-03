#pragma once
#include "AVFastCodecLib.h"
#include <windows.h>
extern "C"
{
#include "x264.h"
}

namespace x264tunning {

	typedef int(*my_x264_param_default_preset)
		(x264_param_t *, const char *preset, const char *tune);
	typedef int(*my_x264_param_apply_profile)
		(x264_param_t *, const char *profile);

	void tunning_gs(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule) {

		//param.i_scenecut_threshold = 400;

		param.rc.i_rc_method = X264_RC_ABR;//X264_ME_UMH;
		param.rc.i_qp_constant = context.i_qp_constant;
		param.rc.i_qp_min = 1;
		param.rc.i_qp_max = 51;
		param.rc.i_bitrate = context.bit_rate / 5 * 4;
		param.rc.i_vbv_max_bitrate = context.max_bit_rate / 5 * 4;
		{
			//控制码率波动范围,由以下1个参数控制
			param.rc.i_vbv_buffer_size = param.rc.i_bitrate / context.codecs;
		}
		//param.rc.i_qp_constant = 51;
		if (context.limit_i_size)
			param.rc.f_rate_tolerance = (float)0.02; //这个可以控制码率波动范围，0.01最小值，波动最小

													 //f_rf_constant
													 //param.rc.i_qp_min = 10;
													 //param.rc.i_qp_max = 51;
													 //param.rc.i_qp_step = 4;
													 //param.rc.f_ip_factor = 1.4;
													 //param.rc.f_pb_factor = 1.3;

		param.i_keyint_max = context.max_key_frame;
		param.i_keyint_min = context.min_key_frame;

		param.rc.i_lookahead = 0; //默认编码缓存40帧
		param.rc.b_mb_tree = 0;
		param.i_bframe = 0; //默认编码缓存3帧
		param.i_bframe_adaptive = 0;
		param.i_bframe_bias = 0;
		param.i_bframe_pyramid = 0;
		param.i_dpb_size = 0;

		//		some tunning has been duplicated
		//		param.b_sliced_threads = 1;
		param.b_deterministic = 0;
		//		param.i_sync_lookahead = 0;

		param.analyse.b_weighted_bipred = 0;
		param.analyse.i_weighted_pred = X264_WEIGHTP_NONE;

		param.analyse.i_mv_range = 512;//运动向量搜索范围 默认512

									   //like zerolatency tunning
		param.rc.i_lookahead = 0;
		param.i_sync_lookahead = 0;
		param.i_bframe = 0;
		param.b_sliced_threads = context.threads;
		param.b_vfr_input = 0;
		param.rc.b_mb_tree = 0;

		printf("Use inner tunning for x264 encoding: GS mode\n");
	}

	void tunning_psp(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule) {

		param.rc.i_qp_constant = 28;
		param.rc.i_rc_method = X264_RC_CQP;
		param.i_level_idc = 21;
	}

	//the conservative tunning for flash
	void tunning_flash_ex(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule) {
		my_x264_param_default_preset x264_param_default_preset_fun = (my_x264_param_default_preset)GetProcAddress(m_x264_dll_hModule, "x264_param_default_preset");
		x264_param_default_preset_fun(&param, "veryfast", "zerolatency");

		//		my_x264_param_apply_profile x264_param_apply_profile_fun = (my_x264_param_apply_profile)GetProcAddress(m_x264_dll_hModule, "x264_param_apply_profile");
		//		x264_param_apply_profile_fun(&param, "baseline");

		//it has been tunning in "veryfast"
		//param.i_frame_reference = 1;

		param.i_keyint_max = context.max_key_frame;
		param.i_keyint_min = context.min_key_frame;

		//		param.rc.i_rc_method = X264_RC_ABR;
		param.rc.i_qp_constant = context.i_qp_constant;
		//		param.rc.i_bitrate = context.bit_rate;		
		param.rc.i_vbv_max_bitrate = context.bit_rate;
		param.rc.i_vbv_buffer_size = param.rc.i_vbv_max_bitrate;

		printf("Use inner tunning for x264 encoding: flash mode\n");
	}

	//the aggressive tunning for flash: No I frame, one frame vbv
	void tunning_flash_ex2(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule) {
		tunning_flash_ex(context, param, m_x264_dll_hModule);

		param.i_keyint_max = X264_KEYINT_MAX_INFINITE;
		param.b_intra_refresh = 1;
		//abandoned: //allowing latency about 1 frames: a compromise setting
		param.rc.i_vbv_buffer_size = param.rc.i_vbv_max_bitrate / context.fps;

		printf("Use inner tunning for x264 encoding: flash enhanced mode\n");
	}

	//the more aggressive tunning for flash: sliced encoded
	void tunning_flash_ex3(EncContext context, x264_param_t& param, HMODULE m_x264_dll_hModule) {
		tunning_flash_ex2(context, param, m_x264_dll_hModule);

		param.b_sliced_threads = context.threads;
		param.i_slice_count = (context.threads <= 0 || context.threads > 4) ? 4 : context.threads;

		printf("Use inner tunning for x264 encoding: flash crazy mode\n");

	}

}