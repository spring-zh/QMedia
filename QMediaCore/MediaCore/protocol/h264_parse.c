
#include "h264_parse.h"
#include <stdio.h>

#include "golomb.h"

extern uint8_t* avc_find_startcode(const uint8_t* inp, int size, int* nal_header_size);

#define MEM_RB8(ptr) (*(ptr))
#define MEM_RB16(ptr) ((MEM_RB8(ptr) << 8)|MEM_RB8(ptr+1))
#define MEM_RB24(ptr) ((MEM_RB16(ptr)<<8)|MEM_RB8(ptr+2))
#define MEM_RB32(ptr) ((MEM_RB16(ptr)<<16)|MEM_RB16(ptr+2))
#define MEM_RB64(ptr) (((uint64_t)MEM_RB32(ptr)<<32)|MEM_RB32(ptr+4))

#define MIN_LOG2_MAX_FRAME_NUM    4

#define EXTENDED_SAR       255

#define FF_ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))

#define H264_MAX_PICTURE_COUNT 36

#define MAX_MMCO_COUNT         66

#define MAX_DELAYED_PIC_COUNT  16

#define FFMAX(a,b) ((a) > (b) ? (a) : (b))
#define FFMIN(a,b) ((a) > (b) ? (b) : (a))

#define PROFILE_H264_CONSTRAINED  (1<<9)  // 8+1; constraint_set1_flag
#define PROFILE_H264_INTRA        (1<<11) // 8+3; constraint_set3_flag

#define PROFILE_H264_BASELINE             66
#define PROFILE_H264_CONSTRAINED_BASELINE (66|PROFILE_H264_CONSTRAINED)
#define PROFILE_H264_MAIN                 77
#define PROFILE_H264_EXTENDED             88
#define PROFILE_H264_HIGH                 100
#define PROFILE_H264_HIGH_10              110
#define PROFILE_H264_HIGH_10_INTRA        (110|PROFILE_H264_INTRA)
#define PROFILE_H264_MULTIVIEW_HIGH       118
#define PROFILE_H264_HIGH_422             122
#define PROFILE_H264_HIGH_422_INTRA       (122|PROFILE_H264_INTRA)
#define PROFILE_H264_STEREO_HIGH          128
#define PROFILE_H264_HIGH_444             144
#define PROFILE_H264_HIGH_444_PREDICTIVE  244
#define PROFILE_H264_HIGH_444_INTRA       (244|PROFILE_H264_INTRA)
#define PROFILE_H264_CAVLC_444

/* picture type */
#define PICT_TOP_FIELD     1
#define PICT_BOTTOM_FIELD  2
#define PICT_FRAME         3

/**
 * Memory management control operation opcode.
 */
typedef enum MMCOOpcode {
    MMCO_END = 0,
    MMCO_SHORT2UNUSED,
    MMCO_LONG2UNUSED,
    MMCO_SHORT2LONG,
    MMCO_SET_MAX_LONG,
    MMCO_RESET,
    MMCO_LONG,
} MMCOOpcode;

/**
 * Memory management control operation.
 */
typedef struct MMCO {
    MMCOOpcode opcode;
    int short_pic_num;  ///< pic_num without wrapping (pic_num & max_pic_num)
    int long_arg;       ///< index, pic_num, or num long refs depending on opcode
} MMCO;

typedef struct H264PredWeightTable {
    int use_weight;
    int use_weight_chroma;
    int luma_log2_weight_denom;
    int chroma_log2_weight_denom;
    int luma_weight_flag[2];    ///< 7.4.3.2 luma_weight_lX_flag
    int chroma_weight_flag[2];  ///< 7.4.3.2 chroma_weight_lX_flag
    // The following 2 can be changed to int8_t but that causes a 10 CPU cycles speed loss
    int luma_weight[48][2][2];
    int chroma_weight[48][2][2][2];
    int implicit_weight[48][48][2];
} H264PredWeightTable;

static const uint8_t default_scaling4[2][16] = {
        {  6, 13, 20, 28, 13, 20, 28, 32,
                20, 28, 32, 37, 28, 32, 37, 42 },
        { 10, 14, 20, 24, 14, 20, 24, 27,
                20, 24, 27, 30, 24, 27, 30, 34 }
};

static const uint8_t default_scaling8[2][64] = {
        {  6, 10, 13, 16, 18, 23, 25, 27,
                10, 11, 16, 18, 23, 25, 27, 29,
                13, 16, 18, 23, 25, 27, 29, 31,
                16, 18, 23, 25, 27, 29, 31, 33,
                18, 23, 25, 27, 29, 31, 33, 36,
                23, 25, 27, 29, 31, 33, 36, 38,
                25, 27, 29, 31, 33, 36, 38, 40,
                27, 29, 31, 33, 36, 38, 40, 42 },
        {  9, 13, 15, 17, 19, 21, 22, 24,
                13, 13, 17, 19, 21, 22, 24, 25,
                15, 17, 19, 21, 22, 24, 25, 27,
                17, 19, 21, 22, 24, 25, 27, 28,
                19, 21, 22, 24, 25, 27, 28, 30,
                21, 22, 24, 25, 27, 28, 30, 32,
                22, 24, 25, 27, 28, 30, 32, 33,
                24, 25, 27, 28, 30, 32, 33, 35 }
};

const uint8_t ff_zigzag_direct[64] = {
        0,   1,  8, 16,  9,  2,  3, 10,
        17, 24, 32, 25, 18, 11,  4,  5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13,  6,  7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63
};

const uint8_t ff_zigzag_scan[16+1] = {
        0 + 0 * 4, 1 + 0 * 4, 0 + 1 * 4, 0 + 2 * 4,
        1 + 1 * 4, 2 + 0 * 4, 3 + 0 * 4, 2 + 1 * 4,
        1 + 2 * 4, 0 + 3 * 4, 1 + 3 * 4, 2 + 2 * 4,
        3 + 1 * 4, 3 + 2 * 4, 2 + 3 * 4, 3 + 3 * 4,
};

static const H264Rational ff_h264_pixel_aspect[17] = {
        {   0,  1 },
        {   1,  1 },
        {  12, 11 },
        {  10, 11 },
        {  16, 11 },
        {  40, 33 },
        {  24, 11 },
        {  20, 11 },
        {  32, 11 },
        {  80, 33 },
        {  18, 11 },
        {  15, 11 },
        {  64, 33 },
        { 160, 99 },
        {   4,  3 },
        {   3,  2 },
        {   2,  1 },
};

/* maximum number of MBs in the DPB for a given level */
static const int level_max_dpb_mbs[][2] = {
        { 10, 396       },
        { 11, 900       },
        { 12, 2376      },
        { 13, 2376      },
        { 20, 2376      },
        { 21, 4752      },
        { 22, 8100      },
        { 30, 8100      },
        { 31, 18000     },
        { 32, 20480     },
        { 40, 32768     },
        { 41, 32768     },
        { 42, 34816     },
        { 50, 110400    },
        { 51, 184320    },
        { 52, 184320    },
};

#define QP(qP, depth) ((qP) + 6 * ((depth) - 8))

#define CHROMA_QP_TABLE_END(d)                                          \
    QP(0,  d), QP(1,  d), QP(2,  d), QP(3,  d), QP(4,  d), QP(5,  d),   \
    QP(6,  d), QP(7,  d), QP(8,  d), QP(9,  d), QP(10, d), QP(11, d),   \
    QP(12, d), QP(13, d), QP(14, d), QP(15, d), QP(16, d), QP(17, d),   \
    QP(18, d), QP(19, d), QP(20, d), QP(21, d), QP(22, d), QP(23, d),   \
    QP(24, d), QP(25, d), QP(26, d), QP(27, d), QP(28, d), QP(29, d),   \
    QP(29, d), QP(30, d), QP(31, d), QP(32, d), QP(32, d), QP(33, d),   \
    QP(34, d), QP(34, d), QP(35, d), QP(35, d), QP(36, d), QP(36, d),   \
    QP(37, d), QP(37, d), QP(37, d), QP(38, d), QP(38, d), QP(38, d),   \
    QP(39, d), QP(39, d), QP(39, d), QP(39, d)

