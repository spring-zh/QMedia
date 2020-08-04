//#include "general_platform.h"
#include "h264_parse.h"
#include <stdio.h>

#include "golomb.h"

#define MAX_LOG2_MAX_FRAME_NUM    (12 + 4)
#define MIN_LOG2_MAX_FRAME_NUM    4

#define EXTENDED_SAR       255

static const uint8_t default_scaling4[2][16] = {
	{ 6, 13, 20, 28, 13, 20, 28, 32,
	20, 28, 32, 37, 28, 32, 37, 42 },
	{ 10, 14, 20, 24, 14, 20, 24, 27,
	20, 24, 27, 30, 24, 27, 30, 34 }
};
static const uint8_t default_scaling8[2][64] = {
	{ 6, 10, 13, 16, 18, 23, 25, 27,
	10, 11, 16, 18, 23, 25, 27, 29,
	13, 16, 18, 23, 25, 27, 29, 31,
	16, 18, 23, 25, 27, 29, 31, 33,
	18, 23, 25, 27, 29, 31, 33, 36,
	23, 25, 27, 29, 31, 33, 36, 38,
	25, 27, 29, 31, 33, 36, 38, 40,
	27, 29, 31, 33, 36, 38, 40, 42 },
	{ 9, 13, 15, 17, 19, 21, 22, 24,
	13, 13, 17, 19, 21, 22, 24, 25,
	15, 17, 19, 21, 22, 24, 25, 27,
	17, 19, 21, 22, 24, 25, 27, 28,
	19, 21, 22, 24, 25, 27, 28, 30,
	21, 22, 24, 25, 27, 28, 30, 32,
	22, 24, 25, 27, 28, 30, 32, 33,
	24, 25, 27, 28, 30, 32, 33, 35 }
};

static const uint8_t zigzag_scan[16 + 1] = {
	0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
	1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
	1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
	3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};
