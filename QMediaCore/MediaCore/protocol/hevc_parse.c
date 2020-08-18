/*
* Copyright (c) 2014 Tim Walker <tdskywalker@gmail.com>
*
* This file is part of FFmpeg.
*
* FFmpeg is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* FFmpeg is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with FFmpeg; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <stddef.h>
#include "hevc_parse.h"
#include "bitstream.h"
#include "golomb.h"
#include "fs_io.h"

uint8_t* avc_find_startcode(const uint8_t* inp, int size, int* nal_header_size)
{
    uint8_t* p = (uint8_t*)inp;
    const uint8_t *end = p + size;
    uint8_t *retp = NULL;
    *nal_header_size = 3;
    uint32_t complare = 0xffff0000 | ((uint32_t)p[0] << 8) | ((uint32_t)p[1]);
    p += 2;
    
    while (p < end)
    {
        complare = (complare << 8) | ((uint32_t)p[0]);
        p++;
        if ((complare & 0x00ffffff) == 0x1)
        {
            if((complare & 0xff000000) == 0)
                *nal_header_size = 4;

            retp = p - *nal_header_size;
            break;
        }
    }

    return retp;
}

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))
#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define FF_PROFILE_HEVC_REXT                        4

#define START_CODE 0x000001 ///< start_code_prefix_one_3bytes

#define IS_IRAP_NAL(nal) (nal->type >= 16 && nal->type <= 23)
#define IS_IDR_NAL(nal) (nal->type == HEVC_NAL_IDR_W_RADL || nal->type == HEVC_NAL_IDR_N_LP)

#define MEM_RB8(ptr) (*(ptr))
#define MEM_RB16(ptr) ((MEM_RB8(ptr) << 8)|MEM_RB8(ptr+1))
#define MEM_RB24(ptr) ((MEM_RB16(ptr)<<8)|MEM_RB8(ptr+2))
#define MEM_RB32(ptr) ((MEM_RB16(ptr)<<16)|MEM_RB16(ptr+2))
#define MEM_RB64(ptr) (((uint64_t)MEM_RB32(ptr)<<32)|MEM_RB32(ptr+4))

const uint8_t ff_hevc_diag_scan4x4_x[16] = {
		0, 0, 1, 0,
		1, 2, 0, 1,
		2, 3, 1, 2,
		3, 2, 3, 3,
};

const uint8_t ff_hevc_diag_scan4x4_y[16] = {
		0, 1, 0, 2,
		1, 0, 3, 2,
		1, 0, 3, 2,
		1, 3, 2, 3,
};

const uint8_t ff_hevc_diag_scan8x8_x[64] = {
		0, 0, 1, 0,
		1, 2, 0, 1,
		2, 3, 0, 1,
		2, 3, 4, 0,
		1, 2, 3, 4,
		5, 0, 1, 2,
		3, 4, 5, 6,
		0, 1, 2, 3,
		4, 5, 6, 7,
		1, 2, 3, 4,
		5, 6, 7, 2,
		3, 4, 5, 6,
		7, 3, 4, 5,
		6, 7, 4, 5,
		6, 7, 5, 6,
		7, 6, 7, 7,
};

const uint8_t ff_hevc_diag_scan8x8_y[64] = {
		0, 1, 0, 2,
		1, 0, 3, 2,
		1, 0, 4, 3,
		2, 1, 0, 5,
		4, 3, 2, 1,
		0, 6, 5, 4,
		3, 2, 1, 0,
		7, 6, 5, 4,
		3, 2, 1, 0,
		7, 6, 5, 4,
		3, 2, 1, 7,
		6, 5, 4, 3,
		2, 7, 6, 5,
		4, 3, 7, 6,
		5, 4, 7, 6,
		5, 7, 6, 7,
};

static const uint8_t default_scaling_list_intra[] = {
		16, 16, 16, 16, 17, 18, 21, 24,
		16, 16, 16, 16, 17, 19, 22, 25,
		16, 16, 17, 18, 20, 22, 25, 29,
		16, 16, 18, 21, 24, 27, 31, 36,
		17, 17, 20, 24, 30, 35, 41, 47,
		18, 19, 22, 27, 35, 44, 54, 65,
		21, 22, 25, 31, 41, 54, 70, 88,
		24, 25, 29, 36, 47, 65, 88, 115
};

static const uint8_t default_scaling_list_inter[] = {
		16, 16, 16, 16, 17, 18, 20, 24,
		16, 16, 16, 17, 18, 20, 24, 25,
		16, 16, 17, 18, 20, 24, 25, 28,
		16, 17, 18, 20, 24, 25, 28, 33,
		17, 18, 20, 24, 25, 28, 33, 41,
		18, 20, 24, 25, 28, 33, 41, 54,
		20, 24, 25, 28, 33, 41, 54, 71,
		24, 25, 28, 33, 41, 54, 71, 91
};

static const Rational vui_sar[] = {
		{  0,   1 },
		{  1,   1 },
		{ 12,  11 },
		{ 10,  11 },
		{ 16,  11 },
		{ 40,  33 },
		{ 24,  11 },
		{ 20,  11 },
		{ 32,  11 },
		{ 80,  33 },
		{ 18,  11 },
		{ 15,  11 },
		{ 64,  33 },
		{ 160, 99 },
		{  4,   3 },
		{  3,   2 },
		{  2,   1 },
};

static const uint8_t hevc_sub_width_c[] = {
		1, 2, 2, 1
};

static const uint8_t hevc_sub_height_c[] = {
		1, 2, 1, 1
};

/** Compute ceil(log2(x)).
 * @param x value used to compute ceil(log2(x))
 * @return computed ceiling of log2(x)
 */
static av_always_inline const int av_ceil_log2_c(int x)
{
	return av_log2((x - 1) << 1);
}

static int map_pixel_format(HEVCSPS *sps)
{
	switch (sps->bit_depth) {
		case 8:
//			if (sps->chroma_format_idc == 0) sps->pix_fmt = AV_PIX_FMT_GRAY8;
//			if (sps->chroma_format_idc == 1) sps->pix_fmt = AV_PIX_FMT_YUV420P;
//			if (sps->chroma_format_idc == 2) sps->pix_fmt = AV_PIX_FMT_YUV422P;
//			if (sps->chroma_format_idc == 3) sps->pix_fmt = AV_PIX_FMT_YUV444P;
			break;
		case 9:
//			if (sps->chroma_format_idc == 0) sps->pix_fmt = AV_PIX_FMT_GRAY9;
//			if (sps->chroma_format_idc == 1) sps->pix_fmt = AV_PIX_FMT_YUV420P9;
//			if (sps->chroma_format_idc == 2) sps->pix_fmt = AV_PIX_FMT_YUV422P9;
//			if (sps->chroma_format_idc == 3) sps->pix_fmt = AV_PIX_FMT_YUV444P9;
			break;
		case 10:
//			if (sps->chroma_format_idc == 0) sps->pix_fmt = AV_PIX_FMT_GRAY10;
//			if (sps->chroma_format_idc == 1) sps->pix_fmt = AV_PIX_FMT_YUV420P10;
//			if (sps->chroma_format_idc == 2) sps->pix_fmt = AV_PIX_FMT_YUV422P10;
//			if (sps->chroma_format_idc == 3) sps->pix_fmt = AV_PIX_FMT_YUV444P10;
			break;
		case 12:
//			if (sps->chroma_format_idc == 0) sps->pix_fmt = AV_PIX_FMT_GRAY12;
//			if (sps->chroma_format_idc == 1) sps->pix_fmt = AV_PIX_FMT_YUV420P12;
//			if (sps->chroma_format_idc == 2) sps->pix_fmt = AV_PIX_FMT_YUV422P12;
//			if (sps->chroma_format_idc == 3) sps->pix_fmt = AV_PIX_FMT_YUV444P12;
			break;
		default:
			return -1;
	}

	sps->hshift[0] = sps->vshift[0] = 0;
//	sps->hshift[2] = sps->hshift[1] = desc->log2_chroma_w;
//	sps->vshift[2] = sps->vshift[1] = desc->log2_chroma_h;

	sps->pixel_shift = sps->bit_depth > 8;

	return 0;
}

static void set_default_scaling_list_data(ScalingList *sl)
{
	int matrixId;

	for (matrixId = 0; matrixId < 6; matrixId++) {
		// 4x4 default is 16
		memset(sl->sl[0][matrixId], 16, 16);
		sl->sl_dc[0][matrixId] = 16; // default for 16x16
		sl->sl_dc[1][matrixId] = 16; // default for 32x32
	}
	memcpy(sl->sl[1][0], default_scaling_list_intra, 64);
	memcpy(sl->sl[1][1], default_scaling_list_intra, 64);
	memcpy(sl->sl[1][2], default_scaling_list_intra, 64);
	memcpy(sl->sl[1][3], default_scaling_list_inter, 64);
	memcpy(sl->sl[1][4], default_scaling_list_inter, 64);
	memcpy(sl->sl[1][5], default_scaling_list_inter, 64);
	memcpy(sl->sl[2][0], default_scaling_list_intra, 64);
	memcpy(sl->sl[2][1], default_scaling_list_intra, 64);
	memcpy(sl->sl[2][2], default_scaling_list_intra, 64);
	memcpy(sl->sl[2][3], default_scaling_list_inter, 64);
	memcpy(sl->sl[2][4], default_scaling_list_inter, 64);
	memcpy(sl->sl[2][5], default_scaling_list_inter, 64);
	memcpy(sl->sl[3][0], default_scaling_list_intra, 64);
	memcpy(sl->sl[3][1], default_scaling_list_intra, 64);
	memcpy(sl->sl[3][2], default_scaling_list_intra, 64);
	memcpy(sl->sl[3][3], default_scaling_list_inter, 64);
	memcpy(sl->sl[3][4], default_scaling_list_inter, 64);
	memcpy(sl->sl[3][5], default_scaling_list_inter, 64);
}