const uint8_t ff_h264_chroma_qp[7][QP_MAX_NUM + 1] = {
        { CHROMA_QP_TABLE_END(8) },
        { 0, 1, 2, 3, 4, 5,
                CHROMA_QP_TABLE_END(9) },
        { 0, 1, 2, 3,  4,  5,
                6, 7, 8, 9, 10, 11,
                CHROMA_QP_TABLE_END(10) },
        { 0,  1, 2, 3,  4,  5,
                6,  7, 8, 9, 10, 11,
                12,13,14,15, 16, 17,
                CHROMA_QP_TABLE_END(11) },
        { 0,  1, 2, 3,  4,  5,
                6,  7, 8, 9, 10, 11,
                12,13,14,15, 16, 17,
                18,19,20,21, 22, 23,
                CHROMA_QP_TABLE_END(12) },
        { 0,  1, 2, 3,  4,  5,
                6,  7, 8, 9, 10, 11,
                12,13,14,15, 16, 17,
                18,19,20,21, 22, 23,
                24,25,26,27, 28, 29,
                CHROMA_QP_TABLE_END(13) },
        { 0,  1, 2, 3,  4,  5,
                6,  7, 8, 9, 10, 11,
                12,13,14,15, 16, 17,
                18,19,20,21, 22, 23,
                24,25,26,27, 28, 29,
                30,31,32,33, 34, 35,
                CHROMA_QP_TABLE_END(14) },
};

const uint8_t ff_h264_dequant4_coeff_init[6][3] = {
        { 10, 13, 16 },
        { 11, 14, 18 },
        { 13, 16, 20 },
        { 14, 18, 23 },
        { 16, 20, 25 },
        { 18, 23, 29 },
};

const uint8_t ff_h264_dequant8_coeff_init_scan[16] = {
        0, 3, 4, 3, 3, 1, 5, 1, 4, 5, 2, 5, 3, 1, 5, 1
};

const uint8_t ff_h264_dequant8_coeff_init[6][6] = {
        { 20, 18, 32, 19, 25, 24 },
        { 22, 19, 35, 21, 28, 26 },
        { 26, 23, 42, 24, 33, 31 },
        { 28, 25, 45, 26, 35, 33 },
        { 32, 28, 51, 30, 40, 38 },
        { 36, 32, 58, 34, 46, 43 },
};

const uint8_t ff_h264_quant_rem6[QP_MAX_NUM + 1] = {
        0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2,
        3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5,
        0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2,
        3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5,
        0, 1, 2, 3,
};

const uint8_t ff_h264_quant_div6[QP_MAX_NUM + 1] = {
        0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3,  3,  3,
        3, 3, 3, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6,  6,  6,
        7, 7, 7, 7, 7, 7, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 10, 10, 10,
        10,10,10,11,11,11,11,11,11,12,12,12,12,12,12,13,13,13, 13, 13, 13,
        14,14,14,14,
};

const uint8_t ff_h264_golomb_to_pict_type[5] = {
        H264_PICTURE_TYPE_P, H264_PICTURE_TYPE_B, H264_PICTURE_TYPE_I,
        H264_PICTURE_TYPE_SP, H264_PICTURE_TYPE_SI
};

static int decode_scaling_list(GetBitContext *gb, uint8_t *factors, int size,
                               const uint8_t *jvt_list,
                               const uint8_t *fallback_list)
{
    int i, last = 8, next = 8;
    const uint8_t *scan = size == 16 ? ff_zigzag_scan : ff_zigzag_direct;
    if (!get_bits1(gb)) /* matrix not written, we use the predicted one */
        memcpy(factors, fallback_list, size * sizeof(uint8_t));
    else
        for (i = 0; i < size; i++) {
            if (next) {
                int v = get_se_golomb(gb);
                if (v < -128 || v > 127) {
//                    av_log(NULL, AV_LOG_ERROR, "delta scale %d is invalid\n", v);
                    return -1;
                }
                next = (last + v) & 0xff;
            }
            if (!i && !next) { /* matrix not written, we use the preset one */
                memcpy(factors, jvt_list, size * sizeof(uint8_t));
                break;
            }
            last = factors[scan[i]] = next ? next : last;
        }
    return 0;
}

/* returns non zero if the provided SPS scaling matrix has been filled */
static int decode_scaling_matrices(GetBitContext *gb, const SPS *sps,
                                   const PPS *pps, int is_sps,
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
    int ret = 0;
    if (get_bits1(gb)) {
        ret |= decode_scaling_list(gb, scaling_matrix4[0], 16, default_scaling4[0], fallback[0]);        // Intra, Y
        ret |= decode_scaling_list(gb, scaling_matrix4[1], 16, default_scaling4[0], scaling_matrix4[0]); // Intra, Cr
        ret |= decode_scaling_list(gb, scaling_matrix4[2], 16, default_scaling4[0], scaling_matrix4[1]); // Intra, Cb
        ret |= decode_scaling_list(gb, scaling_matrix4[3], 16, default_scaling4[1], fallback[1]);        // Inter, Y
        ret |= decode_scaling_list(gb, scaling_matrix4[4], 16, default_scaling4[1], scaling_matrix4[3]); // Inter, Cr
        ret |= decode_scaling_list(gb, scaling_matrix4[5], 16, default_scaling4[1], scaling_matrix4[4]); // Inter, Cb
        if (is_sps || pps->transform_8x8_mode) {
            ret |= decode_scaling_list(gb, scaling_matrix8[0], 64, default_scaling8[0], fallback[2]); // Intra, Y
            ret |= decode_scaling_list(gb, scaling_matrix8[3], 64, default_scaling8[1], fallback[3]); // Inter, Y
            if (sps->chroma_format_idc == 3) {
                ret |= decode_scaling_list(gb, scaling_matrix8[1], 64, default_scaling8[0], scaling_matrix8[0]); // Intra, Cr
                ret |= decode_scaling_list(gb, scaling_matrix8[4], 64, default_scaling8[1], scaling_matrix8[3]); // Inter, Cr
                ret |= decode_scaling_list(gb, scaling_matrix8[2], 64, default_scaling8[0], scaling_matrix8[1]); // Intra, Cb
                ret |= decode_scaling_list(gb, scaling_matrix8[5], 64, default_scaling8[1], scaling_matrix8[4]); // Inter, Cb
            }
        }
        if (!ret)
            ret = is_sps;
    }

    return ret;
}

static inline int decode_hrd_parameters(GetBitContext *gb, SPS *sps)
{
    int cpb_count, i;
    cpb_count = get_ue_golomb_31(gb) + 1;

    if (cpb_count > 32U) {
//        av_log(avctx, AV_LOG_ERROR, "cpb_count %d invalid\n", cpb_count);
        return -1;
    }

    get_bits(gb, 4); /* bit_rate_scale */
    get_bits(gb, 4); /* cpb_size_scale */
    for (i = 0; i < cpb_count; i++) {
        get_ue_golomb_long(gb); /* bit_rate_value_minus1 */
        get_ue_golomb_long(gb); /* cpb_size_value_minus1 */
        get_bits1(gb);          /* cbr_flag */
    }
    sps->initial_cpb_removal_delay_length = get_bits(gb, 5) + 1;
    sps->cpb_removal_delay_length         = get_bits(gb, 5) + 1;
    sps->dpb_output_delay_length          = get_bits(gb, 5) + 1;
    sps->time_offset_length               = get_bits(gb, 5);
    sps->cpb_cnt                          = cpb_count;
    return 0;
}

static inline int decode_vui_parameters(GetBitContext *gb, SPS *sps)
{
    int aspect_ratio_info_present_flag;
    unsigned int aspect_ratio_idc;

    aspect_ratio_info_present_flag = get_bits1(gb);

    if (aspect_ratio_info_present_flag) {
        aspect_ratio_idc = get_bits(gb, 8);
        if (aspect_ratio_idc == EXTENDED_SAR) {
            sps->sar.num = get_bits(gb, 16);
            sps->sar.den = get_bits(gb, 16);
        } else if (aspect_ratio_idc < FF_ARRAY_ELEMS(ff_h264_pixel_aspect)) {
            sps->sar = ff_h264_pixel_aspect[aspect_ratio_idc];
        } else {
//            av_log(avctx, AV_LOG_ERROR, "illegal aspect ratio\n");
            return -1;
        }
    } else {
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
            sps->color_trc       = get_bits(gb, 8); /* transfer_characteristics */
            sps->colorspace      = get_bits(gb, 8); /* matrix_coefficients */

            // Set invalid values to "unspecified"
//            if (!av_color_primaries_name(sps->color_primaries))
//                sps->color_primaries = AVCOL_PRI_UNSPECIFIED;
//            if (!av_color_transfer_name(sps->color_trc))
//                sps->color_trc = AVCOL_TRC_UNSPECIFIED;
//            if (!av_color_space_name(sps->colorspace))
//                sps->colorspace = AVCOL_SPC_UNSPECIFIED;
        }
    }

    /* chroma_location_info_present_flag */
    if (get_bits1(gb)) {
        /* chroma_sample_location_type_top_field */
//        avctx->chroma_sample_location = get_ue_golomb(gb) + 1;
        int chroma_sample_location = get_ue_golomb(gb) + 1;
        get_ue_golomb(gb);  /* chroma_sample_location_type_bottom_field */
    }

    if (show_bits1(gb) && get_bits_left(gb) < 10) {
//        av_log(avctx, AV_LOG_WARNING, "Truncated VUI\n");
        return 0;
    }

    sps->timing_info_present_flag = get_bits1(gb);
    if (sps->timing_info_present_flag) {
        unsigned num_units_in_tick = get_bits_long(gb, 32);
        unsigned time_scale        = get_bits_long(gb, 32);
        if (!num_units_in_tick || !time_scale) {
//            av_log(avctx, AV_LOG_ERROR,
//                   "time_scale/num_units_in_tick invalid or unsupported (%u/%u)\n",
//                   time_scale, num_units_in_tick);
            sps->timing_info_present_flag = 0;
        } else {
            sps->num_units_in_tick = num_units_in_tick;
            sps->time_scale = time_scale;
        }
        sps->fixed_frame_rate_flag = get_bits1(gb);
    }

    sps->nal_hrd_parameters_present_flag = get_bits1(gb);
    if (sps->nal_hrd_parameters_present_flag)
        if (decode_hrd_parameters(gb, sps) < 0)
            return -1;
    sps->vcl_hrd_parameters_present_flag = get_bits1(gb);
    if (sps->vcl_hrd_parameters_present_flag)
        if (decode_hrd_parameters(gb, sps) < 0)
            return -1;
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
            sps->num_reorder_frames         = 0;
            sps->bitstream_restriction_flag = 0;
        }

        if (sps->num_reorder_frames > 16U
            /* max_dec_frame_buffering || max_dec_frame_buffering > 16 */) {
//            av_log(avctx, AV_LOG_ERROR,
//                   "Clipping illegal num_reorder_frames %d\n",
//                   sps->num_reorder_frames);
            sps->num_reorder_frames = 16;
            return -1;
        }
    }

    return 0;
}