const uint8_t zigzag_direct[64] = {
	0, 1, 8, 16, 9, 2, 3, 10,
	17, 24, 32, 25, 18, 11, 4, 5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13, 6, 7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

static const uint8_t golomb_to_pict_type[5] = {
	PICTURE_TYPE_P, PICTURE_TYPE_B, PICTURE_TYPE_I,
	PICTURE_TYPE_SP, PICTURE_TYPE_SI
};

#define QP(qP, depth) ((qP) + 6 * ((depth) - 8))

#define CHROMA_QP_TABLE_END(d)                                          \
	QP(0, d), QP(1, d), QP(2, d), QP(3, d), QP(4, d), QP(5, d), \
	QP(6, d), QP(7, d), QP(8, d), QP(9, d), QP(10, d), QP(11, d), \
	QP(12, d), QP(13, d), QP(14, d), QP(15, d), QP(16, d), QP(17, d), \
	QP(18, d), QP(19, d), QP(20, d), QP(21, d), QP(22, d), QP(23, d), \
	QP(24, d), QP(25, d), QP(26, d), QP(27, d), QP(28, d), QP(29, d), \
	QP(29, d), QP(30, d), QP(31, d), QP(32, d), QP(32, d), QP(33, d), \
	QP(34, d), QP(34, d), QP(35, d), QP(35, d), QP(36, d), QP(36, d), \
	QP(37, d), QP(37, d), QP(37, d), QP(38, d), QP(38, d), QP(38, d), \
	QP(39, d), QP(39, d), QP(39, d), QP(39, d)
const uint8_t h264_chroma_qp[7][QP_MAX_NUM + 1] = {
	{ CHROMA_QP_TABLE_END(8) },
	{ 0, 1, 2, 3, 4, 5,
	CHROMA_QP_TABLE_END(9) },
	{ 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11,
	CHROMA_QP_TABLE_END(10) },
	{ 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11,
	12, 13, 14, 15, 16, 17,
	CHROMA_QP_TABLE_END(11) },
	{ 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11,
	12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23,
	CHROMA_QP_TABLE_END(12) },
	{ 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11,
	12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23,
	24, 25, 26, 27, 28, 29,
	CHROMA_QP_TABLE_END(13) },
	{ 0, 1, 2, 3, 4, 5,
	6, 7, 8, 9, 10, 11,
	12, 13, 14, 15, 16, 17,
	18, 19, 20, 21, 22, 23,
	24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35,
	CHROMA_QP_TABLE_END(14) },
};

//uint8_t* avc_find_startcode(const uint8_t* inp, int size, int* nal_header_size)
//{
//	uint8_t* p = (uint8_t*)inp;
//	const uint8_t *end = p + size;
//	uint8_t *retp = NULL;
//	*nal_header_size = 3;
//	/*
//	const uint8_t *a = p + 4 - ((intptr_t)p & 3);
//	for (end -= 3; p < a && p < end; p++) {
//	if (p[0] == 0 && p[1] == 0 && p[2] == 1){
//	retp = p;
//	goto exit;
//	}
//	}
//	*/
//	for (end -= 3; p < end; p += 4) {
//		uint32_t x = *(const uint32_t*)p;
//		//      if ((x - 0x01000100) & (~x) & 0x80008000) // little endian
//		//      if ((x - 0x00010001) & (~x) & 0x00800080) // big endian
//		if ((x - 0x01010101) & (~x) & 0x80808080) { // generic
//			if (p[1] == 0) {
//				if (p[0] == 0 && p[2] == 1){
//					retp = p;
//					break;
//				}
//				if (p[2] == 0 && p[3] == 1){
//					retp = p + 1;
//					break;
//				}
//			}
//			if (p[3] == 0) {
//				if (p[2] == 0 && p[4] == 1){
//					retp = p + 2;
//					break;
//				}
//				if (p[4] == 0 && p[5] == 1){
//					retp = p + 3;
//					break;
//				}
//			}
//		}
//	}
//
//	if (inp < retp && retp < end && !retp[-1])
//	{
//		retp--;
//		*nal_header_size = 4;
//	}
//
//	return retp;
//}

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

static void h264_decode_annexb(uint8_t *dst, int *dstlen,
	const uint8_t *src, const int srclen)
{
	uint8_t *dst_sav = dst;
	const uint8_t *end = &src[srclen];

	while (src < end)
	{
		if (src < end - 3 && src[0] == 0x00 && src[1] == 0x00 &&
			src[2] == 0x03)
		{
			*dst++ = 0x00;
			*dst++ = 0x00;

			src += 3;
			continue;
		}
		*dst++ = *src++;
	}

	*dstlen = dst - dst_sav;
}

static inline int decode_hrd_parameters(GetBitContext *gb, SPS *sps)
{
	int cpb_count, i;
	cpb_count = get_ue_golomb_31(gb) + 1;

	if (cpb_count > 32U) {
		return ERROR_INVALIDDATA;
	}

	get_bits(gb, 4); /* bit_rate_scale */
	get_bits(gb, 4); /* cpb_size_scale */
	for (i = 0; i < cpb_count; i++) {
		get_ue_golomb_long(gb); /* bit_rate_value_minus1 */
		get_ue_golomb_long(gb); /* cpb_size_value_minus1 */
		get_bits1(gb);          /* cbr_flag */
	}
	sps->initial_cpb_removal_delay_length = get_bits(gb, 5) + 1;
	sps->cpb_removal_delay_length = get_bits(gb, 5) + 1;
	sps->dpb_output_delay_length = get_bits(gb, 5) + 1;
	sps->time_offset_length = get_bits(gb, 5);
	sps->cpb_cnt = cpb_count;
	return 0;
}

static inline int decode_vui_parameters(GetBitContext *gb, SPS *sps)
{
	int aspect_ratio_info_present_flag;
	unsigned int aspect_ratio_idc;
	int chroma_sample_location;

	aspect_ratio_info_present_flag = get_bits1(gb);

	if (aspect_ratio_info_present_flag) {
		aspect_ratio_idc = get_bits(gb, 8);
		if (aspect_ratio_idc == EXTENDED_SAR) {
			sps->sar.num = get_bits(gb, 16);
			sps->sar.den = get_bits(gb, 16);
		}
		else if (aspect_ratio_idc < ARRAY_ELEMS(h264_pixel_aspect)) {
			sps->sar = h264_pixel_aspect[aspect_ratio_idc];
		}
		else {
			return ERROR_INVALIDDATA;
		}
	}
	else {
		sps->sar.num =
			sps->sar.den = 0;
	}

	if (get_bits1(gb))      /* overscan_info_present_flag */
		get_bits1(gb);      /* overscan_appropriate_flag */

	sps->video_signal_type_present_flag = get_bits1(gb);
	if (sps->video_signal_type_present_flag) {
		get_bits(gb, 3);                 /* video_format */
		sps->full_range = get_bits1(gb); /* video_full_range_flag */

		sps->colour_description_present_flag = get_bits1(gb);
		if (sps->colour_description_present_flag) {
			sps->color_primaries = get_bits(gb, 8); /* colour_primaries */
			sps->color_trc = get_bits(gb, 8); /* transfer_characteristics */
			sps->colorspace = get_bits(gb, 8); /* matrix_coefficients */
			if (sps->color_primaries >= COL_PRI_NB)
				sps->color_primaries = COL_PRI_UNSPECIFIED;
			if (sps->color_trc >= COL_TRC_NB)
				sps->color_trc = COL_TRC_UNSPECIFIED;
			if (sps->colorspace >= COL_SPC_NB)
				sps->colorspace = COL_SPC_UNSPECIFIED;
		}
	}

	/* chroma_location_info_present_flag */
	if (get_bits1(gb)) {
		/* chroma_sample_location_type_top_field */
		chroma_sample_location = get_ue_golomb(gb) + 1;
		get_ue_golomb(gb);  /* chroma_sample_location_type_bottom_field */
	}

	if (show_bits1(gb) && get_bits_left(gb) < 10) {
		return 0;
	}

	sps->timing_info_present_flag = get_bits1(gb);
	if (sps->timing_info_present_flag) {
		unsigned num_units_in_tick = get_bits_long(gb, 32);
		unsigned time_scale = get_bits_long(gb, 32);
		if (!num_units_in_tick || !time_scale) {
			sps->timing_info_present_flag = 0;
		}
		else {
			sps->num_units_in_tick = num_units_in_tick;
			sps->time_scale = time_scale;
		}
		sps->fixed_frame_rate_flag = get_bits1(gb);
	}

	sps->nal_hrd_parameters_present_flag = get_bits1(gb);
	if (sps->nal_hrd_parameters_present_flag)
	if (decode_hrd_parameters(gb, sps) < 0)
		return ERROR_INVALIDDATA;
	sps->vcl_hrd_parameters_present_flag = get_bits1(gb);
	if (sps->vcl_hrd_parameters_present_flag)
	if (decode_hrd_parameters(gb, sps) < 0)
		return ERROR_INVALIDDATA;
	if (sps->nal_hrd_parameters_present_flag ||
		sps->vcl_hrd_parameters_present_flag)
		get_bits1(gb);     /* low_delay_hrd_flag */
	sps->pic_struct_present_flag = get_bits1(gb);
	if (!get_bits_left(gb))
		return 0;
	sps->bitstream_restriction_flag = get_bits1(gb);
	if (sps->bitstream_restriction_flag) {
		get_bits1(gb);     /* motion_vectors_over_pic_boundaries_flag */
		get_ue_golomb(gb); /* max_bytes_per_pic_denom */
		get_ue_golomb(gb); /* max_bits_per_mb_denom */
		get_ue_golomb(gb); /* log2_max_mv_length_horizontal */
		get_ue_golomb(gb); /* log2_max_mv_length_vertical */
		sps->num_reorder_frames = get_ue_golomb(gb);
		get_ue_golomb(gb); /*max_dec_frame_buffering*/

		if (get_bits_left(gb) < 0) {
			sps->num_reorder_frames = 0;
			sps->bitstream_restriction_flag = 0;
		}

		if (sps->num_reorder_frames > 16U
			/* max_dec_frame_buffering || max_dec_frame_buffering > 16 */) {
			sps->num_reorder_frames = 16;
			return ERROR_INVALIDDATA;
		}
	}

	return 0;
}
static void decode_scaling_list(GetBitContext *gb, uint8_t *factors, int size,
	const uint8_t *jvt_list,
	const uint8_t *fallback_list)
{
	int i, last = 8, next = 8;
	const uint8_t *scan = size == 16 ? zigzag_scan : zigzag_direct;
	if (!get_bits1(gb)) /* matrix not written, we use the predicted one */
		memcpy(factors, fallback_list, size * sizeof(uint8_t));
	else
	for (i = 0; i < size; i++) {
		if (next)
			next = (last + get_se_golomb(gb)) & 0xff;
		if (!i && !next) { /* matrix not written, we use the preset one */
			memcpy(factors, jvt_list, size * sizeof(uint8_t));
			break;
		}
		last = factors[scan[i]] = next ? next : last;
	}
}
static void decode_scaling_matrices(GetBitContext *gb,SPS *sps,
	PPS *pps, int is_sps,
	uint8_t(*scaling_matrix4)[16],
	uint8_t(*scaling_matrix8)[64])
{
	int fallback_sps = !is_sps && sps->scaling_matrix_present;
	const uint8_t *fallback[4] = {
		fallback_sps ? sps->scaling_matrix4[0] : default_scaling4[0],
		fallback_sps ? sps->scaling_matrix4[3] : default_scaling4[1],
		fallback_sps ? sps->scaling_matrix8[0] : default_scaling8[0],
		fallback_sps ? sps->scaling_matrix8[3] : default_scaling8[1]
	};
	if (get_bits1(gb)) {
		sps->scaling_matrix_present |= is_sps;
		decode_scaling_list(gb, scaling_matrix4[0], 16, default_scaling4[0], fallback[0]);        // Intra, Y
		decode_scaling_list(gb, scaling_matrix4[1], 16, default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
		decode_scaling_list(gb, scaling_matrix4[2], 16, default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
		decode_scaling_list(gb, scaling_matrix4[3], 16, default_scaling4[1], fallback[1]);        // Inter, Y
		decode_scaling_list(gb, scaling_matrix4[4], 16, default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
		decode_scaling_list(gb, scaling_matrix4[5], 16, default_scaling4[1], scaling_matrix4[4]); // Inter, Cb
		if (is_sps || pps->transform_8x8_mode) {
			decode_scaling_list(gb, scaling_matrix8[0], 64, default_scaling8[0], fallback[2]); // Intra, Y
			decode_scaling_list(gb, scaling_matrix8[3], 64, default_scaling8[1], fallback[3]); // Inter, Y
			if (sps->chroma_format_idc == 3) {
				decode_scaling_list(gb, scaling_matrix8[1], 64, default_scaling8[0], scaling_matrix8[0]); // Intra, Cr
				decode_scaling_list(gb, scaling_matrix8[4], 64, default_scaling8[1], scaling_matrix8[3]); // Inter, Cr
				decode_scaling_list(gb, scaling_matrix8[2], 64, default_scaling8[0], scaling_matrix8[1]); // Intra, Cb
				decode_scaling_list(gb, scaling_matrix8[5], 64, default_scaling8[1], scaling_matrix8[4]); // Inter, Cb
			}
		}
	}
}
int h264_decode_seq_parameter_set(uint8_t * buf, int size, SPS* sps, int ignore_truncation)
{
	int profile_idc, level_idc, constraint_set_flags = 0;
	unsigned int sps_id;
	int i, log2_max_frame_num_minus4;

	uint8_t tmp[512];
	int tmp_len;
	uint32_t header = 1;
	memset(sps, 0, sizeof(SPS));
	h264_decode_annexb(tmp, &tmp_len, buf + header, MIN(size - header, 512));

	GetBitContext gb;
	init_get_bits(&gb, tmp, tmp_len * 8);
	profile_idc = get_bits(&gb, 8);
	constraint_set_flags |= get_bits1(&gb) << 0;   // constraint_set0_flag
	constraint_set_flags |= get_bits1(&gb) << 1;   // constraint_set1_flag
	constraint_set_flags |= get_bits1(&gb) << 2;   // constraint_set2_flag
	constraint_set_flags |= get_bits1(&gb) << 3;   // constraint_set3_flag
	constraint_set_flags |= get_bits1(&gb) << 4;   // constraint_set4_flag
	constraint_set_flags |= get_bits1(&gb) << 5;   // constraint_set5_flag
	skip_bits(&gb, 2);                             // reserved_zero_2bits
	level_idc = get_bits(&gb, 8);
	sps_id = get_ue_golomb_31(&gb);

	if (sps_id >= MAX_SPS_COUNT) {
		return ERROR_INVALIDDATA;
	}
	/*sps = av_mallocz(sizeof(SPS));
	if (!sps)
		return -(ENOMEM);*/

	sps->sps_id = sps_id;
	sps->time_offset_length = 24;
	sps->profile_idc = profile_idc;
	sps->constraint_set_flags = constraint_set_flags;
	sps->level_idc = level_idc;
	sps->full_range = -1;

	memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
	memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
	sps->scaling_matrix_present = 0;
	sps->colorspace = 2; //AVCOL_SPC_UNSPECIFIED

	if (sps->profile_idc == 100 ||  // High profile
		sps->profile_idc == 110 ||  // High10 profile
		sps->profile_idc == 122 ||  // High422 profile
		sps->profile_idc == 244 ||  // High444 Predictive profile
		sps->profile_idc == 44 ||  // Cavlc444 profile
		sps->profile_idc == 83 ||  // Scalable Constrained High profile (SVC)
		sps->profile_idc == 86 ||  // Scalable High Intra profile (SVC)
		sps->profile_idc == 118 ||  // Stereo High profile (MVC)
		sps->profile_idc == 128 ||  // Multiview High profile (MVC)
		sps->profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
		sps->profile_idc == 144) {  // old High444 profile
		sps->chroma_format_idc = get_ue_golomb_31(&gb);
		if (sps->chroma_format_idc > 3U) {
			goto fail;
		}
		else if (sps->chroma_format_idc == 3) {
			sps->residual_color_transform_flag = get_bits1(&gb);
			if (sps->residual_color_transform_flag) {
				goto fail;
			}
		}
		sps->bit_depth_luma = get_ue_golomb(&gb) + 8;
		sps->bit_depth_chroma = get_ue_golomb(&gb) + 8;
		if (sps->bit_depth_chroma != sps->bit_depth_luma) {
			goto fail;
		}
		if (sps->bit_depth_luma   < 8 || sps->bit_depth_luma   > 14 ||
			sps->bit_depth_chroma < 8 || sps->bit_depth_chroma > 14) {
			goto fail;
		}
		sps->transform_bypass = get_bits1(&gb);
		decode_scaling_matrices(&gb, sps, NULL, 1,
			sps->scaling_matrix4, sps->scaling_matrix8);
	}
	else {
		sps->chroma_format_idc = 1;
		sps->bit_depth_luma = 8;
		sps->bit_depth_chroma = 8;
	}

	log2_max_frame_num_minus4 = get_ue_golomb(&gb);
	if (log2_max_frame_num_minus4 < MIN_LOG2_MAX_FRAME_NUM - 4 ||
		log2_max_frame_num_minus4 > MAX_LOG2_MAX_FRAME_NUM - 4) {
		goto fail;
	}
	sps->log2_max_frame_num = log2_max_frame_num_minus4 + 4;

	sps->poc_type = get_ue_golomb_31(&gb);

	if (sps->poc_type == 0) { // FIXME #define
		unsigned t = get_ue_golomb(&gb);
		if (t>12) {
			goto fail;
		}
		sps->log2_max_poc_lsb = t + 4;
	}
	else if (sps->poc_type == 1) { // FIXME #define
		sps->delta_pic_order_always_zero_flag = get_bits1(&gb);
		sps->offset_for_non_ref_pic = get_se_golomb(&gb);
		sps->offset_for_top_to_bottom_field = get_se_golomb(&gb);
		sps->poc_cycle_length = get_ue_golomb(&gb);

		if ((unsigned)sps->poc_cycle_length >=
			ARRAY_ELEMS(sps->offset_for_ref_frame)) {
			goto fail;
		}

		for (i = 0; i < sps->poc_cycle_length; i++)
			sps->offset_for_ref_frame[i] = get_se_golomb(&gb);
	}
	else if (sps->poc_type != 2) {
		goto fail;
	}

	sps->ref_frame_count = get_ue_golomb_31(&gb);
	if (sps->ref_frame_count > H264_MAX_PICTURE_COUNT - 2 ||
		sps->ref_frame_count > 16U) {
		goto fail;
	}
	sps->gaps_in_frame_num_allowed_flag = get_bits1(&gb);
	sps->mb_width = get_ue_golomb(&gb) + 1;
	sps->mb_height = get_ue_golomb(&gb) + 1;
	if ((unsigned)sps->mb_width >= INT_MAX / 16 ||
		(unsigned)sps->mb_height >= INT_MAX / 16 ) {
		goto fail;
	}

	sps->frame_mbs_only_flag = get_bits1(&gb);
	if (!sps->frame_mbs_only_flag)
		sps->mb_aff = get_bits1(&gb);
	else
		sps->mb_aff = 0;

	sps->direct_8x8_inference_flag = get_bits1(&gb);

	sps->crop = get_bits1(&gb);
	if (sps->crop) {
		unsigned int crop_left = get_ue_golomb(&gb);
		unsigned int crop_right = get_ue_golomb(&gb);
		unsigned int crop_top = get_ue_golomb(&gb);
		unsigned int crop_bottom = get_ue_golomb(&gb);
		int width = 16 * sps->mb_width;
		int height = 16 * sps->mb_height * (2 - sps->frame_mbs_only_flag);


		int vsub = (sps->chroma_format_idc == 1) ? 1 : 0;
		int hsub = (sps->chroma_format_idc == 1 ||
			sps->chroma_format_idc == 2) ? 1 : 0;
		int step_x = 1 << hsub;
		int step_y = (2 - sps->frame_mbs_only_flag) << vsub;

		if (crop_left & (0x1F >> (sps->bit_depth_luma > 8))) {
			crop_left &= ~(0x1F >> (sps->bit_depth_luma > 8));
		}

		if (crop_left  > (unsigned)INT_MAX / 4 / step_x ||
			crop_right > (unsigned)INT_MAX / 4 / step_x ||
			crop_top   > (unsigned)INT_MAX / 4 / step_y ||
			crop_bottom> (unsigned)INT_MAX / 4 / step_y ||
			(crop_left + crop_right) * step_x >= width ||
			(crop_top + crop_bottom) * step_y >= height
			) {
			goto fail;
		}

		sps->crop_left = crop_left   * step_x;
		sps->crop_right = crop_right  * step_x;
		sps->crop_top = crop_top    * step_y;
		sps->crop_bottom = crop_bottom * step_y;

	}
	else {
		sps->crop_left =
			sps->crop_right =
			sps->crop_top =
			sps->crop_bottom =
			sps->crop = 0;
	}

	sps->vui_parameters_present_flag = get_bits1(&gb);
	if (sps->vui_parameters_present_flag) {
		int ret = decode_vui_parameters(&gb, sps);
		if (ret < 0)
			goto fail;
	}

	if (get_bits_left(&gb) < 0) {
		if (!ignore_truncation)
			goto fail;
	}

	if (!sps->sar.den)
		sps->sar.den = 1;

	return 0;

fail:
	return ERROR_INVALIDDATA;
}

static void build_qp_table(PPS *pps, int t, int index, const int depth)
{
	int i;
	const int max_qp = 51 + 6 * (depth - 8);
	for (i = 0; i < max_qp + 1; i++)
		pps->chroma_qp_table[t][i] =
		h264_chroma_qp[depth - 8][av_clip(i + index, 0, max_qp)];
}

static int more_rbsp_data_in_pps(SPS *sps, PPS *pps)
{
	int profile_idc = sps->profile_idc;

	if ((profile_idc == 66 || profile_idc == 77 ||
		profile_idc == 88) && (sps->constraint_set_flags & 7)) {
		return 0;
	}

	return 1;
}
int h264_decode_picture_parameter_set(uint8_t * buf, int size, SPS *sps, PPS *pps, int bit_length)
{
	int qp_bd_offset;
	int bits_left;
	int ret;
	uint8_t tmp[512];
	int tmp_len;
	uint32_t header = 1;

	memset(pps, 0, sizeof(PPS));
	h264_decode_annexb(tmp, &tmp_len, buf + header, MIN(size - header, 512));

	GetBitContext gb;
	init_get_bits(&gb, tmp, tmp_len * 8);
	unsigned int pps_id = get_ue_golomb(&gb);

	if (pps_id >= MAX_PPS_COUNT) {
		return ERROR_INVALIDDATA;
	}

	pps->sps_id = get_ue_golomb_31(&gb);
	if ((unsigned)pps->sps_id >= MAX_SPS_COUNT) {
		ret = ERROR_INVALIDDATA;
		goto fail;
	}
	/*sps = h->sps_buffers[pps->sps_id];*/
	if (sps->bit_depth_luma > 14) {
		ret = ERROR_INVALIDDATA;
		goto fail;
	}
	else if (sps->bit_depth_luma == 11 || sps->bit_depth_luma == 13) {
		ret = ERROR_PATCHWELCOME;
		goto fail;
	}

	pps->cabac = get_bits1(&gb);
	pps->pic_order_present = get_bits1(&gb);
	pps->slice_group_count = get_ue_golomb(&gb) + 1;
	if (pps->slice_group_count > 1) {
		pps->mb_slice_group_map_type = get_ue_golomb(&gb);
		switch (pps->mb_slice_group_map_type) {
		case 0:
#if 0
			| for (i = 0; i <= num_slice_groups_minus1; i++) | | |
				| run_length[i] | 1 | ue(v) |
#endif
				break;
		case 2:
#if 0
			| for (i = 0; i < num_slice_groups_minus1; i++) {
				| | |
					| top_left_mb[i] | 1 | ue(v) |
					| bottom_right_mb[i] | 1 | ue(v) |
					|
			} | | |
#endif
			break;
		case 3:
		case 4:
		case 5:
#if 0
			| slice_group_change_direction_flag | 1 | u(1) |
				| slice_group_change_rate_minus1 | 1 | ue(v) |
#endif
				break;
		case 6:
#if 0
			| slice_group_id_cnt_minus1 | 1 | ue(v) |
				| for (i = 0; i <= slice_group_id_cnt_minus1; i++) | | |
				| slice_group_id[i] | 1 | u(v) |
#endif
				break;
		}
	}
	pps->ref_count[0] = get_ue_golomb(&gb) + 1;
	pps->ref_count[1] = get_ue_golomb(&gb) + 1;
	if (pps->ref_count[0] - 1 > 32 - 1 || pps->ref_count[1] - 1 > 32 - 1) {
		ret = ERROR_INVALIDDATA;
		goto fail;
	}

	qp_bd_offset = 6 * (sps->bit_depth_luma - 8);

	pps->weighted_pred = get_bits1(&gb);
	pps->weighted_bipred_idc = get_bits(&gb, 2);
	pps->init_qp = get_se_golomb(&gb) + 26 + qp_bd_offset;
	pps->init_qs = get_se_golomb(&gb) + 26 + qp_bd_offset;
	pps->chroma_qp_index_offset[0] = get_se_golomb(&gb);
	pps->deblocking_filter_parameters_present = get_bits1(&gb);
	pps->constrained_intra_pred = get_bits1(&gb);
	pps->redundant_pic_cnt_present = get_bits1(&gb);

	pps->transform_8x8_mode = 0;
	// contents of sps/pps can change even if id doesn't, so reinit
	//h->dequant_coeff_pps = -1;
	/*memcpy(pps->scaling_matrix4, h->sps_buffers[pps->sps_id]->scaling_matrix4,
		sizeof(pps->scaling_matrix4));
	memcpy(pps->scaling_matrix8, h->sps_buffers[pps->sps_id]->scaling_matrix8,
		sizeof(pps->scaling_matrix8));*/
	memcpy(pps->scaling_matrix4, sps->scaling_matrix4,
	sizeof(pps->scaling_matrix4));
	memcpy(pps->scaling_matrix8, sps->scaling_matrix8,
	sizeof(pps->scaling_matrix8));

	bits_left = bit_length - get_bits_count(&gb);
	if (bits_left > 0 && more_rbsp_data_in_pps(sps, pps)) {
		pps->transform_8x8_mode = get_bits1(&gb);
		decode_scaling_matrices(&gb, sps, pps, 0,
			pps->scaling_matrix4, pps->scaling_matrix8);
		// second_chroma_qp_index_offset
		pps->chroma_qp_index_offset[1] = get_se_golomb(&gb);
	}
	else {
		pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
	}

	build_qp_table(pps, 0, pps->chroma_qp_index_offset[0],
		sps->bit_depth_luma);
	build_qp_table(pps, 1, pps->chroma_qp_index_offset[1],
		sps->bit_depth_luma);
	if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
		pps->chroma_qp_diff = 1;

	return 0;

fail:
	return ret;
}

int h264_decode_slice(uint8_t * buf, int size, Slice *slice, int log2_max_frame_num/*in sps*/)
{
	uint32_t header = 1;
	uint8_t tmp[512];
	int tmp_len;
	memset(slice, 0, sizeof(Slice));
	h264_decode_annexb(tmp, &tmp_len, buf + header, MIN(size - header, 512));

	GetBitContext gb;
	init_get_bits(&gb, tmp, tmp_len * 8);
	
	slice->first_mb_in_slice = get_ue_golomb_long(&gb);
	slice->slice_type = get_ue_golomb_31(&gb);
	slice->pict_type = golomb_to_pict_type[slice->slice_type % 5];
	//if (h->sei_recovery_frame_cnt >= 0) {
	//	/* key frame, since recovery_frame_cnt is set */
	//	s->key_frame = 1;
	//}
	slice->pic_parameter_set_id = get_ue_golomb(&gb);
	if (slice->pic_parameter_set_id >= MAX_PPS_COUNT) {
		return -1;
	}
	slice->frame_num = get_bits(&gb, log2_max_frame_num);

	//...
	return 0;
}

char *h264_get_profile_level_string(const uint8_t profile,
	const uint8_t level)
{
	const char *pro;
	char profileb[20], levelb[20];
	if (profile == 66) {
		pro = "Baseline";
	}
	else if (profile == 77) {
		pro = "Main";
	}
	else if (profile == 88) {
		pro = "Extended";
	}
	else if (profile == 100) {
		pro = "High";
	}
	else if (profile == 110) {
		pro = "High 10";
	}
	else if (profile == 122) {
		pro = "High 4:2:2";
	}
	else if (profile == 144) {
		pro = "High 4:4:4";
	}
	else {
		snprintf(profileb, sizeof(profileb), "Unknown Profile %x", profile);
		pro = profileb;
	}
	switch (level) {
	case 10: case 20: case 30: case 40: case 50:
		snprintf(levelb, sizeof(levelb), "%u", level / 10);
		break;
	case 11: case 12: case 13:
	case 21: case 22:
	case 31: case 32:
	case 41: case 42:
	case 51:
		snprintf(levelb, sizeof(levelb), "%u.%u", level / 10, level % 10);
		break;
	default:
		snprintf(levelb, sizeof(levelb), "unknown level %x", level);
		break;
	}
	uint32_t len =
		1 + strlen("H.264 @") + strlen(pro) + strlen(levelb);
	char *typebuffer =
		(char *)malloc(len);
	if (typebuffer == NULL) return NULL;

	snprintf(typebuffer, len, "H.264 %s@%s", pro, levelb);
	return typebuffer;
}