static int scaling_list_data(GetBitContext *gb, ScalingList *sl, HEVCSPS *sps)
{
	uint8_t scaling_list_pred_mode_flag;
	int32_t scaling_list_dc_coef[2][6];
	int size_id, matrix_id, pos;
	int i;

	for (size_id = 0; size_id < 4; size_id++)
		for (matrix_id = 0; matrix_id < 6; matrix_id += ((size_id == 3) ? 3 : 1)) {
			scaling_list_pred_mode_flag = get_bits1(gb);
			if (!scaling_list_pred_mode_flag) {
				unsigned int delta = get_ue_golomb_long(gb);
				/* Only need to handle non-zero delta. Zero means default,
                 * which should already be in the arrays. */
				if (delta) {
					// Copy from previous array.
					delta *= (size_id == 3) ? 3 : 1;
					if (matrix_id < delta) {
						return -1;
					}

					memcpy(sl->sl[size_id][matrix_id],
						   sl->sl[size_id][matrix_id - delta],
						   size_id > 0 ? 64 : 16);
					if (size_id > 1)
						sl->sl_dc[size_id - 2][matrix_id] = sl->sl_dc[size_id - 2][matrix_id - delta];
				}
			} else {
				int next_coef, coef_num;
				int32_t scaling_list_delta_coef;

				next_coef = 8;
				coef_num  = FFMIN(64, 1 << (4 + (size_id << 1)));
				if (size_id > 1) {
					scaling_list_dc_coef[size_id - 2][matrix_id] = get_se_golomb(gb) + 8;
					next_coef = scaling_list_dc_coef[size_id - 2][matrix_id];
					sl->sl_dc[size_id - 2][matrix_id] = next_coef;
				}
				for (i = 0; i < coef_num; i++) {
					if (size_id == 0)
						pos = 4 * ff_hevc_diag_scan4x4_y[i] +
							  ff_hevc_diag_scan4x4_x[i];
					else
						pos = 8 * ff_hevc_diag_scan8x8_y[i] +
							  ff_hevc_diag_scan8x8_x[i];

					scaling_list_delta_coef = get_se_golomb(gb);
					next_coef = (next_coef + 256U + scaling_list_delta_coef) % 256;
					sl->sl[size_id][matrix_id][pos] = next_coef;
				}
			}
		}

	if (sps->chroma_format_idc == 3) {
		for (i = 0; i < 64; i++) {
			sl->sl[3][1][i] = sl->sl[2][1][i];
			sl->sl[3][2][i] = sl->sl[2][2][i];
			sl->sl[3][4][i] = sl->sl[2][4][i];
			sl->sl[3][5][i] = sl->sl[2][5][i];
		}
		sl->sl_dc[1][1] = sl->sl_dc[0][1];
		sl->sl_dc[1][2] = sl->sl_dc[0][2];
		sl->sl_dc[1][4] = sl->sl_dc[0][4];
		sl->sl_dc[1][5] = sl->sl_dc[0][5];
	}


	return 0;
}

int hevc_decode_short_term_rps(GetBitContext *gb,
								  ShortTermRPS *rps, const HEVCSPS *sps, int is_slice_header)
{
	uint8_t rps_predict = 0;
	int delta_poc;
	int k0 = 0;
	int k1 = 0;
	int k  = 0;
	int i;

	if (rps != sps->st_rps && sps->nb_st_rps)
		rps_predict = get_bits1(gb);

	if (rps_predict) {
		const ShortTermRPS *rps_ridx;
		int delta_rps;
		unsigned abs_delta_rps;
		uint8_t use_delta_flag = 0;
		uint8_t delta_rps_sign;

		if (is_slice_header) {
			unsigned int delta_idx = get_ue_golomb_long(gb) + 1;
			if (delta_idx > sps->nb_st_rps) {
//				av_log(avctx, AV_LOG_ERROR,
//					   "Invalid value of delta_idx in slice header RPS: %d > %d.\n",
//					   delta_idx, sps->nb_st_rps);
//				return AVERROR_INVALIDDATA;
				return -1;
			}
			rps_ridx = &sps->st_rps[sps->nb_st_rps - delta_idx];
			rps->rps_idx_num_delta_pocs = rps_ridx->num_delta_pocs;
		} else
			rps_ridx = &sps->st_rps[rps - sps->st_rps - 1];

		delta_rps_sign = get_bits1(gb);
		abs_delta_rps  = get_ue_golomb_long(gb) + 1;
		if (abs_delta_rps < 1 || abs_delta_rps > 32768) {
//			av_log(avctx, AV_LOG_ERROR,
//				   "Invalid value of abs_delta_rps: %d\n",
//				   abs_delta_rps);
//			return AVERROR_INVALIDDATA;
			return -1;
		}
		delta_rps      = (1 - (delta_rps_sign << 1)) * abs_delta_rps;
		for (i = 0; i <= rps_ridx->num_delta_pocs; i++) {
			int used = rps->used[k] = get_bits1(gb);

			if (!used)
				use_delta_flag = get_bits1(gb);

			if (used || use_delta_flag) {
				if (i < rps_ridx->num_delta_pocs)
					delta_poc = delta_rps + rps_ridx->delta_poc[i];
				else
					delta_poc = delta_rps;
				rps->delta_poc[k] = delta_poc;
				if (delta_poc < 0)
					k0++;
				else
					k1++;
				k++;
			}
		}

		if (k >= (sizeof(rps->used) / sizeof(rps->used[0]))) {
//			av_log(avctx, AV_LOG_ERROR,
//				   "Invalid num_delta_pocs: %d\n", k);
//			return AVERROR_INVALIDDATA;
			return -1;
		}

		rps->num_delta_pocs    = k;
		rps->num_negative_pics = k0;
		// sort in increasing order (smallest first)
		if (rps->num_delta_pocs != 0) {
			int used, tmp;
			for (i = 1; i < rps->num_delta_pocs; i++) {
				delta_poc = rps->delta_poc[i];
				used      = rps->used[i];
				for (k = i - 1; k >= 0; k--) {
					tmp = rps->delta_poc[k];
					if (delta_poc < tmp) {
						rps->delta_poc[k + 1] = tmp;
						rps->used[k + 1]      = rps->used[k];
						rps->delta_poc[k]     = delta_poc;
						rps->used[k]          = used;
					}
				}
			}
		}
		if ((rps->num_negative_pics >> 1) != 0) {
			int used;
			k = rps->num_negative_pics - 1;
			// flip the negative values to largest first
			for (i = 0; i < rps->num_negative_pics >> 1; i++) {
				delta_poc         = rps->delta_poc[i];
				used              = rps->used[i];
				rps->delta_poc[i] = rps->delta_poc[k];
				rps->used[i]      = rps->used[k];
				rps->delta_poc[k] = delta_poc;
				rps->used[k]      = used;
				k--;
			}
		}
	} else {
		unsigned int prev, nb_positive_pics;
		rps->num_negative_pics = get_ue_golomb_long(gb);
		nb_positive_pics       = get_ue_golomb_long(gb);

		if (rps->num_negative_pics >= HEVC_MAX_REFS ||
			nb_positive_pics >= HEVC_MAX_REFS) {
//			av_log(avctx, AV_LOG_ERROR, "Too many refs in a short term RPS.\n");
//			return AVERROR_INVALIDDATA;
			return -1;
		}

		rps->num_delta_pocs = rps->num_negative_pics + nb_positive_pics;
		if (rps->num_delta_pocs) {
			prev = 0;
			for (i = 0; i < rps->num_negative_pics; i++) {
				delta_poc = get_ue_golomb_long(gb) + 1;
				if (delta_poc < 1 || delta_poc > 32768) {
//					av_log(avctx, AV_LOG_ERROR,
//						   "Invalid value of delta_poc: %d\n",
//						   delta_poc);
//					return AVERROR_INVALIDDATA;
					return -1;
				}
				prev -= delta_poc;
				rps->delta_poc[i] = prev;
				rps->used[i]      = get_bits1(gb);
			}
			prev = 0;
			for (i = 0; i < nb_positive_pics; i++) {
				delta_poc = get_ue_golomb_long(gb) + 1;
				if (delta_poc < 1 || delta_poc > 32768) {
//					av_log(avctx, AV_LOG_ERROR,
//						   "Invalid value of delta_poc: %d\n",
//						   delta_poc);
//					return AVERROR_INVALIDDATA;
					return -1;
				}
				prev += delta_poc;
				rps->delta_poc[rps->num_negative_pics + i] = prev;
				rps->used[rps->num_negative_pics + i]      = get_bits1(gb);
			}
		}
	}
	return 0;
}

static void decode_sublayer_hrd(GetBitContext *gb, unsigned int nb_cpb,
                                int subpic_params_present)
{
    int i;

    for (i = 0; i < nb_cpb; i++) {
        get_ue_golomb_long(gb); // bit_rate_value_minus1
        get_ue_golomb_long(gb); // cpb_size_value_minus1

        if (subpic_params_present) {
            get_ue_golomb_long(gb); // cpb_size_du_value_minus1
            get_ue_golomb_long(gb); // bit_rate_du_value_minus1
        }
        skip_bits1(gb); // cbr_flag
    }
}

static int decode_hrd(GetBitContext *gb, int common_inf_present,
                      int max_sublayers)
{
    int nal_params_present = 0, vcl_params_present = 0;
    int subpic_params_present = 0;
    int i;

    if (common_inf_present) {
        nal_params_present = get_bits1(gb);
        vcl_params_present = get_bits1(gb);

        if (nal_params_present || vcl_params_present) {
            subpic_params_present = get_bits1(gb);

            if (subpic_params_present) {
                skip_bits(gb, 8); // tick_divisor_minus2
                skip_bits(gb, 5); // du_cpb_removal_delay_increment_length_minus1
                skip_bits(gb, 1); // sub_pic_cpb_params_in_pic_timing_sei_flag
                skip_bits(gb, 5); // dpb_output_delay_du_length_minus1
            }

            skip_bits(gb, 4); // bit_rate_scale
            skip_bits(gb, 4); // cpb_size_scale

            if (subpic_params_present)
                skip_bits(gb, 4);  // cpb_size_du_scale

            skip_bits(gb, 5); // initial_cpb_removal_delay_length_minus1
            skip_bits(gb, 5); // au_cpb_removal_delay_length_minus1
            skip_bits(gb, 5); // dpb_output_delay_length_minus1
        }
    }

    for (i = 0; i < max_sublayers; i++) {
        int low_delay = 0;
        unsigned int nb_cpb = 1;
        int fixed_rate = get_bits1(gb);

        if (!fixed_rate)
            fixed_rate = get_bits1(gb);

        if (fixed_rate)
            get_ue_golomb_long(gb);  // elemental_duration_in_tc_minus1
        else
            low_delay = get_bits1(gb);

        if (!low_delay) {
            nb_cpb = get_ue_golomb_long(gb) + 1;
            if (nb_cpb < 1 || nb_cpb > 32) {
//                av_log(NULL, AV_LOG_ERROR, "nb_cpb %d invalid\n", nb_cpb);
                return -1;
            }
        }

        if (nal_params_present)
            decode_sublayer_hrd(gb, nb_cpb, subpic_params_present);
        if (vcl_params_present)
            decode_sublayer_hrd(gb, nb_cpb, subpic_params_present);
    }
    return 0;
}