int H264DecodeNalSPS(const uint8_t *data, int size, H264ParamSets *ps, int ignore_truncation) {
    int profile_idc, level_idc, constraint_set_flags = 0;
    unsigned int sps_id;
    int i, log2_max_frame_num_minus4;
    SPS *sps;
    int ret;
    GetBitContext gbc;
    GetBitContext * gb = &gbc;
    init_get_bits8(gb, data, size);

    profile_idc           = get_bits(gb, 8);
    constraint_set_flags |= get_bits1(gb) << 0;   // constraint_set0_flag
    constraint_set_flags |= get_bits1(gb) << 1;   // constraint_set1_flag
    constraint_set_flags |= get_bits1(gb) << 2;   // constraint_set2_flag
    constraint_set_flags |= get_bits1(gb) << 3;   // constraint_set3_flag
    constraint_set_flags |= get_bits1(gb) << 4;   // constraint_set4_flag
    constraint_set_flags |= get_bits1(gb) << 5;   // constraint_set5_flag
    skip_bits(gb, 2);                             // reserved_zero_2bits
    level_idc = get_bits(gb, 8);
    sps_id    = get_ue_golomb_31(gb);

    //check if sps already in sps list
    if (ps->sps_list[sps_id] && (memcmp(ps->sps_list[sps_id]->data, data, size) == 0)) {
        // sps is exist and is same
        return 0;
    }

    sps = calloc(1, sizeof(SPS));

    sps->data_size = gb->buffer_end - gb->buffer;
    if (sps->data_size > sizeof(sps->data)) {
        sps->data_size = sizeof(sps->data);
    }
    memcpy(sps->data, gb->buffer, sps->data_size);

    if (sps_id >= MAX_SPS_COUNT) {
//        av_log(avctx, AV_LOG_ERROR, "sps_id %u out of range\n", sps_id);
        goto fail;
    }

    sps->sps_id               = sps_id;
    sps->time_offset_length   = 24;
    sps->profile_idc          = profile_idc;
    sps->constraint_set_flags = constraint_set_flags;
    sps->level_idc            = level_idc;
    sps->full_range           = -1;

    memset(sps->scaling_matrix4, 16, sizeof(sps->scaling_matrix4));
    memset(sps->scaling_matrix8, 16, sizeof(sps->scaling_matrix8));
    sps->scaling_matrix_present = 0;
//    sps->colorspace = 2; //AVCOL_SPC_UNSPECIFIED

    if (sps->profile_idc == 100 ||  // High profile
        sps->profile_idc == 110 ||  // High10 profile
        sps->profile_idc == 122 ||  // High422 profile
        sps->profile_idc == 244 ||  // High444 Predictive profile
        sps->profile_idc ==  44 ||  // Cavlc444 profile
        sps->profile_idc ==  83 ||  // Scalable Constrained High profile (SVC)
        sps->profile_idc ==  86 ||  // Scalable High Intra profile (SVC)
        sps->profile_idc == 118 ||  // Stereo High profile (MVC)
        sps->profile_idc == 128 ||  // Multiview High profile (MVC)
        sps->profile_idc == 138 ||  // Multiview Depth High profile (MVCD)
        sps->profile_idc == 144) {  // old High444 profile
        sps->chroma_format_idc = get_ue_golomb_31(gb);
        if (sps->chroma_format_idc > 3U) {
//            avpriv_request_sample(avctx, "chroma_format_idc %u",
//                                  sps->chroma_format_idc);
            goto fail;
        } else if (sps->chroma_format_idc == 3) {
            sps->residual_color_transform_flag = get_bits1(gb);
            if (sps->residual_color_transform_flag) {
//                av_log(avctx, AV_LOG_ERROR, "separate color planes are not supported\n");
                goto fail;
            }
        }
        sps->bit_depth_luma   = get_ue_golomb(gb) + 8;
        sps->bit_depth_chroma = get_ue_golomb(gb) + 8;
        if (sps->bit_depth_chroma != sps->bit_depth_luma) {
//            avpriv_request_sample(avctx,
//                                  "Different chroma and luma bit depth");
            goto fail;
        }
        if (sps->bit_depth_luma   < 8 || sps->bit_depth_luma   > 14 ||
            sps->bit_depth_chroma < 8 || sps->bit_depth_chroma > 14) {
//            av_log(avctx, AV_LOG_ERROR, "illegal bit depth value (%d, %d)\n",
//                   sps->bit_depth_luma, sps->bit_depth_chroma);
            goto fail;
        }
        sps->transform_bypass = get_bits1(gb);
        ret = decode_scaling_matrices(gb, sps, NULL, 1,
                                      sps->scaling_matrix4, sps->scaling_matrix8);
        if (ret < 0)
            goto fail;
        sps->scaling_matrix_present |= ret;
    } else {
        sps->chroma_format_idc = 1;
        sps->bit_depth_luma    = 8;
        sps->bit_depth_chroma  = 8;
    }

    log2_max_frame_num_minus4 = get_ue_golomb(gb);
    if (log2_max_frame_num_minus4 < MIN_LOG2_MAX_FRAME_NUM - 4 ||
        log2_max_frame_num_minus4 > MAX_LOG2_MAX_FRAME_NUM - 4) {
//        av_log(avctx, AV_LOG_ERROR,
//               "log2_max_frame_num_minus4 out of range (0-12): %d\n",
//               log2_max_frame_num_minus4);
        goto fail;
    }
    sps->log2_max_frame_num = log2_max_frame_num_minus4 + 4;

    sps->poc_type = get_ue_golomb_31(gb);

    if (sps->poc_type == 0) { // FIXME #define
        unsigned t = get_ue_golomb(gb);
        if (t>12) {
//            av_log(avctx, AV_LOG_ERROR, "log2_max_poc_lsb (%d) is out of range\n", t);
            goto fail;
        }
        sps->log2_max_poc_lsb = t + 4;
    } else if (sps->poc_type == 1) { // FIXME #define
        sps->delta_pic_order_always_zero_flag = get_bits1(gb);
        sps->offset_for_non_ref_pic           = get_se_golomb_long(gb);
        sps->offset_for_top_to_bottom_field   = get_se_golomb_long(gb);

        if (   sps->offset_for_non_ref_pic         == INT32_MIN
               || sps->offset_for_top_to_bottom_field == INT32_MIN
                ) {
//            av_log(avctx, AV_LOG_ERROR,
//                   "offset_for_non_ref_pic or offset_for_top_to_bottom_field is out of range\n");
            goto fail;
        }

        sps->poc_cycle_length                 = get_ue_golomb(gb);

        if ((unsigned)sps->poc_cycle_length >=
            FF_ARRAY_ELEMS(sps->offset_for_ref_frame)) {
//            av_log(avctx, AV_LOG_ERROR,
//                   "poc_cycle_length overflow %d\n", sps->poc_cycle_length);
            goto fail;
        }

        for (i = 0; i < sps->poc_cycle_length; i++) {
            sps->offset_for_ref_frame[i] = get_se_golomb_long(gb);
            if (sps->offset_for_ref_frame[i] == INT32_MIN) {
//                av_log(avctx, AV_LOG_ERROR,
//                       "offset_for_ref_frame is out of range\n");
                goto fail;
            }
        }
    } else if (sps->poc_type != 2) {
//        av_log(avctx, AV_LOG_ERROR, "illegal POC type %d\n", sps->poc_type);
        goto fail;
    }

    sps->ref_frame_count = get_ue_golomb_31(gb);
//    if (avctx->codec_tag == MKTAG('S', 'M', 'V', '2'))
//        sps->ref_frame_count = FFMAX(2, sps->ref_frame_count);
    if (sps->ref_frame_count > MAX_DELAYED_PIC_COUNT) {
//        av_log(avctx, AV_LOG_ERROR,
//               "too many reference frames %d\n", sps->ref_frame_count);
        goto fail;
    }
    sps->gaps_in_frame_num_allowed_flag = get_bits1(gb);
    sps->mb_width                       = get_ue_golomb(gb) + 1;
    sps->mb_height                      = get_ue_golomb(gb) + 1;

    sps->frame_mbs_only_flag = get_bits1(gb);

    if (sps->mb_height >= INT_MAX / 2U) {
//        av_log(avctx, AV_LOG_ERROR, "height overflow\n");
        goto fail;
    }
    sps->mb_height *= 2 - sps->frame_mbs_only_flag;

    if (!sps->frame_mbs_only_flag)
        sps->mb_aff = get_bits1(gb);
    else
        sps->mb_aff = 0;

    if ((unsigned)sps->mb_width  >= INT_MAX / 16 ||
        (unsigned)sps->mb_height >= INT_MAX / 16) {
//        av_log(avctx, AV_LOG_ERROR, "mb_width/height overflow\n");
        goto fail;
    }

    sps->direct_8x8_inference_flag = get_bits1(gb);

    sps->crop = get_bits1(gb);
    if (sps->crop) {
        unsigned int crop_left   = get_ue_golomb(gb);
        unsigned int crop_right  = get_ue_golomb(gb);
        unsigned int crop_top    = get_ue_golomb(gb);
        unsigned int crop_bottom = get_ue_golomb(gb);
        int width  = 16 * sps->mb_width;
        int height = 16 * sps->mb_height;

//        if (avctx->flags2 & AV_CODEC_FLAG2_IGNORE_CROP) {
//            av_log(avctx, AV_LOG_DEBUG, "discarding sps cropping, original "
//                                        "values are l:%d r:%d t:%d b:%d\n",
//                   crop_left, crop_right, crop_top, crop_bottom);
//
//            sps->crop_left   =
//            sps->crop_right  =
//            sps->crop_top    =
//            sps->crop_bottom = 0;
//        } else {
            int vsub   = (sps->chroma_format_idc == 1) ? 1 : 0;
            int hsub   = (sps->chroma_format_idc == 1 ||
                          sps->chroma_format_idc == 2) ? 1 : 0;
            int step_x = 1 << hsub;
            int step_y = (2 - sps->frame_mbs_only_flag) << vsub;

            if (crop_left  > (unsigned)INT_MAX / 4 / step_x ||
                crop_right > (unsigned)INT_MAX / 4 / step_x ||
                crop_top   > (unsigned)INT_MAX / 4 / step_y ||
                crop_bottom> (unsigned)INT_MAX / 4 / step_y ||
                (crop_left + crop_right ) * step_x >= width ||
                (crop_top  + crop_bottom) * step_y >= height
                    ) {
//                av_log(avctx, AV_LOG_ERROR, "crop values invalid %d %d %d %d / %d %d\n", crop_left, crop_right, crop_top, crop_bottom, width, height);
                goto fail;
            }

            sps->crop_left   = crop_left   * step_x;
            sps->crop_right  = crop_right  * step_x;
            sps->crop_top    = crop_top    * step_y;
            sps->crop_bottom = crop_bottom * step_y;
//        }
    } else {
        sps->crop_left   =
        sps->crop_right  =
        sps->crop_top    =
        sps->crop_bottom =
        sps->crop        = 0;
    }

    sps->vui_parameters_present_flag = get_bits1(gb);
    if (sps->vui_parameters_present_flag) {
        int ret = decode_vui_parameters(gb, sps);
        if (ret < 0)
            goto fail;
    }

    if (get_bits_left(gb) < 0) {
//        av_log(avctx, ignore_truncation ? AV_LOG_WARNING : AV_LOG_ERROR,
//               "Overread %s by %d bits\n", sps->vui_parameters_present_flag ? "VUI" : "SPS", -get_bits_left(gb));
        if (!ignore_truncation)
            goto fail;
    }

    /* if the maximum delay is not stored in the SPS, derive it based on the
     * level */
    if (!sps->bitstream_restriction_flag &&
        (sps->ref_frame_count /*|| avctx->strict_std_compliance >= FF_COMPLIANCE_STRICT*/)) {
        sps->num_reorder_frames = MAX_DELAYED_PIC_COUNT - 1;
        for (i = 0; i < FF_ARRAY_ELEMS(level_max_dpb_mbs); i++) {
            if (level_max_dpb_mbs[i][0] == sps->level_idc) {
                sps->num_reorder_frames = FFMIN(level_max_dpb_mbs[i][1] / (sps->mb_width * sps->mb_height),
                                                sps->num_reorder_frames);
                break;
            }
        }
    }

    if (!sps->sar.den)
        sps->sar.den = 1;

    /* check if this is a repeat of an already parsed SPS, then keep the
     * original one.
     * otherwise drop all PPSes that depend on it */

    if (ps->sps_list[sps_id])
        free(ps->sps_list[sps_id]);

    ps->sps_list[sps_id] = sps;

    return 0;
fail:
    free(sps);
    return -1;
}

static int more_rbsp_data_in_pps(const SPS *sps)
{
    int profile_idc = sps->profile_idc;

    if ((profile_idc == 66 || profile_idc == 77 ||
         profile_idc == 88) && (sps->constraint_set_flags & 7)) {
//        av_log(logctx, AV_LOG_VERBOSE,
//               "Current profile doesn't provide more RBSP data in PPS, skipping\n");
        return 0;
    }

    return 1;
}

static void build_qp_table(PPS *pps, int t, int index, const int depth)
{
    int i;
    const int max_qp = 51 + 6 * (depth - 8);
    for (i = 0; i < max_qp + 1; i++)
        pps->chroma_qp_table[t][i] =
                ff_h264_chroma_qp[depth - 8][av_clip(i + index, 0, max_qp)];
}

static void init_dequant4_coeff_table(PPS *pps, const SPS *sps)
{
    int i, j, q, x;
    const int max_qp = 51 + 6 * (sps->bit_depth_luma - 8);
    for (i = 0; i < 6; i++) {
        pps->dequant4_coeff[i] = pps->dequant4_buffer[i];
        for (j = 0; j < i; j++)
            if (!memcmp(pps->scaling_matrix4[j], pps->scaling_matrix4[i],
                        16 * sizeof(uint8_t))) {
                pps->dequant4_coeff[i] = pps->dequant4_buffer[j];
                break;
            }
        if (j < i)
            continue;

        for (q = 0; q < max_qp + 1; q++) {
            int shift = ff_h264_quant_div6[q] + 2;
            int idx   = ff_h264_quant_rem6[q];
            for (x = 0; x < 16; x++)
                pps->dequant4_coeff[i][q][(x >> 2) | ((x << 2) & 0xF)] =
                        ((uint32_t)ff_h264_dequant4_coeff_init[idx][(x & 1) + ((x >> 2) & 1)] *
                         pps->scaling_matrix4[i][x]) << shift;
        }
    }
}

static void init_dequant8_coeff_table(PPS *pps, const SPS *sps)
{
    int i, j, q, x;
    const int max_qp = 51 + 6 * (sps->bit_depth_luma - 8);

    for (i = 0; i < 6; i++) {
        pps->dequant8_coeff[i] = pps->dequant8_buffer[i];
        for (j = 0; j < i; j++)
            if (!memcmp(pps->scaling_matrix8[j], pps->scaling_matrix8[i],
                        64 * sizeof(uint8_t))) {
                pps->dequant8_coeff[i] = pps->dequant8_buffer[j];
                break;
            }
        if (j < i)
            continue;

        for (q = 0; q < max_qp + 1; q++) {
            int shift = ff_h264_quant_div6[q];
            int idx   = ff_h264_quant_rem6[q];
            for (x = 0; x < 64; x++)
                pps->dequant8_coeff[i][q][(x >> 3) | ((x & 7) << 3)] =
                        ((uint32_t)ff_h264_dequant8_coeff_init[idx][ff_h264_dequant8_coeff_init_scan[((x >> 1) & 12) | (x & 3)]] *
                         pps->scaling_matrix8[i][x]) << shift;
        }
    }
}