static void decode_vui(GetBitContext *gb,
					   int apply_defdispwin, HEVCSPS *sps)
{
	VUI backup_vui, *vui = &sps->vui;
	GetBitContext backup;
	int sar_present, alt = 0;

	sar_present = get_bits1(gb);
	if (sar_present) {
		uint8_t sar_idx = get_bits(gb, 8);
		if (sar_idx < FF_ARRAY_ELEMS(vui_sar))
			vui->sar = vui_sar[sar_idx];
		else if (sar_idx == 255) {
			vui->sar.num = get_bits(gb, 16);
			vui->sar.den = get_bits(gb, 16);
		} /*else
			av_log(avctx, AV_LOG_WARNING,
				   "Unknown SAR index: %u.\n", sar_idx);*/
	}

	vui->overscan_info_present_flag = get_bits1(gb);
	if (vui->overscan_info_present_flag)
		vui->overscan_appropriate_flag = get_bits1(gb);

	vui->video_signal_type_present_flag = get_bits1(gb);
	if (vui->video_signal_type_present_flag) {
		vui->video_format                    = get_bits(gb, 3);
		vui->video_full_range_flag           = get_bits1(gb);
		vui->colour_description_present_flag = get_bits1(gb);
//		if (vui->video_full_range_flag && sps->pix_fmt == AV_PIX_FMT_YUV420P)
//			sps->pix_fmt = AV_PIX_FMT_YUVJ420P;
		if (vui->colour_description_present_flag) {
			vui->colour_primaries        = get_bits(gb, 8);
			vui->transfer_characteristic = get_bits(gb, 8);
			vui->matrix_coeffs           = get_bits(gb, 8);

			// Set invalid values to "unspecified"
#if 0
			if (!av_color_primaries_name(vui->colour_primaries))
				vui->colour_primaries = AVCOL_PRI_UNSPECIFIED;
			if (!av_color_transfer_name(vui->transfer_characteristic))
				vui->transfer_characteristic = AVCOL_TRC_UNSPECIFIED;
			if (!av_color_space_name(vui->matrix_coeffs))
				vui->matrix_coeffs = AVCOL_SPC_UNSPECIFIED;
			if (vui->matrix_coeffs == AVCOL_SPC_RGB) {
				switch (sps->pix_fmt) {
					case AV_PIX_FMT_YUV444P:
						sps->pix_fmt = AV_PIX_FMT_GBRP;
						break;
					case AV_PIX_FMT_YUV444P10:
						sps->pix_fmt = AV_PIX_FMT_GBRP10;
						break;
					case AV_PIX_FMT_YUV444P12:
						sps->pix_fmt = AV_PIX_FMT_GBRP12;
						break;
				}
			}
#endif
		}
	}

	vui->chroma_loc_info_present_flag = get_bits1(gb);
	if (vui->chroma_loc_info_present_flag) {
		vui->chroma_sample_loc_type_top_field    = get_ue_golomb_long(gb);
		vui->chroma_sample_loc_type_bottom_field = get_ue_golomb_long(gb);
	}

	vui->neutra_chroma_indication_flag = get_bits1(gb);
	vui->field_seq_flag                = get_bits1(gb);
	vui->frame_field_info_present_flag = get_bits1(gb);

	// Backup context in case an alternate header is detected
	memcpy(&backup, gb, sizeof(backup));
	memcpy(&backup_vui, vui, sizeof(backup_vui));
	if (get_bits_left(gb) >= 68 && show_bits_long(gb, 21) == 0x100000) {
		vui->default_display_window_flag = 0;
//		av_log(avctx, AV_LOG_WARNING, "Invalid default display window\n");
	} else
		vui->default_display_window_flag = get_bits1(gb);

	if (vui->default_display_window_flag) {
		int vert_mult  = hevc_sub_height_c[sps->chroma_format_idc];
		int horiz_mult = hevc_sub_width_c[sps->chroma_format_idc];
		vui->def_disp_win.left_offset   = get_ue_golomb_long(gb) * horiz_mult;
		vui->def_disp_win.right_offset  = get_ue_golomb_long(gb) * horiz_mult;
		vui->def_disp_win.top_offset    = get_ue_golomb_long(gb) *  vert_mult;
		vui->def_disp_win.bottom_offset = get_ue_golomb_long(gb) *  vert_mult;

//		if (apply_defdispwin &&
//			avctx->flags2 & AV_CODEC_FLAG2_IGNORE_CROP) {
//			av_log(avctx, AV_LOG_DEBUG,
//				   "discarding vui default display window, "
//				   "original values are l:%u r:%u t:%u b:%u\n",
//				   vui->def_disp_win.left_offset,
//				   vui->def_disp_win.right_offset,
//				   vui->def_disp_win.top_offset,
//				   vui->def_disp_win.bottom_offset);
//
//			vui->def_disp_win.left_offset   =
//			vui->def_disp_win.right_offset  =
//			vui->def_disp_win.top_offset    =
//			vui->def_disp_win.bottom_offset = 0;
//		}
	}

	timing_info:
	vui->vui_timing_info_present_flag = get_bits1(gb);

	if (vui->vui_timing_info_present_flag) {
		if( get_bits_left(gb) < 66 && !alt) {
			// The alternate syntax seem to have timing info located
			// at where def_disp_win is normally located
//			av_log(avctx, AV_LOG_WARNING,
//				   "Strange VUI timing information, retrying...\n");
			memcpy(vui, &backup_vui, sizeof(backup_vui));
			memcpy(gb, &backup, sizeof(backup));
			alt = 1;
			goto timing_info;
		}
		vui->vui_num_units_in_tick               = get_bits_long(gb, 32);
		vui->vui_time_scale                      = get_bits_long(gb, 32);
//		if (alt) {
//			av_log(avctx, AV_LOG_INFO, "Retry got %"PRIu32"/%"PRIu32" fps\n",
//				   vui->vui_time_scale, vui->vui_num_units_in_tick);
//		}
		vui->vui_poc_proportional_to_timing_flag = get_bits1(gb);
		if (vui->vui_poc_proportional_to_timing_flag)
			vui->vui_num_ticks_poc_diff_one_minus1 = get_ue_golomb_long(gb);
		vui->vui_hrd_parameters_present_flag = get_bits1(gb);
		if (vui->vui_hrd_parameters_present_flag)
			decode_hrd(gb, 1, sps->max_sub_layers);
	}

	vui->bitstream_restriction_flag = get_bits1(gb);
	if (vui->bitstream_restriction_flag) {
		if (get_bits_left(gb) < 8 && !alt) {
//			av_log(avctx, AV_LOG_WARNING,
//				   "Strange VUI bitstream restriction information, retrying"
//				   " from timing information...\n");
			memcpy(vui, &backup_vui, sizeof(backup_vui));
			memcpy(gb, &backup, sizeof(backup));
			alt = 1;
			goto timing_info;
		}
		vui->tiles_fixed_structure_flag              = get_bits1(gb);
		vui->motion_vectors_over_pic_boundaries_flag = get_bits1(gb);
		vui->restricted_ref_pic_lists_flag           = get_bits1(gb);
		vui->min_spatial_segmentation_idc            = get_ue_golomb_long(gb);
		vui->max_bytes_per_pic_denom                 = get_ue_golomb_long(gb);
		vui->max_bits_per_min_cu_denom               = get_ue_golomb_long(gb);
		vui->log2_max_mv_length_horizontal           = get_ue_golomb_long(gb);
		vui->log2_max_mv_length_vertical             = get_ue_golomb_long(gb);
	}

	if (get_bits_left(gb) < 1 && !alt) {
		// XXX: Alternate syntax when sps_range_extension_flag != 0?
//		av_log(avctx, AV_LOG_WARNING,
//			   "Overread in VUI, retrying from timing information...\n");
		memcpy(vui, &backup_vui, sizeof(backup_vui));
		memcpy(gb, &backup, sizeof(backup));
		alt = 1;
		goto timing_info;
	}
}

HEVCParamSets * CreateHEVCParamSets() {
	HEVCParamSets *ps = calloc(1, sizeof(HEVCParamSets));
	memset(ps->vps_list, 0, HEVC_MAX_VPS_COUNT * sizeof(HEVCVPS *));
	memset(ps->sps_list, 0, HEVC_MAX_SPS_COUNT * sizeof(HEVCSPS *));
	memset(ps->pps_list, 0, HEVC_MAX_PPS_COUNT * sizeof(HEVCPPS *));
	ps->sps = NULL;
	ps->pps = NULL;
	ps->vps = NULL;
	return ps;
}

#define FREE_P(p) \
if(p) {\
	free(p);\
	p = NULL;\
}

static void hevc_pps_free(HEVCPPS **p_pps)
{
	HEVCPPS *pps = *p_pps;
	if (!pps)
		return;

	FREE_P(pps->column_width);
	FREE_P(pps->row_height);
	FREE_P(pps->col_bd);
	FREE_P(pps->row_bd);
	FREE_P(pps->col_idxX);
	FREE_P(pps->ctb_addr_rs_to_ts);
	FREE_P(pps->ctb_addr_ts_to_rs);
	FREE_P(pps->tile_pos_rs);
	FREE_P(pps->tile_id);
	FREE_P(pps->min_tb_addr_zs_tab);

	free(pps);
	*p_pps = NULL;
}

void ReleaseHEVCParamSets(HEVCParamSets **p_ps) {
	int i;
	HEVCParamSets *ps = *p_ps;
	if (ps == NULL)
		return;
	for (i = 0; i < HEVC_MAX_VPS_COUNT; i++) {
		if (ps->vps_list[i])
			free(ps->vps_list[i]);
	}
	for (i = 0; i < HEVC_MAX_SPS_COUNT; i++) {
		if (ps->sps_list[i])
			free(ps->sps_list[i]);
	}
	for (i = 0; i < HEVC_MAX_PPS_COUNT; i++) {
		if (ps->pps_list[i])
			hevc_pps_free(&ps->pps_list[i]);
	}

	ps->sps = NULL;
	ps->pps = NULL;
	ps->vps = NULL;

	free(ps);
	*p_ps = NULL;
}

int HEVCParseNalHeader(H2645NAL *nal, const uint8_t *data, int size) {
	int nuh_layer_id;

	GetBitContext gbc;
	GetBitContext * gb = &gbc;
	init_get_bits8(gb, data, size);

	if (get_bits1(gb) != 0)
		return -1;

	nal->type = get_bits(gb, 6);

	nuh_layer_id   = get_bits(gb, 6);
	nal->temporal_id = get_bits(gb, 3) - 1;
	if (nal->temporal_id < 0)
		return -1;

	return nuh_layer_id == 0;
}

static int decode_profile_tier_level(GetBitContext *gb, PTLCommon *ptl)
{
	int i;

	if (get_bits_left(gb) < 2+1+5 + 32 + 4 + 16 + 16 + 12)
		return -1;

	ptl->profile_space = get_bits(gb, 2);
	ptl->tier_flag     = get_bits1(gb);
	ptl->profile_idc   = get_bits(gb, 5);

	for (i = 0; i < 32; i++) {
		ptl->profile_compatibility_flag[i] = get_bits1(gb);

		if (ptl->profile_idc == 0 && i > 0 && ptl->profile_compatibility_flag[i])
			ptl->profile_idc = i;
	}
	ptl->progressive_source_flag    = get_bits1(gb);
	ptl->interlaced_source_flag     = get_bits1(gb);
	ptl->non_packed_constraint_flag = get_bits1(gb);
	ptl->frame_only_constraint_flag = get_bits1(gb);

	skip_bits(gb, 16); // XXX_reserved_zero_44bits[0..15]
	skip_bits(gb, 16); // XXX_reserved_zero_44bits[16..31]
	skip_bits(gb, 12); // XXX_reserved_zero_44bits[32..43]

	return 0;
}

static int parse_ptl(GetBitContext *gb,
					 PTL *ptl, int max_num_sub_layers)
{
	int i;
	if (decode_profile_tier_level(gb, &ptl->general_ptl) < 0 ||
		get_bits_left(gb) < 8 + (8*2 * (max_num_sub_layers - 1 > 0))) {
//		av_log(avctx, AV_LOG_ERROR, "PTL information too short\n");
		return -1;
	}

	ptl->general_ptl.level_idc = get_bits(gb, 8);

	for (i = 0; i < max_num_sub_layers - 1; i++) {
		ptl->sub_layer_profile_present_flag[i] = get_bits1(gb);
		ptl->sub_layer_level_present_flag[i]   = get_bits1(gb);
	}

	if (max_num_sub_layers - 1> 0)
		for (i = max_num_sub_layers - 1; i < 8; i++)
			skip_bits(gb, 2); // reserved_zero_2bits[i]
	for (i = 0; i < max_num_sub_layers - 1; i++) {
		if (ptl->sub_layer_profile_present_flag[i] &&
			decode_profile_tier_level(gb, &ptl->sub_layer_ptl[i]) < 0) {
//			av_log(avctx, AV_LOG_ERROR,
//				   "PTL information for sublayer %i too short\n", i);
			return -1;
		}
		if (ptl->sub_layer_level_present_flag[i]) {
			if (get_bits_left(gb) < 8) {
//				av_log(avctx, AV_LOG_ERROR,
//					   "Not enough data for sublayer %i level_idc\n", i);
				return -1;
			} else
				ptl->sub_layer_ptl[i].level_idc = get_bits(gb, 8);
		}
	}

	return 0;
}