static void init_dequant_tables(PPS *pps, const SPS *sps)
{
    int i, x;
    init_dequant4_coeff_table(pps, sps);
    memset(pps->dequant8_coeff, 0, sizeof(pps->dequant8_coeff));

    if (pps->transform_8x8_mode)
        init_dequant8_coeff_table(pps, sps);
    if (sps->transform_bypass) {
        for (i = 0; i < 6; i++)
            for (x = 0; x < 16; x++)
                pps->dequant4_coeff[i][0][x] = 1 << 6;
        if (pps->transform_8x8_mode)
            for (i = 0; i < 6; i++)
                for (x = 0; x < 64; x++)
                    pps->dequant8_coeff[i][0][x] = 1 << 6;
    }
}

int H264DecodeNalPPS(const uint8_t *data, int size, H264ParamSets *ps, int bit_length) {
    const SPS *sps;
    unsigned int pps_id;
    PPS *pps;
    int qp_bd_offset;
    int bits_left;
    int ret;

    GetBitContext gbc;
    GetBitContext * gb = &gbc;
    init_get_bits8(gb, data, size);

    pps_id = get_ue_golomb(gb);

    //check if sps already in sps list
    if (ps->pps_list[pps_id] && (memcmp(ps->pps_list[pps_id]->data, data, size) == 0)) {
        // sps is exist and is same
        return 0;
    }

    if (pps_id >= MAX_PPS_COUNT) {
        return -1;
    }

    pps = calloc(1, sizeof(PPS));

    pps->data_size = gb->buffer_end - gb->buffer;
    if (pps->data_size > sizeof(pps->data)) {
//        av_log(avctx, AV_LOG_DEBUG, "Truncating likely oversized PPS "
//                                    "(%"SIZE_SPECIFIER" > %"SIZE_SPECIFIER")\n",
//                pps->data_size, sizeof(pps->data));
        pps->data_size = sizeof(pps->data);
    }
    memcpy(pps->data, gb->buffer, pps->data_size);

    pps->sps_id = get_ue_golomb_31(gb);
    if ((unsigned)pps->sps_id >= MAX_SPS_COUNT ||
        !ps->sps_list[pps->sps_id]) {
//        av_log(avctx, AV_LOG_ERROR, "sps_id %u out of range\n", pps->sps_id);
        ret = -1;
        goto fail;
    }
    sps = (const SPS*)ps->sps_list[pps->sps_id]->data;
    if (sps->bit_depth_luma > 14) {
//        av_log(avctx, AV_LOG_ERROR,
//               "Invalid luma bit depth=%d\n",
//               sps->bit_depth_luma);
        ret = -1;
        goto fail;
    } else if (sps->bit_depth_luma == 11 || sps->bit_depth_luma == 13) {
//        avpriv_report_missing_feature(avctx,
//                                      "Unimplemented luma bit depth=%d",
//                                      sps->bit_depth_luma);
        ret = -2;
        goto fail;
    }

    pps->cabac             = get_bits1(gb);
    pps->pic_order_present = get_bits1(gb);
    pps->slice_group_count = get_ue_golomb(gb) + 1;
    if (pps->slice_group_count > 1) {
        pps->mb_slice_group_map_type = get_ue_golomb(gb);
//        av_log(avctx, AV_LOG_ERROR, "FMO not supported\n");
    }
    pps->ref_count[0] = get_ue_golomb(gb) + 1;
    pps->ref_count[1] = get_ue_golomb(gb) + 1;
    if (pps->ref_count[0] - 1 > 32 - 1 || pps->ref_count[1] - 1 > 32 - 1) {
//        av_log(avctx, AV_LOG_ERROR, "reference overflow (pps)\n");
        ret = -1;
        goto fail;
    }

    qp_bd_offset = 6 * (sps->bit_depth_luma - 8);

    pps->weighted_pred                        = get_bits1(gb);
    pps->weighted_bipred_idc                  = get_bits(gb, 2);
    pps->init_qp                              = get_se_golomb(gb) + 26U + qp_bd_offset;
    pps->init_qs                              = get_se_golomb(gb) + 26U + qp_bd_offset;
    pps->chroma_qp_index_offset[0]            = get_se_golomb(gb);
    if (pps->chroma_qp_index_offset[0] < -12 || pps->chroma_qp_index_offset[0] > 12) {
        ret = -1;
        goto fail;
    }

    pps->deblocking_filter_parameters_present = get_bits1(gb);
    pps->constrained_intra_pred               = get_bits1(gb);
    pps->redundant_pic_cnt_present            = get_bits1(gb);

    pps->transform_8x8_mode = 0;
    memcpy(pps->scaling_matrix4, sps->scaling_matrix4,
           sizeof(pps->scaling_matrix4));
    memcpy(pps->scaling_matrix8, sps->scaling_matrix8,
           sizeof(pps->scaling_matrix8));

    bits_left = bit_length - get_bits_count(gb);
    if (bits_left > 0 && more_rbsp_data_in_pps(sps)) {
        pps->transform_8x8_mode = get_bits1(gb);
        ret = decode_scaling_matrices(gb, sps, pps, 0,
                                      pps->scaling_matrix4, pps->scaling_matrix8);
        if (ret < 0)
            goto fail;
        // second_chroma_qp_index_offset
        pps->chroma_qp_index_offset[1] = get_se_golomb(gb);
        if (pps->chroma_qp_index_offset[1] < -12 || pps->chroma_qp_index_offset[1] > 12) {
            ret = -1;
            goto fail;
        }
    } else {
        pps->chroma_qp_index_offset[1] = pps->chroma_qp_index_offset[0];
    }

    build_qp_table(pps, 0, pps->chroma_qp_index_offset[0],
                   sps->bit_depth_luma);
    build_qp_table(pps, 1, pps->chroma_qp_index_offset[1],
                   sps->bit_depth_luma);

    init_dequant_tables(pps, sps);

    if (pps->chroma_qp_index_offset[0] != pps->chroma_qp_index_offset[1])
        pps->chroma_qp_diff = 1;

    if (ps->pps_list[pps_id])
        free(ps->pps_list[pps_id]);

    ps->pps_list[pps_id] = pps;

    return 0;

fail:
    free(pps);
    return ret;
}

/**
 * Compute profile from profile_idc and constraint_set?_flags.
 *
 * @param sps SPS
 *
 * @return profile as defined by FF_PROFILE_H264_*
 */
static int h264_get_profile(const SPS *sps)
{
    int profile = sps->profile_idc;

    switch (sps->profile_idc) {
        case PROFILE_H264_BASELINE:
            // constraint_set1_flag set to 1
            profile |= (sps->constraint_set_flags & 1 << 1) ? PROFILE_H264_CONSTRAINED : 0;
            break;
        case PROFILE_H264_HIGH_10:
        case PROFILE_H264_HIGH_422:
        case PROFILE_H264_HIGH_444_PREDICTIVE:
            // constraint_set3_flag set to 1
            profile |= (sps->constraint_set_flags & 1 << 3) ? PROFILE_H264_INTRA : 0;
            break;
    }

    return profile;
}

static int h264_init_poc(int pic_field_poc[2], int *pic_poc,
                     const SPS *sps, H264POCContext *pc,
                     int picture_structure, int nal_ref_idc)
{
    const int max_frame_num = 1 << sps->log2_max_frame_num;
    int64_t field_poc[2];

    pc->frame_num_offset = pc->prev_frame_num_offset;
    if (pc->frame_num < pc->prev_frame_num)
        pc->frame_num_offset += max_frame_num;

    if (sps->poc_type == 0) {
        const int max_poc_lsb = 1 << sps->log2_max_poc_lsb;

        if (pc->poc_lsb < pc->prev_poc_lsb &&
            pc->prev_poc_lsb - pc->poc_lsb >= max_poc_lsb / 2)
            pc->poc_msb = pc->prev_poc_msb + max_poc_lsb;
        else if (pc->poc_lsb > pc->prev_poc_lsb &&
                 pc->prev_poc_lsb - pc->poc_lsb < -max_poc_lsb / 2)
            pc->poc_msb = pc->prev_poc_msb - max_poc_lsb;
        else
            pc->poc_msb = pc->prev_poc_msb;
        field_poc[0] =
        field_poc[1] = pc->poc_msb + pc->poc_lsb;
        if (picture_structure == PICT_FRAME)
            field_poc[1] += pc->delta_poc_bottom;
    } else if (sps->poc_type == 1) {
        int abs_frame_num;
        int64_t expected_delta_per_poc_cycle, expectedpoc;
        int i;

        if (sps->poc_cycle_length != 0)
            abs_frame_num = pc->frame_num_offset + pc->frame_num;
        else
            abs_frame_num = 0;

        if (nal_ref_idc == 0 && abs_frame_num > 0)
            abs_frame_num--;

        expected_delta_per_poc_cycle = 0;
        for (i = 0; i < sps->poc_cycle_length; i++)
            // FIXME integrate during sps parse
            expected_delta_per_poc_cycle += sps->offset_for_ref_frame[i];

        if (abs_frame_num > 0) {
            int poc_cycle_cnt          = (abs_frame_num - 1) / sps->poc_cycle_length;
            int frame_num_in_poc_cycle = (abs_frame_num - 1) % sps->poc_cycle_length;

            expectedpoc = poc_cycle_cnt * expected_delta_per_poc_cycle;
            for (i = 0; i <= frame_num_in_poc_cycle; i++)
                expectedpoc = expectedpoc + sps->offset_for_ref_frame[i];
        } else
            expectedpoc = 0;

        if (nal_ref_idc == 0)
            expectedpoc = expectedpoc + sps->offset_for_non_ref_pic;

        field_poc[0] = expectedpoc + pc->delta_poc[0];
        field_poc[1] = field_poc[0] + sps->offset_for_top_to_bottom_field;

        if (picture_structure == PICT_FRAME)
            field_poc[1] += pc->delta_poc[1];
    } else {
        int poc = 2 * (pc->frame_num_offset + pc->frame_num);

        if (!nal_ref_idc)
            poc--;

        field_poc[0] = poc;
        field_poc[1] = poc;
    }

    if (   field_poc[0] != (int)field_poc[0]
           || field_poc[1] != (int)field_poc[1])
        return -1;

    if (picture_structure != PICT_BOTTOM_FIELD)
        pic_field_poc[0] = field_poc[0];
    if (picture_structure != PICT_TOP_FIELD)
        pic_field_poc[1] = field_poc[1];
    *pic_poc = FFMIN(pic_field_poc[0], pic_field_poc[1]);

    return 0;
}

int ff_h264_parse_ref_count(int *plist_count, int ref_count[2],
                            GetBitContext *gb, const PPS *pps,
                            int slice_type_nos, int picture_structure)
{
    int list_count;
    int num_ref_idx_active_override_flag;

    // set defaults, might be overridden a few lines later
    ref_count[0] = pps->ref_count[0];
    ref_count[1] = pps->ref_count[1];

    if (slice_type_nos != H264_PICTURE_TYPE_I) {
        unsigned max[2];
        max[0] = max[1] = picture_structure == PICT_FRAME ? 15 : 31;

        num_ref_idx_active_override_flag = get_bits1(gb);

        if (num_ref_idx_active_override_flag) {
            ref_count[0] = get_ue_golomb(gb) + 1;
            if (slice_type_nos == H264_PICTURE_TYPE_B) {
                ref_count[1] = get_ue_golomb(gb) + 1;
            } else
                // full range is spec-ok in this case, even for frames
                ref_count[1] = 1;
        }

        if (slice_type_nos == H264_PICTURE_TYPE_B)
            list_count = 2;
        else
            list_count = 1;

        if (ref_count[0] - 1 > max[0] || (list_count == 2 && (ref_count[1] - 1 > max[1]))) {
//            av_log(logctx, AV_LOG_ERROR, "reference overflow %u > %u or %u > %u\n",
//                   ref_count[0] - 1, max[0], ref_count[1] - 1, max[1]);
            ref_count[0] = ref_count[1] = 0;
            *plist_count = 0;
            goto fail;
        } else if (ref_count[1] - 1 > max[1]) {
//            av_log(logctx, AV_LOG_DEBUG, "reference overflow %u > %u \n",
//                   ref_count[1] - 1, max[1]);
            ref_count[1] = 0;
        }

    } else {
        list_count   = 0;
        ref_count[0] = ref_count[1] = 0;
    }

    *plist_count = list_count;

    return 0;
fail:
    *plist_count = 0;
    ref_count[0] = 0;
    ref_count[1] = 0;
    return -1;
}

static int h264_pred_weight_table(GetBitContext *gb, const SPS *sps,
                              const int *ref_count, int slice_type_nos,
                              H264PredWeightTable *pwt,
                              int picture_structure)
{
    int list, i, j;
    int luma_def, chroma_def;

    pwt->use_weight             = 0;
    pwt->use_weight_chroma      = 0;

    pwt->luma_log2_weight_denom = get_ue_golomb(gb);
    if (pwt->luma_log2_weight_denom > 7U) {
//        av_log(logctx, AV_LOG_ERROR, "luma_log2_weight_denom %d is out of range\n", pwt->luma_log2_weight_denom);
        pwt->luma_log2_weight_denom = 0;
    }
    luma_def = 1 << pwt->luma_log2_weight_denom;

    if (sps->chroma_format_idc) {
        pwt->chroma_log2_weight_denom = get_ue_golomb(gb);
        if (pwt->chroma_log2_weight_denom > 7U) {
//            av_log(logctx, AV_LOG_ERROR, "chroma_log2_weight_denom %d is out of range\n", pwt->chroma_log2_weight_denom);
            pwt->chroma_log2_weight_denom = 0;
        }
        chroma_def = 1 << pwt->chroma_log2_weight_denom;
    }

    for (list = 0; list < 2; list++) {
        pwt->luma_weight_flag[list]   = 0;
        pwt->chroma_weight_flag[list] = 0;
        for (i = 0; i < ref_count[list]; i++) {
            int luma_weight_flag, chroma_weight_flag;

            luma_weight_flag = get_bits1(gb);
            if (luma_weight_flag) {
                pwt->luma_weight[i][list][0] = get_se_golomb(gb);
                pwt->luma_weight[i][list][1] = get_se_golomb(gb);
                if ((int8_t)pwt->luma_weight[i][list][0] != pwt->luma_weight[i][list][0] ||
                    (int8_t)pwt->luma_weight[i][list][1] != pwt->luma_weight[i][list][1])
                    goto out_range_weight;
                if (pwt->luma_weight[i][list][0] != luma_def ||
                    pwt->luma_weight[i][list][1] != 0) {
                    pwt->use_weight             = 1;
                    pwt->luma_weight_flag[list] = 1;
                }
            } else {
                pwt->luma_weight[i][list][0] = luma_def;
                pwt->luma_weight[i][list][1] = 0;
            }

            if (sps->chroma_format_idc) {
                chroma_weight_flag = get_bits1(gb);
                if (chroma_weight_flag) {
                    int j;
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[i][list][j][0] = get_se_golomb(gb);
                        pwt->chroma_weight[i][list][j][1] = get_se_golomb(gb);
                        if ((int8_t)pwt->chroma_weight[i][list][j][0] != pwt->chroma_weight[i][list][j][0] ||
                            (int8_t)pwt->chroma_weight[i][list][j][1] != pwt->chroma_weight[i][list][j][1]) {
                            pwt->chroma_weight[i][list][j][0] = chroma_def;
                            pwt->chroma_weight[i][list][j][1] = 0;
                            goto out_range_weight;
                        }
                        if (pwt->chroma_weight[i][list][j][0] != chroma_def ||
                            pwt->chroma_weight[i][list][j][1] != 0) {
                            pwt->use_weight_chroma        = 1;
                            pwt->chroma_weight_flag[list] = 1;
                        }
                    }
                } else {
                    int j;
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[i][list][j][0] = chroma_def;
                        pwt->chroma_weight[i][list][j][1] = 0;
                    }
                }
            }

            // for MBAFF
            if (picture_structure == PICT_FRAME) {
                pwt->luma_weight[16 + 2 * i][list][0] = pwt->luma_weight[16 + 2 * i + 1][list][0] = pwt->luma_weight[i][list][0];
                pwt->luma_weight[16 + 2 * i][list][1] = pwt->luma_weight[16 + 2 * i + 1][list][1] = pwt->luma_weight[i][list][1];
                if (sps->chroma_format_idc) {
                    for (j = 0; j < 2; j++) {
                        pwt->chroma_weight[16 + 2 * i][list][j][0] = pwt->chroma_weight[16 + 2 * i + 1][list][j][0] = pwt->chroma_weight[i][list][j][0];
                        pwt->chroma_weight[16 + 2 * i][list][j][1] = pwt->chroma_weight[16 + 2 * i + 1][list][j][1] = pwt->chroma_weight[i][list][j][1];
                    }
                }
            }
        }
        if (slice_type_nos != H264_PICTURE_TYPE_B)
            break;
    }
    pwt->use_weight = pwt->use_weight || pwt->use_weight_chroma;
    return 0;
out_range_weight:
//    avpriv_request_sample(logctx, "Out of range weight");
    return -1;
}