int HEVCDecodeNalVPS(const uint8_t *data, int size, HEVCParamSets *ps) {
	int i,j;
	int vps_id = 0;
	ptrdiff_t nal_size;

	GetBitContext gbc;
	GetBitContext * gb = &gbc;
	init_get_bits8(gb, data, size);

	HEVCVPS *vps = calloc(1, sizeof(HEVCVPS));

	nal_size = gb->buffer_end - gb->buffer;
	if (nal_size > sizeof(vps->data)) {
		vps->data_size = sizeof(vps->data);
	} else {
		vps->data_size = nal_size;
	}
	memcpy(vps->data, gb->buffer, vps->data_size);

	vps_id = get_bits(gb, 4);
	if (vps_id >= HEVC_MAX_VPS_COUNT) {
		goto err;
	}

	if (ps->vps_list[vps_id] && (memcmp(ps->vps_list[vps_id]->data, data, size) == 0)) {
		// vps is exist and is same
		return 0;
	}

	if (get_bits(gb, 2) != 3) { // vps_reserved_three_2bits
		goto err;
	}

	vps->vps_max_layers               = get_bits(gb, 6) + 1;
	vps->vps_max_sub_layers           = get_bits(gb, 3) + 1;
	vps->vps_temporal_id_nesting_flag = get_bits1(gb);

	if (get_bits(gb, 16) != 0xffff) { // vps_reserved_ffff_16bits
		goto err;
	}

	if (vps->vps_max_sub_layers > HEVC_MAX_SUB_LAYERS) {
		goto err;
	}

	if (parse_ptl(gb, &vps->ptl, vps->vps_max_sub_layers) < 0)
		goto err;

	vps->vps_sub_layer_ordering_info_present_flag = get_bits1(gb);

	i = vps->vps_sub_layer_ordering_info_present_flag ? 0 : vps->vps_max_sub_layers - 1;
	for (; i < vps->vps_max_sub_layers; i++) {
		vps->vps_max_dec_pic_buffering[i] = get_ue_golomb_long(gb) + 1;
		vps->vps_num_reorder_pics[i]      = get_ue_golomb_long(gb);
		vps->vps_max_latency_increase[i]  = get_ue_golomb_long(gb) - 1;

		if (vps->vps_max_dec_pic_buffering[i] > HEVC_MAX_DPB_SIZE || !vps->vps_max_dec_pic_buffering[i]) {
//			av_log(avctx, AV_LOG_ERROR, "vps_max_dec_pic_buffering_minus1 out of range: %d\n",
//				   vps->vps_max_dec_pic_buffering[i] - 1);
			goto err;
		}
		if (vps->vps_num_reorder_pics[i] > vps->vps_max_dec_pic_buffering[i] - 1) {
//			av_log(avctx, AV_LOG_WARNING, "vps_max_num_reorder_pics out of range: %d\n",
//				   vps->vps_num_reorder_pics[i]);
//			if (avctx->err_recognition & AV_EF_EXPLODE)
				goto err;
		}
	}

	vps->vps_max_layer_id   = get_bits(gb, 6);
	vps->vps_num_layer_sets = get_ue_golomb_long(gb) + 1;
	if (vps->vps_num_layer_sets < 1 || vps->vps_num_layer_sets > 1024 ||
		(vps->vps_num_layer_sets - 1LL) * (vps->vps_max_layer_id + 1LL) > get_bits_left(gb)) {
//		av_log(avctx, AV_LOG_ERROR, "too many layer_id_included_flags\n");
		goto err;
	}

	for (i = 1; i < vps->vps_num_layer_sets; i++)
		for (j = 0; j <= vps->vps_max_layer_id; j++)
			skip_bits(gb, 1);  // layer_id_included_flag[i][j]

	vps->vps_timing_info_present_flag = get_bits1(gb);
	if (vps->vps_timing_info_present_flag) {
		vps->vps_num_units_in_tick               = get_bits_long(gb, 32);
		vps->vps_time_scale                      = get_bits_long(gb, 32);
		vps->vps_poc_proportional_to_timing_flag = get_bits1(gb);
		if (vps->vps_poc_proportional_to_timing_flag)
			vps->vps_num_ticks_poc_diff_one = get_ue_golomb_long(gb) + 1;
		vps->vps_num_hrd_parameters = get_ue_golomb_long(gb);
		if (vps->vps_num_hrd_parameters > (unsigned)vps->vps_num_layer_sets) {
//			av_log(avctx, AV_LOG_ERROR,
//				   "vps_num_hrd_parameters %d is invalid\n", vps->vps_num_hrd_parameters);
			goto err;
		}
		for (i = 0; i < vps->vps_num_hrd_parameters; i++) {
			int common_inf_present = 1;

			get_ue_golomb_long(gb); // hrd_layer_set_idx
			if (i)
				common_inf_present = get_bits1(gb);
			decode_hrd(gb, common_inf_present, vps->vps_max_sub_layers);
		}
	}
	get_bits1(gb); /* vps_extension_flag */

	if (get_bits_left(gb) < 0) {
//		av_log(avctx, AV_LOG_ERROR,
//			   "Overread VPS by %d bits\n", -get_bits_left(gb));
		if (ps->vps_list[vps_id])
			goto err;
	}

	if (ps->vps_list[vps_id])
		free(ps->vps_list[vps_id]);

	ps->vps_list[vps_id] = vps;

	return 0;

err:
	free(vps);
	return -1;
}