static int scan_mmco_reset(H264ParseContext *s, GetBitContext *gb)
{
    H264PredWeightTable pwt;
    int slice_type_nos = s->pict_type & 3;
    int list_count, ref_count[2];


    if (s->ps.pps->redundant_pic_cnt_present)
        get_ue_golomb(gb); // redundant_pic_count

    if (slice_type_nos == H264_PICTURE_TYPE_B)
        get_bits1(gb); // direct_spatial_mv_pred

    if (ff_h264_parse_ref_count(&list_count, ref_count, gb, s->ps.pps,
                                slice_type_nos, s->picture_structure) < 0)
        return -1;

    if (slice_type_nos != H264_PICTURE_TYPE_I) {
        int list;
        for (list = 0; list < list_count; list++) {
            if (get_bits1(gb)) {
                int index;
                for (index = 0; ; index++) {
                    unsigned int reordering_of_pic_nums_idc = get_ue_golomb_31(gb);

                    if (reordering_of_pic_nums_idc < 3)
                        get_ue_golomb_long(gb);
                    else if (reordering_of_pic_nums_idc > 3) {
//                        av_log(logctx, AV_LOG_ERROR,
//                               "illegal reordering_of_pic_nums_idc %d\n",
//                               reordering_of_pic_nums_idc);
                        return -1;
                    } else
                        break;

                    if (index >= ref_count[list]) {
//                        av_log(logctx, AV_LOG_ERROR,
//                               "reference count %d overflow\n", index);
                        return -1;
                    }
                }
            }
        }
    }

    if ((s->ps.pps->weighted_pred && slice_type_nos == H264_PICTURE_TYPE_P) ||
        (s->ps.pps->weighted_bipred_idc == 1 && slice_type_nos == H264_PICTURE_TYPE_B))
        h264_pred_weight_table(gb, s->ps.sps, ref_count, slice_type_nos,
                                  &pwt, s->picture_structure);

    if (get_bits1(gb)) { // adaptive_ref_pic_marking_mode_flag
        int i;
        for (i = 0; i < MAX_MMCO_COUNT; i++) {
            MMCOOpcode opcode = get_ue_golomb_31(gb);
            if (opcode > (unsigned) MMCO_LONG) {
//                av_log(logctx, AV_LOG_ERROR,
//                       "illegal memory management control operation %d\n",
//                       opcode);
                return -1;
            }
            if (opcode == MMCO_END)
                return 0;
            else if (opcode == MMCO_RESET)
                return 1;

            if (opcode == MMCO_SHORT2UNUSED || opcode == MMCO_SHORT2LONG)
                get_ue_golomb_long(gb); // difference_of_pic_nums_minus1
            if (opcode == MMCO_SHORT2LONG || opcode == MMCO_LONG2UNUSED ||
                opcode == MMCO_LONG || opcode == MMCO_SET_MAX_LONG)
                get_ue_golomb_31(gb);
        }
    }

    return 0;
}

int H264ParseSliceHeader(const uint8_t *data, int size, const H264NAL* nal, H264ParseContext *s) {
    int buf_index, next_avc;
    unsigned int pps_id;
    unsigned int slice_type;
    int state = -1, got_reset = 0;
    const SPS *sps;
    int field_poc[2];
    int ret;

    GetBitContext gbc;
    GetBitContext * gb = &gbc;
    init_get_bits8(gb, data, size);

    /* set some sane default values */
    s->pict_type         = H264_PICTURE_TYPE_I;
    s->key_frame         = 0;
    s->picture_structure = H264_PICTURE_STRUCTURE_UNKNOWN;

    get_ue_golomb_long(gb);  // skip first_mb_in_slice
    slice_type   = get_ue_golomb_31(gb);
    s->pict_type = ff_h264_golomb_to_pict_type[slice_type % 5];
//    if (p->sei.recovery_point.recovery_frame_cnt >= 0) {
    if (s->pict_type == H264_PICTURE_TYPE_I) {
        /* key frame, since recovery_frame_cnt is set */
        s->key_frame = 1;
    }
    pps_id = get_ue_golomb(gb);
    if (pps_id >= MAX_PPS_COUNT) {
//        av_log(avctx, AV_LOG_ERROR,
//               "pps_id %u out of range\n", pps_id);
        goto fail;
    }
    if (!s->ps.pps_list[pps_id]) {
//        av_log(avctx, AV_LOG_ERROR,
//               "non-existing PPS %u referenced\n", pps_id);
        goto fail;
    }

    s->ps.pps = s->ps.pps_list[pps_id];

    if (!s->ps.sps_list[s->ps.pps->sps_id]) {
//        av_log(avctx, AV_LOG_ERROR,
//               "non-existing SPS %u referenced\n", p->ps.pps->sps_id);
        goto fail;
    }

    s->ps.sps = s->ps.sps_list[s->ps.pps->sps_id];

    sps = s->ps.sps;

    // heuristic to detect non marked keyframes
    if (s->ps.sps->ref_frame_count <= 1 && s->ps.pps->ref_count[0] <= 1 && s->pict_type == H264_PICTURE_TYPE_I)
        s->key_frame = 1;

    s->poc.frame_num = get_bits(gb, sps->log2_max_frame_num);

    s->coded_width  = 16 * sps->mb_width;
    s->coded_height = 16 * sps->mb_height;
    s->width        = s->coded_width  - (sps->crop_right + sps->crop_left);
    s->height       = s->coded_height - (sps->crop_top   + sps->crop_bottom);
    if (s->width <= 0 || s->height <= 0) {
        s->width  = s->coded_width;
        s->height = s->coded_height;
    }

//    switch (sps->bit_depth_luma) {
//        case 9:
//            if (sps->chroma_format_idc == 3)      s->format = AV_PIX_FMT_YUV444P9;
//            else if (sps->chroma_format_idc == 2) s->format = AV_PIX_FMT_YUV422P9;
//            else                                  s->format = AV_PIX_FMT_YUV420P9;
//            break;
//        case 10:
//            if (sps->chroma_format_idc == 3)      s->format = AV_PIX_FMT_YUV444P10;
//            else if (sps->chroma_format_idc == 2) s->format = AV_PIX_FMT_YUV422P10;
//            else                                  s->format = AV_PIX_FMT_YUV420P10;
//            break;
//        case 8:
//            if (sps->chroma_format_idc == 3)      s->format = AV_PIX_FMT_YUV444P;
//            else if (sps->chroma_format_idc == 2) s->format = AV_PIX_FMT_YUV422P;
//            else                                  s->format = AV_PIX_FMT_YUV420P;
//            break;
//        default:
//            s->format = AV_PIX_FMT_NONE;
//    }

    s->profile = h264_get_profile(sps);
    s->level   = sps->level_idc;

    if (sps->frame_mbs_only_flag) {
        s->picture_structure = PICT_FRAME;
    } else {
        if (get_bits1(gb)) { // field_pic_flag
            s->picture_structure = PICT_TOP_FIELD + get_bits1(gb); // bottom_field_flag
        } else {
            s->picture_structure = PICT_FRAME;
        }
    }

    if (nal->type == H264_NAL_IDR_SLICE)
        get_ue_golomb_long(gb); /* idr_pic_id */
    if (sps->poc_type == 0) {
        s->poc.poc_lsb = get_bits(gb, sps->log2_max_poc_lsb);

        if (s->ps.pps->pic_order_present == 1 &&
            s->picture_structure == PICT_FRAME)
            s->poc.delta_poc_bottom = get_se_golomb(gb);
    }

    if (sps->poc_type == 1 &&
        !sps->delta_pic_order_always_zero_flag) {
        s->poc.delta_poc[0] = get_se_golomb(gb);

        if (s->ps.pps->pic_order_present == 1 &&
            s->picture_structure == PICT_FRAME)
            s->poc.delta_poc[1] = get_se_golomb(gb);
    }

    /* Decode POC of this picture.
     * The prev_ values needed for decoding POC of the next picture are not set here. */
    field_poc[0] = field_poc[1] = INT_MAX;
    ret = h264_init_poc(field_poc, &s->output_picture_number, sps,
                           &s->poc, s->picture_structure, nal->ref_idc);
    if (ret < 0)
        goto fail;

    /* Continue parsing to check if MMCO_RESET is present.
     * FIXME: MMCO_RESET could appear in non-first slice.
     *        Maybe, we should parse all undisposable non-IDR slice of this
     *        picture until encountering MMCO_RESET in a slice of it. */
    if (nal->ref_idc && nal->type != H264_NAL_IDR_SLICE) {
        got_reset = scan_mmco_reset(s, gb);
        if (got_reset < 0)
            goto fail;
    }

    /* Set up the prev_ values for decoding POC of the next picture. */
    s->poc.prev_frame_num        = got_reset ? 0 : s->poc.frame_num;
    s->poc.prev_frame_num_offset = got_reset ? 0 : s->poc.frame_num_offset;
    if (nal->ref_idc != 0) {
        if (!got_reset) {
            s->poc.prev_poc_msb = s->poc.poc_msb;
            s->poc.prev_poc_lsb = s->poc.poc_lsb;
        } else {
            s->poc.prev_poc_msb = 0;
            s->poc.prev_poc_lsb =
                    s->picture_structure == PICT_BOTTOM_FIELD ? 0 : field_poc[0];
        }
    }

//    if (sps->pic_struct_present_flag && p->sei.picture_timing.present) {
//        switch (p->sei.picture_timing.pic_struct) {
//            case H264_SEI_PIC_STRUCT_TOP_FIELD:
//            case H264_SEI_PIC_STRUCT_BOTTOM_FIELD:
//                s->repeat_pict = 0;
//                break;
//            case H264_SEI_PIC_STRUCT_FRAME:
//            case H264_SEI_PIC_STRUCT_TOP_BOTTOM:
//            case H264_SEI_PIC_STRUCT_BOTTOM_TOP:
//                s->repeat_pict = 1;
//                break;
//            case H264_SEI_PIC_STRUCT_TOP_BOTTOM_TOP:
//            case H264_SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM:
//                s->repeat_pict = 2;
//                break;
//            case H264_SEI_PIC_STRUCT_FRAME_DOUBLING:
//                s->repeat_pict = 3;
//                break;
//            case H264_SEI_PIC_STRUCT_FRAME_TRIPLING:
//                s->repeat_pict = 5;
//                break;
//            default:
//                s->repeat_pict = p->picture_structure == PICT_FRAME ? 1 : 0;
//                break;
//        }
//    } else {
//        s->repeat_pict = p->picture_structure == PICT_FRAME ? 1 : 0;
//    }

//    if (s->picture_structure == PICT_FRAME) {
//        s->picture_structure = AV_PICTURE_STRUCTURE_FRAME;
//        if (sps->pic_struct_present_flag && p->sei.picture_timing.present) {
//            switch (p->sei.picture_timing.pic_struct) {
//                case H264_SEI_PIC_STRUCT_TOP_BOTTOM:
//                case H264_SEI_PIC_STRUCT_TOP_BOTTOM_TOP:
//                    s->field_order = AV_FIELD_TT;
//                    break;
//                case H264_SEI_PIC_STRUCT_BOTTOM_TOP:
//                case H264_SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM:
//                    s->field_order = AV_FIELD_BB;
//                    break;
//                default:
//                    s->field_order = AV_FIELD_PROGRESSIVE;
//                    break;
//            }
//        } else {
//            if (field_poc[0] < field_poc[1])
//                s->field_order = AV_FIELD_TT;
//            else if (field_poc[0] > field_poc[1])
//                s->field_order = AV_FIELD_BB;
//            else
//                s->field_order = AV_FIELD_PROGRESSIVE;
//        }
//    } else {
//        if (p->picture_structure == PICT_TOP_FIELD)
//            s->picture_structure = AV_PICTURE_STRUCTURE_TOP_FIELD;
//        else
//            s->picture_structure = AV_PICTURE_STRUCTURE_BOTTOM_FIELD;
//        if (p->poc.frame_num == p->last_frame_num &&
//            p->last_picture_structure != AV_PICTURE_STRUCTURE_UNKNOWN &&
//            p->last_picture_structure != AV_PICTURE_STRUCTURE_FRAME &&
//            p->last_picture_structure != s->picture_structure) {
//            if (p->last_picture_structure == AV_PICTURE_STRUCTURE_TOP_FIELD)
//                s->field_order = AV_FIELD_TT;
//            else
//                s->field_order = AV_FIELD_BB;
//        } else {
//            s->field_order = AV_FIELD_UNKNOWN;
//        }
//        p->last_picture_structure = s->picture_structure;
//        p->last_frame_num = p->poc.frame_num;
//    }
//
//    av_freep(&rbsp.rbsp_buffer);
    return 0; /* no need to evaluate the rest */

fail:
    return -1;
}

H264ParseContext * CreateH264ParserContext() {
    H264ParseContext * ctx = calloc(1, sizeof(H264ParseContext));
    H264ParamSets *ps = &ctx->ps;
    memset(ps->sps_list, 0, MAX_SPS_COUNT * sizeof(SPS *));
    memset(ps->pps_list, 0, MAX_PPS_COUNT * sizeof(SPS *));
    ps->sps = NULL;
    ps->pps = NULL;
    return ctx;
}
void ReleaseH264ParserContext(H264ParseContext **p_ctx) {
    int i;
    H264ParseContext *ctx = *p_ctx;
    if (ctx == NULL)
        return;

    H264ParamSets *ps = &ctx->ps;
    for (i = 0; i < MAX_SPS_COUNT; i++) {
        if (ps->sps_list[i])
            free(ps->sps_list[i]);
    }
    for (i = 0; i < MAX_PPS_COUNT; i++) {
        if (ps->pps_list[i])
            free(&ps->pps_list[i]);
    }

    ps->sps = NULL;
    ps->pps = NULL;

    free(ctx);
    *p_ctx = NULL;
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

int H264ParseNalUnits(H264ParseContext *ctx, const uint8_t *buf, int buf_size, int is_avcc) {
    int ret = -1;
    if (is_avcc) {
        uint8_t *ptr = (uint8_t *)buf;
        int nalu_size = MEM_RB32(ptr);
        int nalu_size_total = nalu_size + 4;
        int offset = 0;
        while (nalu_size > 1 && offset + nalu_size_total <= buf_size) {
            if(H264ParseSingleNalUnits(ctx, ptr + 4, nalu_size) >= 0) ret = 0;
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

            if(H264ParseSingleNalUnits(ctx, find_ptr + find_nalu_header_size, nalu_len - find_nalu_header_size) >= 0) ret = 0;

            find_ptr = p;

        }
    }
    return ret;
}
int H264ParseSingleNalUnits(H264ParseContext *ctx, const uint8_t *buf, int buf_size) {
    H264ParamSets *ps = &ctx->ps;
//	H264SEI *sei = &ctx->sei;
    int ret = -1;
    int rbsp_size;

    H264NAL nal;

    GetBitContext gb;
    init_get_bits8(&gb, buf, buf_size);

    get_bits1(&gb);
    nal.ref_idc = get_bits(&gb, 2);
    nal.type    = get_bits(&gb, 5);

    rbsp_size = nal_unit_extract_rbsp(buf, MIN(buf_size, sizeof(ctx->rbsp)), ctx->rbsp);

    const uint8_t *nal_ptr = ctx->rbsp + 1;
    int nal_size = rbsp_size - 1;

    switch (nal.type) {
        case H264_NAL_SPS:
            H264DecodeNalSPS(nal_ptr, nal_size, &ctx->ps, 0);
            break;
        case H264_NAL_PPS:
            H264DecodeNalPPS(nal_ptr, nal_size, &ctx->ps, nal_size*8);
            break;
        case H264_NAL_SEI:
            break;
        case H264_NAL_IDR_SLICE:
            ctx->key_frame = 1;

            ctx->poc.prev_frame_num        = 0;
            ctx->poc.prev_frame_num_offset = 0;
            ctx->poc.prev_poc_msb          =
            ctx->poc.prev_poc_lsb          = 0;
        case H264_NAL_SLICE:
            ret = H264ParseSliceHeader(nal_ptr, nal_size, &nal, ctx);
            break;
        default:
            break;
    }

    /* didn't find a picture! */
    return ret;
}

int H264DecodeExtradata(H264ParseContext *ctx, const uint8_t *data, int size) {
    int ret;

    if (!data || size <= 0)
        return -1;

    if (data[0] == 1) {
        int i, cnt, nalsize;
        const uint8_t *p = data;

        ctx->is_avc = 1;

        if (size < 7) {
//            av_log(logctx, AV_LOG_ERROR, "avcC %d too short\n", size);
            return -1;
        }

        // Decode sps from avcC
        cnt = *(p + 5) & 0x1f; // Number of sps
        p  += 6;
        for (i = 0; i < cnt; i++) {
            nalsize = MEM_RB16(p);
            if (nalsize  + 2 > size - (p - data))
                return -1;
            ret = H264ParseSingleNalUnits(ctx, p + 2, nalsize);
            p += nalsize + 2;
        }
        // Decode pps from avcC
        cnt = *(p++); // Number of pps
        for (i = 0; i < cnt; i++) {
            nalsize = MEM_RB16(p);
            if (nalsize  + 2 > size - (p - data))
                return -1;
            ret = H264ParseSingleNalUnits(ctx, p + 2, nalsize);
            p += nalsize;
        }
        // Store right nal length size that will be used to parse all other nals
//        *nal_length_size = (data[4] & 0x03) + 1;
    } else {
        ctx->is_avc = 0;
        ret = H264ParseNalUnits(ctx, data, size, 0);
        if (ret < 0)
            return ret;
    }
    return size;
}