int hevc_parse_sps(HEVCSPS *sps, GetBitContext *gb, unsigned int *sps_id,
					  int apply_defdispwin, HEVCVPS **vps_list)
{
	HEVCWindow *ow;
	int ret = 0;
	int log2_diff_max_min_transform_block_size;
	int bit_depth_chroma, start, vui_present, sublayer_ordering_info;
	int i;

	// Coded parameters

	sps->vps_id = get_bits(gb, 4);
	if (sps->vps_id >= HEVC_MAX_VPS_COUNT) {
//		av_log(avctx, AV_LOG_ERROR, "VPS id out of range: %d\n", sps->vps_id);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	if (vps_list && !vps_list[sps->vps_id]) {
//		av_log(avctx, AV_LOG_ERROR, "VPS %d does not exist\n",
//			   sps->vps_id);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	sps->max_sub_layers = get_bits(gb, 3) + 1;
	if (sps->max_sub_layers > HEVC_MAX_SUB_LAYERS) {
//		av_log(avctx, AV_LOG_ERROR, "sps_max_sub_layers out of range: %d\n",
//			   sps->max_sub_layers);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	sps->temporal_id_nesting_flag = get_bits(gb, 1);

	if ((ret = parse_ptl(gb, &sps->ptl, sps->max_sub_layers)) < 0)
		return ret;

	*sps_id = get_ue_golomb_long(gb);
	if (*sps_id >= HEVC_MAX_SPS_COUNT) {
//		av_log(avctx, AV_LOG_ERROR, "SPS id out of range: %d\n", *sps_id);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	sps->chroma_format_idc = get_ue_golomb_long(gb);
	if (sps->chroma_format_idc > 3U) {
//		av_log(avctx, AV_LOG_ERROR, "chroma_format_idc %d is invalid\n", sps->chroma_format_idc);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	if (sps->chroma_format_idc == 3)
		sps->separate_colour_plane_flag = get_bits1(gb);

	if (sps->separate_colour_plane_flag)
		sps->chroma_format_idc = 0;

	sps->width  = get_ue_golomb_long(gb);
	sps->height = get_ue_golomb_long(gb);
//	if ((ret = av_image_check_size(sps->width,
//								   sps->height, 0, avctx)) < 0)
//		return ret;

	if (get_bits1(gb)) { // pic_conformance_flag
		int vert_mult  = hevc_sub_height_c[sps->chroma_format_idc];
		int horiz_mult = hevc_sub_width_c[sps->chroma_format_idc];
		sps->pic_conf_win.left_offset   = get_ue_golomb_long(gb) * horiz_mult;
		sps->pic_conf_win.right_offset  = get_ue_golomb_long(gb) * horiz_mult;
		sps->pic_conf_win.top_offset    = get_ue_golomb_long(gb) *  vert_mult;
		sps->pic_conf_win.bottom_offset = get_ue_golomb_long(gb) *  vert_mult;

		sps->output_window = sps->pic_conf_win;
	}

	sps->bit_depth   = get_ue_golomb_long(gb) + 8;
	bit_depth_chroma = get_ue_golomb_long(gb) + 8;
	if (sps->chroma_format_idc && bit_depth_chroma != sps->bit_depth) {
//		av_log(avctx, AV_LOG_ERROR,
//			   "Luma bit depth (%d) is different from chroma bit depth (%d), "
//			   "this is unsupported.\n",
//			   sps->bit_depth, bit_depth_chroma);
//		return AVERROR_INVALIDDATA;
		return -1;
	}
	sps->bit_depth_chroma = bit_depth_chroma;

	ret = map_pixel_format(sps);
	if (ret < 0)
		return ret;

	sps->log2_max_poc_lsb = get_ue_golomb_long(gb) + 4;
	if (sps->log2_max_poc_lsb > 16) {
//		av_log(avctx, AV_LOG_ERROR, "log2_max_pic_order_cnt_lsb_minus4 out range: %d\n",
//			   sps->log2_max_poc_lsb - 4);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	sublayer_ordering_info = get_bits1(gb);
	start = sublayer_ordering_info ? 0 : sps->max_sub_layers - 1;
	for (i = start; i < sps->max_sub_layers; i++) {
		sps->temporal_layer[i].max_dec_pic_buffering = get_ue_golomb_long(gb) + 1;
		sps->temporal_layer[i].num_reorder_pics      = get_ue_golomb_long(gb);
		sps->temporal_layer[i].max_latency_increase  = get_ue_golomb_long(gb) - 1;
		if (sps->temporal_layer[i].max_dec_pic_buffering > (unsigned)HEVC_MAX_DPB_SIZE) {
//			av_log(avctx, AV_LOG_ERROR, "sps_max_dec_pic_buffering_minus1 out of range: %d\n",
//				   sps->temporal_layer[i].max_dec_pic_buffering - 1U);
//			return AVERROR_INVALIDDATA;
			return -1;
		}
		if (sps->temporal_layer[i].num_reorder_pics > sps->temporal_layer[i].max_dec_pic_buffering - 1) {
			sps->temporal_layer[i].max_dec_pic_buffering = sps->temporal_layer[i].num_reorder_pics + 1;
		}
	}

	if (!sublayer_ordering_info) {
		for (i = 0; i < start; i++) {
			sps->temporal_layer[i].max_dec_pic_buffering = sps->temporal_layer[start].max_dec_pic_buffering;
			sps->temporal_layer[i].num_reorder_pics      = sps->temporal_layer[start].num_reorder_pics;
			sps->temporal_layer[i].max_latency_increase  = sps->temporal_layer[start].max_latency_increase;
		}
	}

	sps->log2_min_cb_size                    = get_ue_golomb_long(gb) + 3;
	sps->log2_diff_max_min_coding_block_size = get_ue_golomb_long(gb);
	sps->log2_min_tb_size                    = get_ue_golomb_long(gb) + 2;
	log2_diff_max_min_transform_block_size   = get_ue_golomb_long(gb);
	sps->log2_max_trafo_size                 = log2_diff_max_min_transform_block_size +
											   sps->log2_min_tb_size;

	if (sps->log2_min_cb_size < 3 || sps->log2_min_cb_size > 30) {
//		av_log(avctx, AV_LOG_ERROR, "Invalid value %d for log2_min_cb_size", sps->log2_min_cb_size);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	if (sps->log2_diff_max_min_coding_block_size > 30) {
//		av_log(avctx, AV_LOG_ERROR, "Invalid value %d for log2_diff_max_min_coding_block_size", sps->log2_diff_max_min_coding_block_size);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	if (sps->log2_min_tb_size >= sps->log2_min_cb_size || sps->log2_min_tb_size < 2) {
//		av_log(avctx, AV_LOG_ERROR, "Invalid value for log2_min_tb_size");
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	if (log2_diff_max_min_transform_block_size < 0 || log2_diff_max_min_transform_block_size > 30) {
//		av_log(avctx, AV_LOG_ERROR, "Invalid value %d for log2_diff_max_min_transform_block_size", log2_diff_max_min_transform_block_size);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	sps->max_transform_hierarchy_depth_inter = get_ue_golomb_long(gb);
	sps->max_transform_hierarchy_depth_intra = get_ue_golomb_long(gb);

	sps->scaling_list_enable_flag = get_bits1(gb);
	if (sps->scaling_list_enable_flag) {
		set_default_scaling_list_data(&sps->scaling_list);

		if (get_bits1(gb)) {
			ret = scaling_list_data(gb, &sps->scaling_list, sps);
			if (ret < 0)
				return ret;
		}
	}

	sps->amp_enabled_flag = get_bits1(gb);
	sps->sao_enabled      = get_bits1(gb);

	sps->pcm_enabled_flag = get_bits1(gb);
	if (sps->pcm_enabled_flag) {
		sps->pcm.bit_depth   = get_bits(gb, 4) + 1;
		sps->pcm.bit_depth_chroma = get_bits(gb, 4) + 1;
		sps->pcm.log2_min_pcm_cb_size = get_ue_golomb_long(gb) + 3;
		sps->pcm.log2_max_pcm_cb_size = sps->pcm.log2_min_pcm_cb_size +
										get_ue_golomb_long(gb);
		if (FFMAX(sps->pcm.bit_depth, sps->pcm.bit_depth_chroma) > sps->bit_depth) {
//			av_log(avctx, AV_LOG_ERROR,
//				   "PCM bit depth (%d, %d) is greater than normal bit depth (%d)\n",
//				   sps->pcm.bit_depth, sps->pcm.bit_depth_chroma, sps->bit_depth);
//			return AVERROR_INVALIDDATA;
			return -1;
		}

		sps->pcm.loop_filter_disable_flag = get_bits1(gb);
	}

	sps->nb_st_rps = get_ue_golomb_long(gb);
	if (sps->nb_st_rps > HEVC_MAX_SHORT_TERM_REF_PIC_SETS) {
//		av_log(avctx, AV_LOG_ERROR, "Too many short term RPS: %d.\n",
//			   sps->nb_st_rps);
//		return AVERROR_INVALIDDATA;
		return -1;
	}
	for (i = 0; i < sps->nb_st_rps; i++) {
		if ((ret = hevc_decode_short_term_rps(gb, &sps->st_rps[i],
												 sps, 0)) < 0)
			return ret;
	}

	sps->long_term_ref_pics_present_flag = get_bits1(gb);
	if (sps->long_term_ref_pics_present_flag) {
		sps->num_long_term_ref_pics_sps = get_ue_golomb_long(gb);
		if (sps->num_long_term_ref_pics_sps > HEVC_MAX_LONG_TERM_REF_PICS) {
//			av_log(avctx, AV_LOG_ERROR, "Too many long term ref pics: %d.\n",
//				   sps->num_long_term_ref_pics_sps);
//			return AVERROR_INVALIDDATA;
			return -1;
		}
		for (i = 0; i < sps->num_long_term_ref_pics_sps; i++) {
			sps->lt_ref_pic_poc_lsb_sps[i]       = get_bits(gb, sps->log2_max_poc_lsb);
			sps->used_by_curr_pic_lt_sps_flag[i] = get_bits1(gb);
		}
	}

	sps->sps_temporal_mvp_enabled_flag          = get_bits1(gb);
	sps->sps_strong_intra_smoothing_enable_flag = get_bits1(gb);
	sps->vui.sar = (Rational){0, 1};
	vui_present = get_bits1(gb);
	if (vui_present)
		decode_vui(gb, apply_defdispwin, sps);

	if (get_bits1(gb)) { // sps_extension_flag
		sps->sps_range_extension_flag = get_bits1(gb);
		skip_bits(gb, 7); //sps_extension_7bits = get_bits(gb, 7);
		if (sps->sps_range_extension_flag) {
			sps->transform_skip_rotation_enabled_flag = get_bits1(gb);
			sps->transform_skip_context_enabled_flag  = get_bits1(gb);
			sps->implicit_rdpcm_enabled_flag = get_bits1(gb);

			sps->explicit_rdpcm_enabled_flag = get_bits1(gb);

			sps->extended_precision_processing_flag = get_bits1(gb);
			if (sps->extended_precision_processing_flag)
//				av_log(avctx, AV_LOG_WARNING,
//					   "extended_precision_processing_flag not yet implemented\n");

			sps->intra_smoothing_disabled_flag       = get_bits1(gb);
			sps->high_precision_offsets_enabled_flag = get_bits1(gb);
//			if (sps->high_precision_offsets_enabled_flag)
//				av_log(avctx, AV_LOG_WARNING,
//					   "high_precision_offsets_enabled_flag not yet implemented\n");

			sps->persistent_rice_adaptation_enabled_flag = get_bits1(gb);

			sps->cabac_bypass_alignment_enabled_flag  = get_bits1(gb);
//			if (sps->cabac_bypass_alignment_enabled_flag)
//				av_log(avctx, AV_LOG_WARNING,
//					   "cabac_bypass_alignment_enabled_flag not yet implemented\n");
		}
	}
	if (apply_defdispwin) {
		sps->output_window.left_offset   += sps->vui.def_disp_win.left_offset;
		sps->output_window.right_offset  += sps->vui.def_disp_win.right_offset;
		sps->output_window.top_offset    += sps->vui.def_disp_win.top_offset;
		sps->output_window.bottom_offset += sps->vui.def_disp_win.bottom_offset;
	}

	ow = &sps->output_window;
	if (ow->left_offset >= INT_MAX - ow->right_offset     ||
		ow->top_offset  >= INT_MAX - ow->bottom_offset    ||
		ow->left_offset + ow->right_offset  >= sps->width ||
		ow->top_offset  + ow->bottom_offset >= sps->height) {
//		av_log(avctx, AV_LOG_WARNING, "Invalid cropping offsets: %u/%u/%u/%u\n",
//			   ow->left_offset, ow->right_offset, ow->top_offset, ow->bottom_offset);
//		if (avctx->err_recognition & AV_EF_EXPLODE) {
//			return AVERROR_INVALIDDATA;
//		}
//		av_log(avctx, AV_LOG_WARNING,
//			   "Displaying the whole video surface.\n");
		memset(ow, 0, sizeof(*ow));
		memset(&sps->pic_conf_win, 0, sizeof(sps->pic_conf_win));
	}

	// Inferred parameters
	sps->log2_ctb_size = sps->log2_min_cb_size +
						 sps->log2_diff_max_min_coding_block_size;
	sps->log2_min_pu_size = sps->log2_min_cb_size - 1;

	if (sps->log2_ctb_size > HEVC_MAX_LOG2_CTB_SIZE) {
//		av_log(avctx, AV_LOG_ERROR, "CTB size out of range: 2^%d\n", sps->log2_ctb_size);
//		return AVERROR_INVALIDDATA;
		return -1;
	}
	if (sps->log2_ctb_size < 4) {
//		av_log(avctx,
//			   AV_LOG_ERROR,
//			   "log2_ctb_size %d differs from the bounds of any known profile\n",
//			   sps->log2_ctb_size);
//		avpriv_request_sample(avctx, "log2_ctb_size %d", sps->log2_ctb_size);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	sps->ctb_width  = (sps->width  + (1 << sps->log2_ctb_size) - 1) >> sps->log2_ctb_size;
	sps->ctb_height = (sps->height + (1 << sps->log2_ctb_size) - 1) >> sps->log2_ctb_size;
	sps->ctb_size   = sps->ctb_width * sps->ctb_height;

	sps->min_cb_width  = sps->width  >> sps->log2_min_cb_size;
	sps->min_cb_height = sps->height >> sps->log2_min_cb_size;
	sps->min_tb_width  = sps->width  >> sps->log2_min_tb_size;
	sps->min_tb_height = sps->height >> sps->log2_min_tb_size;
	sps->min_pu_width  = sps->width  >> sps->log2_min_pu_size;
	sps->min_pu_height = sps->height >> sps->log2_min_pu_size;
	sps->tb_mask       = (1 << (sps->log2_ctb_size - sps->log2_min_tb_size)) - 1;

	sps->qp_bd_offset = 6 * (sps->bit_depth - 8);

	if (av_mod_uintp2(sps->width, sps->log2_min_cb_size) ||
		av_mod_uintp2(sps->height, sps->log2_min_cb_size)) {
//		av_log(avctx, AV_LOG_ERROR, "Invalid coded frame dimensions.\n");
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	if (sps->max_transform_hierarchy_depth_inter > sps->log2_ctb_size - sps->log2_min_tb_size) {
//		av_log(avctx, AV_LOG_ERROR, "max_transform_hierarchy_depth_inter out of range: %d\n",
//			   sps->max_transform_hierarchy_depth_inter);
//		return AVERROR_INVALIDDATA;
		return -1;
	}
	if (sps->max_transform_hierarchy_depth_intra > sps->log2_ctb_size - sps->log2_min_tb_size) {
//		av_log(avctx, AV_LOG_ERROR, "max_transform_hierarchy_depth_intra out of range: %d\n",
//			   sps->max_transform_hierarchy_depth_intra);
//		return AVERROR_INVALIDDATA;
		return -1;
	}
	if (sps->log2_max_trafo_size > FFMIN(sps->log2_ctb_size, 5)) {
//		av_log(avctx, AV_LOG_ERROR,
//			   "max transform block size out of range: %d\n",
//			   sps->log2_max_trafo_size);
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	if (get_bits_left(gb) < 0) {
//		av_log(avctx, AV_LOG_ERROR,
//			   "Overread SPS by %d bits\n", -get_bits_left(gb));
//		return AVERROR_INVALIDDATA;
		return -1;
	}

	return 0;
}

int HEVCDecodeNalSPS(const uint8_t *data, int size, HEVCParamSets *ps, int apply_defdispwin) {
	unsigned int sps_id;
	int ret;
	ptrdiff_t nal_size;
	HEVCSPS *sps = calloc(1, sizeof(HEVCSPS));

	GetBitContext gbc;
	GetBitContext * gb = &gbc;
	init_get_bits8(gb, data, size);

	nal_size = gb->buffer_end - gb->buffer;
	if (nal_size > sizeof(sps->data)) {
//		av_log(avctx, AV_LOG_WARNING, "Truncating likely oversized SPS "
//									  "(%"PTRDIFF_SPECIFIER" > %"SIZE_SPECIFIER")\n",
//				nal_size, sizeof(sps->data));
		sps->data_size = sizeof(sps->data);
	} else {
		sps->data_size = nal_size;
	}
	memcpy(sps->data, gb->buffer, sps->data_size);
	sps_id = show_bits(gb, 4);
	if (ps->sps_list[sps_id] && (memcmp(ps->sps_list[sps_id]->data, data, size) == 0)) {
		// sps is exist and is same
		return 0;
	}

	ret = hevc_parse_sps(sps, gb, &sps_id,
							apply_defdispwin,
							ps->vps_list);
	if (ret < 0) {
		free(sps);
		return ret;
	}

	if (ps->sps_list[sps_id])
		free(ps->sps_list[sps_id]);

	ps->sps_list[sps_id] = sps;

	return 0;
}

static int pps_range_extensions(GetBitContext *gb,
								HEVCPPS *pps, HEVCSPS *sps) {
	int i;

	if (pps->transform_skip_enabled_flag) {
		pps->log2_max_transform_skip_block_size = get_ue_golomb_long(gb) + 2;
	}
	pps->cross_component_prediction_enabled_flag = get_bits1(gb);
	pps->chroma_qp_offset_list_enabled_flag = get_bits1(gb);
	if (pps->chroma_qp_offset_list_enabled_flag) {
		pps->diff_cu_chroma_qp_offset_depth = get_ue_golomb_long(gb);
		pps->chroma_qp_offset_list_len_minus1 = get_ue_golomb_long(gb);
		if (pps->chroma_qp_offset_list_len_minus1 > 5) {
//			av_log(avctx, AV_LOG_ERROR,
//				   "chroma_qp_offset_list_len_minus1 shall be in the range [0, 5].\n");
			return -1;
		}
		for (i = 0; i <= pps->chroma_qp_offset_list_len_minus1; i++) {
			pps->cb_qp_offset_list[i] = get_se_golomb_long(gb);
			if (pps->cb_qp_offset_list[i]) {
//				av_log(avctx, AV_LOG_WARNING,
//					   "cb_qp_offset_list not tested yet.\n");
			}
			pps->cr_qp_offset_list[i] = get_se_golomb_long(gb);
			if (pps->cr_qp_offset_list[i]) {
//				av_log(avctx, AV_LOG_WARNING,
//					   "cb_qp_offset_list not tested yet.\n");
			}
		}
	}
	pps->log2_sao_offset_scale_luma = get_ue_golomb_long(gb);
	pps->log2_sao_offset_scale_chroma = get_ue_golomb_long(gb);

	if (   pps->log2_sao_offset_scale_luma   > FFMAX(sps->bit_depth        - 10, 0)
		   || pps->log2_sao_offset_scale_chroma > FFMAX(sps->bit_depth_chroma - 10, 0)
			)
		return -1;

	return(0);
}

static inline int setup_pps(GetBitContext *gb,
							HEVCPPS *pps, HEVCSPS *sps)
{
	int log2_diff;
	int pic_area_in_ctbs;
	int i, j, x, y, ctb_addr_rs, tile_id;

	// Inferred parameters
	pps->col_bd   = calloc(pps->num_tile_columns + 1, sizeof(*pps->col_bd));
	pps->row_bd   = calloc(pps->num_tile_rows + 1,    sizeof(*pps->row_bd));
	pps->col_idxX = calloc(sps->ctb_width,    sizeof(*pps->col_idxX));
	if (!pps->col_bd || !pps->row_bd || !pps->col_idxX)
		return -(ENOMEM);

	if (pps->uniform_spacing_flag) {
		if (!pps->column_width) {
			pps->column_width = calloc(pps->num_tile_columns, sizeof(*pps->column_width));
			pps->row_height   = calloc(pps->num_tile_rows,    sizeof(*pps->row_height));
		}
		if (!pps->column_width || !pps->row_height)
			return -(ENOMEM);

		for (i = 0; i < pps->num_tile_columns; i++) {
			pps->column_width[i] = ((i + 1) * sps->ctb_width) / pps->num_tile_columns -
								   (i * sps->ctb_width) / pps->num_tile_columns;
		}

		for (i = 0; i < pps->num_tile_rows; i++) {
			pps->row_height[i] = ((i + 1) * sps->ctb_height) / pps->num_tile_rows -
								 (i * sps->ctb_height) / pps->num_tile_rows;
		}
	}

	pps->col_bd[0] = 0;
	for (i = 0; i < pps->num_tile_columns; i++)
		pps->col_bd[i + 1] = pps->col_bd[i] + pps->column_width[i];

	pps->row_bd[0] = 0;
	for (i = 0; i < pps->num_tile_rows; i++)
		pps->row_bd[i + 1] = pps->row_bd[i] + pps->row_height[i];

	for (i = 0, j = 0; i < sps->ctb_width; i++) {
		if (i > pps->col_bd[j])
			j++;
		pps->col_idxX[i] = j;
	}

	/**
     * 6.5
     */
	pic_area_in_ctbs     = sps->ctb_width    * sps->ctb_height;

	pps->ctb_addr_rs_to_ts = calloc(pic_area_in_ctbs,    sizeof(*pps->ctb_addr_rs_to_ts));
	pps->ctb_addr_ts_to_rs = calloc(pic_area_in_ctbs,    sizeof(*pps->ctb_addr_ts_to_rs));
	pps->tile_id           = calloc(pic_area_in_ctbs,    sizeof(*pps->tile_id));
	pps->min_tb_addr_zs_tab = calloc((sps->tb_mask+2) * (sps->tb_mask+2), sizeof(*pps->min_tb_addr_zs_tab));
	if (!pps->ctb_addr_rs_to_ts || !pps->ctb_addr_ts_to_rs ||
		!pps->tile_id || !pps->min_tb_addr_zs_tab) {
		return -(ENOMEM);
	}

	for (ctb_addr_rs = 0; ctb_addr_rs < pic_area_in_ctbs; ctb_addr_rs++) {
		int tb_x   = ctb_addr_rs % sps->ctb_width;
		int tb_y   = ctb_addr_rs / sps->ctb_width;
		int tile_x = 0;
		int tile_y = 0;
		int val    = 0;

		for (i = 0; i < pps->num_tile_columns; i++) {
			if (tb_x < pps->col_bd[i + 1]) {
				tile_x = i;
				break;
			}
		}

		for (i = 0; i < pps->num_tile_rows; i++) {
			if (tb_y < pps->row_bd[i + 1]) {
				tile_y = i;
				break;
			}
		}

		for (i = 0; i < tile_x; i++)
			val += pps->row_height[tile_y] * pps->column_width[i];
		for (i = 0; i < tile_y; i++)
			val += sps->ctb_width * pps->row_height[i];

		val += (tb_y - pps->row_bd[tile_y]) * pps->column_width[tile_x] +
			   tb_x - pps->col_bd[tile_x];

		pps->ctb_addr_rs_to_ts[ctb_addr_rs] = val;
		pps->ctb_addr_ts_to_rs[val]         = ctb_addr_rs;
	}

	for (j = 0, tile_id = 0; j < pps->num_tile_rows; j++)
		for (i = 0; i < pps->num_tile_columns; i++, tile_id++)
			for (y = pps->row_bd[j]; y < pps->row_bd[j + 1]; y++)
				for (x = pps->col_bd[i]; x < pps->col_bd[i + 1]; x++)
					pps->tile_id[pps->ctb_addr_rs_to_ts[y * sps->ctb_width + x]] = tile_id;

	pps->tile_pos_rs = calloc(tile_id, sizeof(*pps->tile_pos_rs));
	if (!pps->tile_pos_rs)
		return -(ENOMEM);

	for (j = 0; j < pps->num_tile_rows; j++)
		for (i = 0; i < pps->num_tile_columns; i++)
			pps->tile_pos_rs[j * pps->num_tile_columns + i] =
					pps->row_bd[j] * sps->ctb_width + pps->col_bd[i];

	log2_diff = sps->log2_ctb_size - sps->log2_min_tb_size;
	pps->min_tb_addr_zs = &pps->min_tb_addr_zs_tab[1*(sps->tb_mask+2)+1];
	for (y = 0; y < sps->tb_mask+2; y++) {
		pps->min_tb_addr_zs_tab[y*(sps->tb_mask+2)] = -1;
		pps->min_tb_addr_zs_tab[y]    = -1;
	}
	for (y = 0; y < sps->tb_mask+1; y++) {
		for (x = 0; x < sps->tb_mask+1; x++) {
			int tb_x = x >> log2_diff;
			int tb_y = y >> log2_diff;
			int rs   = sps->ctb_width * tb_y + tb_x;
			int val  = pps->ctb_addr_rs_to_ts[rs] << (log2_diff * 2);
			for (i = 0; i < log2_diff; i++) {
				int m = 1 << i;
				val += (m & x ? m * m : 0) + (m & y ? 2 * m * m : 0);
			}
			pps->min_tb_addr_zs[y * (sps->tb_mask+2) + x] = val;
		}
	}

	return 0;
}

int HEVCDecodeNalPPS(const uint8_t *data, int size, HEVCParamSets *ps) {
	HEVCSPS *sps = NULL;
	int i, ret = 0;
	unsigned int pps_id = 0;
	ptrdiff_t nal_size;
	unsigned log2_parallel_merge_level_minus2;

	GetBitContext gbc;
	GetBitContext * gb = &gbc;
	init_get_bits8(gb, data, size);

	HEVCPPS *pps = calloc(1, sizeof(HEVCPPS));

	nal_size = gb->buffer_end - gb->buffer;
	if (nal_size > sizeof(pps->data)) {
		pps->data_size = sizeof(pps->data);
	} else {
		pps->data_size = nal_size;
	}
	memcpy(pps->data, gb->buffer, pps->data_size);
	// Default values
	pps->loop_filter_across_tiles_enabled_flag = 1;
	pps->num_tile_columns                      = 1;
	pps->num_tile_rows                         = 1;
	pps->uniform_spacing_flag                  = 1;
	pps->disable_dbf                           = 0;
	pps->beta_offset                           = 0;
	pps->tc_offset                             = 0;
	pps->log2_max_transform_skip_block_size    = 2;

	// Coded parameters
	pps_id = get_ue_golomb_long(gb);

	if (ps->pps_list[pps_id] && (memcmp(ps->pps_list[pps_id]->data, data, size) == 0)) {
		// pps is exist and is same
		return 0;
	}

	if (pps_id >= HEVC_MAX_PPS_COUNT) {
		ret = -1;
		goto err;
	}
	pps->sps_id = get_ue_golomb_long(gb);
	if (pps->sps_id >= HEVC_MAX_SPS_COUNT) {
		ret = -1;
		goto err;
	}
	if (!ps->sps_list[pps->sps_id]) {
		ret = -1;
		goto err;
	}
	sps = ps->sps_list[pps->sps_id];

	pps->dependent_slice_segments_enabled_flag = get_bits1(gb);
	pps->output_flag_present_flag              = get_bits1(gb);
	pps->num_extra_slice_header_bits           = get_bits(gb, 3);

	pps->sign_data_hiding_flag = get_bits1(gb);

	pps->cabac_init_present_flag = get_bits1(gb);

	pps->num_ref_idx_l0_default_active = get_ue_golomb_long(gb) + 1;
	pps->num_ref_idx_l1_default_active = get_ue_golomb_long(gb) + 1;

	pps->pic_init_qp_minus26 = get_se_golomb(gb);

	pps->constrained_intra_pred_flag = get_bits1(gb);
	pps->transform_skip_enabled_flag = get_bits1(gb);

	pps->cu_qp_delta_enabled_flag = get_bits1(gb);
	pps->diff_cu_qp_delta_depth   = 0;
	if (pps->cu_qp_delta_enabled_flag)
		pps->diff_cu_qp_delta_depth = get_ue_golomb_long(gb);

	if (pps->diff_cu_qp_delta_depth < 0 ||
		pps->diff_cu_qp_delta_depth > sps->log2_diff_max_min_coding_block_size) {
//		av_log(avctx, AV_LOG_ERROR, "diff_cu_qp_delta_depth %d is invalid\n",
//			   pps->diff_cu_qp_delta_depth);
		ret = -1;
		goto err;
	}

	pps->cb_qp_offset = get_se_golomb(gb);
	if (pps->cb_qp_offset < -12 || pps->cb_qp_offset > 12) {
//		av_log(avctx, AV_LOG_ERROR, "pps_cb_qp_offset out of range: %d\n",
//			   pps->cb_qp_offset);
		ret = -1;
		goto err;
	}
	pps->cr_qp_offset = get_se_golomb(gb);
	if (pps->cr_qp_offset < -12 || pps->cr_qp_offset > 12) {
//		av_log(avctx, AV_LOG_ERROR, "pps_cr_qp_offset out of range: %d\n",
//			   pps->cr_qp_offset);
		ret = -1;
		goto err;
	}
	pps->pic_slice_level_chroma_qp_offsets_present_flag = get_bits1(gb);

	pps->weighted_pred_flag   = get_bits1(gb);
	pps->weighted_bipred_flag = get_bits1(gb);

	pps->transquant_bypass_enable_flag    = get_bits1(gb);
	pps->tiles_enabled_flag               = get_bits1(gb);
	pps->entropy_coding_sync_enabled_flag = get_bits1(gb);

	if (pps->tiles_enabled_flag) {
		int num_tile_columns_minus1 = get_ue_golomb(gb);
		int num_tile_rows_minus1    = get_ue_golomb(gb);

		if (num_tile_columns_minus1 < 0 ||
			num_tile_columns_minus1 >= sps->ctb_width) {
//			av_log(avctx, AV_LOG_ERROR, "num_tile_columns_minus1 out of range: %d\n",
//				   num_tile_columns_minus1);
//			ret = num_tile_columns_minus1 < 0 ? num_tile_columns_minus1 : AVERROR_INVALIDDATA;
			ret = -1;
			goto err;
		}
		if (num_tile_rows_minus1 < 0 ||
			num_tile_rows_minus1 >= sps->ctb_height) {
//			av_log(avctx, AV_LOG_ERROR, "num_tile_rows_minus1 out of range: %d\n",
//				   num_tile_rows_minus1);
//			ret = num_tile_rows_minus1 < 0 ? num_tile_rows_minus1 : AVERROR_INVALIDDATA;
			ret = -1;
			goto err;
		}
		pps->num_tile_columns = num_tile_columns_minus1 + 1;
		pps->num_tile_rows    = num_tile_rows_minus1    + 1;

		pps->column_width = calloc(pps->num_tile_columns, sizeof(*pps->column_width));
		pps->row_height   = calloc(pps->num_tile_rows,    sizeof(*pps->row_height));
		if (!pps->column_width || !pps->row_height) {
			ret = -(ENOMEM);
			goto err;
		}

		pps->uniform_spacing_flag = get_bits1(gb);
		if (!pps->uniform_spacing_flag) {
			uint64_t sum = 0;
			for (i = 0; i < pps->num_tile_columns - 1; i++) {
				pps->column_width[i] = get_ue_golomb_long(gb) + 1;
				sum                 += pps->column_width[i];
			}
			if (sum >= sps->ctb_width) {
//				av_log(avctx, AV_LOG_ERROR, "Invalid tile widths.\n");
				ret = -1;
				goto err;
			}
			pps->column_width[pps->num_tile_columns - 1] = sps->ctb_width - sum;

			sum = 0;
			for (i = 0; i < pps->num_tile_rows - 1; i++) {
				pps->row_height[i] = get_ue_golomb_long(gb) + 1;
				sum               += pps->row_height[i];
			}
			if (sum >= sps->ctb_height) {
//				av_log(avctx, AV_LOG_ERROR, "Invalid tile heights.\n");
				ret = -1;
				goto err;
			}
			pps->row_height[pps->num_tile_rows - 1] = sps->ctb_height - sum;
		}
		pps->loop_filter_across_tiles_enabled_flag = get_bits1(gb);
	}

	pps->seq_loop_filter_across_slices_enabled_flag = get_bits1(gb);

	pps->deblocking_filter_control_present_flag = get_bits1(gb);
	if (pps->deblocking_filter_control_present_flag) {
		pps->deblocking_filter_override_enabled_flag = get_bits1(gb);
		pps->disable_dbf                             = get_bits1(gb);
		if (!pps->disable_dbf) {
			int beta_offset_div2 = get_se_golomb(gb);
			int tc_offset_div2   = get_se_golomb(gb) ;
			if (beta_offset_div2 < -6 || beta_offset_div2 > 6) {
//				av_log(avctx, AV_LOG_ERROR, "pps_beta_offset_div2 out of range: %d\n",
//					   beta_offset_div2);
				ret = -1;
				goto err;
			}
			if (tc_offset_div2 < -6 || tc_offset_div2 > 6) {
//				av_log(avctx, AV_LOG_ERROR, "pps_tc_offset_div2 out of range: %d\n",
//					   tc_offset_div2);
				ret = -1;
				goto err;
			}
			pps->beta_offset = 2 * beta_offset_div2;
			pps->tc_offset   = 2 *   tc_offset_div2;
		}
	}

	pps->scaling_list_data_present_flag = get_bits1(gb);
	if (pps->scaling_list_data_present_flag) {
		set_default_scaling_list_data(&pps->scaling_list);
		ret = scaling_list_data(gb, &pps->scaling_list, sps);
		if (ret < 0)
			goto err;
	}
	pps->lists_modification_present_flag = get_bits1(gb);
	log2_parallel_merge_level_minus2     = get_ue_golomb_long(gb);
	if (log2_parallel_merge_level_minus2 > sps->log2_ctb_size) {
//		av_log(avctx, AV_LOG_ERROR, "log2_parallel_merge_level_minus2 out of range: %d\n",
//			   log2_parallel_merge_level_minus2);
		ret = -1;
		goto err;
	}
	pps->log2_parallel_merge_level       = log2_parallel_merge_level_minus2 + 2;

	pps->slice_header_extension_present_flag = get_bits1(gb);

	if (get_bits1(gb)) { // pps_extension_present_flag
		pps->pps_range_extensions_flag = get_bits1(gb);
		skip_bits(gb, 7); // pps_extension_7bits
		if (sps->ptl.general_ptl.profile_idc == FF_PROFILE_HEVC_REXT && pps->pps_range_extensions_flag) {
			if ((ret = pps_range_extensions(gb, pps, sps)) < 0)
				goto err;
		}
	}

	ret = setup_pps(gb, pps, sps);
	if (ret < 0)
		goto err;

	if (get_bits_left(gb) < 0) {
//		av_log(avctx, AV_LOG_ERROR,
//			   "Overread PPS by %d bits\n", -get_bits_left(gb));
		goto err;
	}

	if (ps->pps_list[pps_id])
		free(ps->pps_list[pps_id]);

	ps->pps_list[pps_id] = pps;
	return 0;

err:
	free(pps);
	return ret;
}

static int hevc_compute_poc(const HEVCSPS *sps, int pocTid0, int poc_lsb, int nal_unit_type)
{
	int max_poc_lsb  = 1 << sps->log2_max_poc_lsb;
	int prev_poc_lsb = pocTid0 % max_poc_lsb;
	int prev_poc_msb = pocTid0 - prev_poc_lsb;
	int poc_msb;

	if (poc_lsb < prev_poc_lsb && prev_poc_lsb - poc_lsb >= max_poc_lsb / 2)
		poc_msb = prev_poc_msb + max_poc_lsb;
	else if (poc_lsb > prev_poc_lsb && poc_lsb - prev_poc_lsb > max_poc_lsb / 2)
		poc_msb = prev_poc_msb - max_poc_lsb;
	else
		poc_msb = prev_poc_msb;

	// For BLA picture types, POCmsb is set to 0.
	if (nal_unit_type == HEVC_NAL_BLA_W_LP   ||
		nal_unit_type == HEVC_NAL_BLA_W_RADL ||
		nal_unit_type == HEVC_NAL_BLA_N_LP)
		poc_msb = 0;

	return poc_msb + poc_lsb;
}

int HEVCParseSliceHeader(const uint8_t *data, int size, HEVCParserContext *ctx, H2645NAL* nal, SliceHeader *sh)
{
//	HEVCSEI *sei = &ctx->sei;
	HEVCParamSets* ps = &ctx->ps;
	const HEVCWindow *ow;
	int i, num = 0, den = 0;

    GetBitContext gbc;
    GetBitContext * gb = &gbc;
    init_get_bits8(gb, data, size);

	sh->first_slice_in_pic_flag = get_bits1(gb);
//	s->picture_structure = sei->picture_timing.picture_struct;
//	s->field_order = sei->picture_timing.picture_struct;

	if (IS_IRAP_NAL(nal)) {
//		s->key_frame = 1;
		sh->no_output_of_prior_pics_flag = get_bits1(gb);
	}

	sh->pps_id = get_ue_golomb(gb);
	if (sh->pps_id >= HEVC_MAX_PPS_COUNT || !ps->pps_list[sh->pps_id]) {
//		av_log(avctx, AV_LOG_ERROR, "PPS id out of range: %d\n", sh->pps_id);
		return -1;
	}
	ps->pps = (HEVCPPS*)ps->pps_list[sh->pps_id];

	if (ps->pps->sps_id >= HEVC_MAX_SPS_COUNT || !ps->sps_list[ps->pps->sps_id]) {
//		av_log(avctx, AV_LOG_ERROR, "SPS id out of range: %d\n", ps->pps->sps_id);
		return -1;
	}
	if (ps->sps != (HEVCSPS*)ps->sps_list[ps->pps->sps_id]) {
		ps->sps = (HEVCSPS*)ps->sps_list[ps->pps->sps_id];
		ps->vps = (HEVCVPS*)ps->vps_list[ps->sps->vps_id];
	}
	ow  = &ps->sps->output_window;

	ctx->coded_width  = ps->sps->width;
	ctx->coded_height = ps->sps->height;
	ctx->width        = ps->sps->width  - ow->left_offset - ow->right_offset;
	ctx->height       = ps->sps->height - ow->top_offset  - ow->bottom_offset;
//	ctx->format       = ps->sps->pix_fmt;
	ctx->profile  = ps->sps->ptl.general_ptl.profile_idc;
	ctx->level    = ps->sps->ptl.general_ptl.level_idc;

	if (ps->vps->vps_timing_info_present_flag) {
		num = ps->vps->vps_num_units_in_tick;
		den = ps->vps->vps_time_scale;
	} else if (ps->sps->vui.vui_timing_info_present_flag) {
		num = ps->sps->vui.vui_num_units_in_tick;
		den = ps->sps->vui.vui_time_scale;
	}

//	if (num != 0 && den != 0)
//		av_reduce(&avctx->framerate.den, &avctx->framerate.num,
//				  num, den, 1 << 30);

	if (!sh->first_slice_in_pic_flag) {
		int slice_address_length;

		if (ps->pps->dependent_slice_segments_enabled_flag)
			sh->dependent_slice_segment_flag = get_bits1(gb);
		else
			sh->dependent_slice_segment_flag = 0;

		slice_address_length = av_ceil_log2_c(ps->sps->ctb_width *
											  ps->sps->ctb_height);
		sh->slice_segment_addr = get_bitsz(gb, slice_address_length);
		if (sh->slice_segment_addr >= ps->sps->ctb_width * ps->sps->ctb_height) {
//			av_log(avctx, AV_LOG_ERROR, "Invalid slice segment address: %u.\n",
//				   sh->slice_segment_addr);
			return -1;
		}
	} else
		sh->dependent_slice_segment_flag = 0;

	if (sh->dependent_slice_segment_flag)
		return 0; /* break; */

	for (i = 0; i < ps->pps->num_extra_slice_header_bits; i++)
		skip_bits(gb, 1); // slice_reserved_undetermined_flag[]

	sh->slice_type = get_ue_golomb(gb);
	if (!(sh->slice_type == HEVC_SLICE_I || sh->slice_type == HEVC_SLICE_P ||
		  sh->slice_type == HEVC_SLICE_B)) {
//		av_log(avctx, AV_LOG_ERROR, "Unknown slice type: %d.\n",
//			   sh->slice_type);
		return -1;
	}
//	s->pict_type = sh->slice_type == HEVC_SLICE_B ? AV_PICTURE_TYPE_B :
//				   sh->slice_type == HEVC_SLICE_P ? AV_PICTURE_TYPE_P :
//				   AV_PICTURE_TYPE_I;

	if (ps->pps->output_flag_present_flag)
		sh->pic_output_flag = get_bits1(gb);

	if (ps->sps->separate_colour_plane_flag)
		sh->colour_plane_id = get_bits(gb, 2);

	if (!IS_IDR_NAL(nal)) {
		sh->pic_order_cnt_lsb = get_bits(gb, ps->sps->log2_max_poc_lsb);
		ctx->output_picture_number = ctx->poc = hevc_compute_poc(ps->sps, ctx->pocTid0, sh->pic_order_cnt_lsb, nal->type);
	} else
		ctx->output_picture_number = ctx->poc = 0;

	if (nal->temporal_id == 0 &&
			nal->type != HEVC_NAL_TRAIL_N &&
			nal->type != HEVC_NAL_TSA_N &&
			nal->type != HEVC_NAL_STSA_N &&
			nal->type != HEVC_NAL_RADL_N &&
			nal->type != HEVC_NAL_RASL_N &&
			nal->type != HEVC_NAL_RADL_R &&
			nal->type != HEVC_NAL_RASL_R)
		ctx->pocTid0 = ctx->poc;

	return 1; /* no need to evaluate the rest */
}

HEVCParserContext * CreateHEVCParserContext() {
	HEVCParserContext * ctx = calloc(1, sizeof(HEVCParserContext));
	HEVCParamSets *ps = &ctx->ps;
	memset(ps->vps_list, 0, HEVC_MAX_VPS_COUNT * sizeof(HEVCVPS *));
	memset(ps->sps_list, 0, HEVC_MAX_SPS_COUNT * sizeof(HEVCSPS *));
	memset(ps->pps_list, 0, HEVC_MAX_PPS_COUNT * sizeof(HEVCPPS *));
	ps->sps = NULL;
	ps->pps = NULL;
	ps->vps = NULL;
	return ctx;
}
void ReleaseHEVCParserContext(HEVCParserContext **p_ctx) {
	int i;
	HEVCParserContext *ctx = *p_ctx;
	if (ctx == NULL)
		return;

	HEVCParamSets *ps = &ctx->ps;
	for (i = 0; i < HEVC_MAX_VPS_COUNT; i++) {
		if (ps->vps_list[i])
			free(ps->vps_list[i]);
	}
	for (i = 0; i < HEVC_MAX_SPS_COUNT; i++) {
		if (ps->sps_list[i])
			free(ps->sps_list[i]);
	}
	for (i = 0; i < HEVC_MAX_PPS_COUNT; i++) {
		if (ps->pps_list[i])
			hevc_pps_free(&ps->pps_list[i]);
	}

	ps->sps = NULL;
	ps->pps = NULL;
	ps->vps = NULL;

	free(ctx);
	*p_ctx = NULL;
}


int HEVCDecodeExtradata(HEVCParserContext *ctx, const uint8_t *data, int size) {
	int ret = 0;
	FSIOContext fsio;
	fsio_init_context(&fsio, (void*)data, size, 0);
    if (size > 3 && (data[0] || data[1] || data[2] > 1)) {
        int i, j, num_arrays, nal_len_size;
        fsio_skip(&fsio, 21);
        nal_len_size = (fsio_r8(&fsio) & 3) + 1;
        num_arrays   = fsio_r8(&fsio);
        /* Decode nal units from hvcC. */
        for (i = 0; i < num_arrays; i++) {
            int type = fsio_r8(&fsio) & 0x3f;
            int cnt  = fsio_r8(&fsio);

            for (j = 0; j < cnt; j++) {
                // +2 for the nal size field
                int nalsize = fsio_rb16(&fsio);
                if (fsio_get_bytes_left(&fsio) < nalsize) {
                    return -1;
                }

				ret = HEVCParseSingleNalUnits(ctx, data + fsio_tell(&fsio), nalsize);

                fsio_skip(&fsio, nalsize);
            }
        }

    } else {
		ret = HEVCParseNalUnits(ctx, data, size, 0);
    }

	return ret;
}

static int nal_unit_extract_rbsp(const uint8_t *src, int src_len, uint8_t *dst)
{
    uint32_t i, len;

    /* NAL unit header (2 bytes) */
    i = len = 0;
    while (i < 2 && i < src_len)
        dst[len++] = src[i++];

    while (i + 2 < src_len)
        if (!src[i] && !src[i + 1] && src[i + 2] == 3) {
            dst[len++] = src[i++];
            dst[len++] = src[i++];
            i++; // remove emulation_prevention_three_byte
        }
        else
            dst[len++] = src[i++];

    while (i < src_len)
        dst[len++] = src[i++];

    return len;
}

int HEVCParseSingleNalUnits(HEVCParserContext *ctx, const uint8_t *buf, int buf_size) {
	HEVCParamSets *ps = &ctx->ps;
//	HEVCSEI *sei = &ctx->sei;
	int ret = -1;
    uint32_t rbsp_size;

	H2645NAL nal;
    
	HEVCParseNalHeader(&nal, (uint8_t*)buf, buf_size);
    
    rbsp_size = nal_unit_extract_rbsp(buf, MIN(buf_size, sizeof(ctx->rbsp)), ctx->rbsp);

    const uint8_t *nal_ptr = ctx->rbsp + 2;
    int nal_size = rbsp_size - 2;
    
//	if (ret == 0) {
		switch (nal.type) {
			case HEVC_NAL_VPS:
				HEVCDecodeNalVPS(nal_ptr, nal_size, &ctx->ps);
				break;
			case HEVC_NAL_SPS:
				HEVCDecodeNalSPS(nal_ptr, nal_size, &ctx->ps, 1);
				break;
			case HEVC_NAL_PPS:
				HEVCDecodeNalPPS(nal_ptr, nal_size, &ctx->ps);
				break;
			case HEVC_NAL_SEI_PREFIX:
			case HEVC_NAL_SEI_SUFFIX:
//				ff_hevc_decode_nal_sei(gb, avctx, sei, ps, nal->type);
				break;
			case HEVC_NAL_TRAIL_N:
			case HEVC_NAL_TRAIL_R:
			case HEVC_NAL_TSA_N:
			case HEVC_NAL_TSA_R:
			case HEVC_NAL_STSA_N:
			case HEVC_NAL_STSA_R:
			case HEVC_NAL_BLA_W_LP:
			case HEVC_NAL_BLA_W_RADL:
			case HEVC_NAL_BLA_N_LP:
			case HEVC_NAL_IDR_W_RADL:
			case HEVC_NAL_IDR_N_LP:
			case HEVC_NAL_CRA_NUT:
			case HEVC_NAL_RADL_N:
			case HEVC_NAL_RADL_R:
			case HEVC_NAL_RASL_N:
			case HEVC_NAL_RASL_R:
				ret = HEVCParseSliceHeader(nal_ptr, nal_size, ctx, &nal, &ctx->sh);
				break;
		}
//	}

	/* didn't find a picture! */
	return ret;
}

/**
 * Parse NAL units of found picture and decode some basic information.
 *
 * @param s parser context.
 * @param buf buffer with field/frame data.
 * @param buf_size size of the buffer.
 */

int HEVCParseNalUnits(HEVCParserContext *ctx, const uint8_t *buf,
					  int buf_size, int is_avcc)
{
	int ret = -1;
	if (is_avcc) {
		uint8_t *ptr = (uint8_t *)buf;
		int nalu_size = MEM_RB32(ptr);
		int nalu_size_total = nalu_size + 4;
		int offset = 0;
		while (nalu_size > 1 && offset + nalu_size_total <= buf_size) {
			if(HEVCParseSingleNalUnits(ctx, ptr + 4, nalu_size) >= 0) ret = 0;
			offset += nalu_size_total;
			ptr += nalu_size_total;
            nalu_size = MEM_RB32(ptr);
            nalu_size_total = nalu_size + 4;
		}
	} else {
		//parse annexb
		const uint8_t *endp = buf + buf_size;
		int find_nalu_header_size;
		int new_nalu_header_size;
		uint8_t *find_ptr = avc_find_startcode(buf, buf_size, &new_nalu_header_size);
		while (find_ptr) {
			find_nalu_header_size = new_nalu_header_size;
			uint8_t *startp = find_ptr + find_nalu_header_size + 1;
			uint8_t *p = avc_find_startcode(startp, endp - startp, &new_nalu_header_size);
            int nalu_len = p ? (p - find_ptr) : (endp - find_ptr);

			if(HEVCParseSingleNalUnits(ctx, find_ptr + find_nalu_header_size, nalu_len - find_nalu_header_size) >= 0) ret = 0;

			find_ptr = p;

		}
	}
	return ret;
}
