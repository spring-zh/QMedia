#ifndef _AVCC_PARSE_H
#define _AVCC_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define H264_MAX_PICTURE_COUNT 36
#define H264_MAX_THREADS       32

#define MAX_SPS_COUNT          32
#define MAX_PPS_COUNT         256

#define MAX_MMCO_COUNT         66

#define MAX_DELAYED_PIC_COUNT  16

#define MAX_MBPAIR_SIZE (256*1024) // a tighter bound could be calculated if someone cares about a few bytes

#define MB_TYPE_REF0       MB_TYPE_ACPRED // dirty but it fits in 16 bit
#define MB_TYPE_8x8DCT     0x01000000
#define IS_REF0(a)         ((a) & MB_TYPE_REF0)
#define IS_8x8DCT(a)       ((a) & MB_TYPE_8x8DCT)

#define QP_MAX_NUM (51 + 6*6)           // The maximum supported qp

/* NAL unit types */
enum {
	NAL_SLICE = 1,
	NAL_DPA = 2,
	NAL_DPB = 3,
	NAL_DPC = 4,
	NAL_IDR_SLICE = 5,
	NAL_SEI = 6,
	NAL_SPS = 7,
	NAL_PPS = 8,
	NAL_AUD = 9,
	NAL_END_SEQUENCE = 10,
	NAL_END_STREAM = 11,
	NAL_FILLER_DATA = 12,
	NAL_SPS_EXT = 13,
	NAL_AUXILIARY_SLICE = 19,
	NAL_FF_IGNORE = 0xff0f001,
};

/**
* SEI message types
*/
typedef enum {
	SEI_TYPE_BUFFERING_PERIOD = 0,   ///< buffering period (H.264, D.1.1)
	SEI_TYPE_PIC_TIMING = 1,   ///< picture timing
	SEI_TYPE_USER_DATA_ITU_T_T35 = 4,   ///< user data registered by ITU-T Recommendation T.35
	SEI_TYPE_USER_DATA_UNREGISTERED = 5,   ///< unregistered user data
	SEI_TYPE_RECOVERY_POINT = 6,   ///< recovery point (frame # to decoder sync)
	SEI_TYPE_FRAME_PACKING = 45,  ///< frame packing arrangement
	SEI_TYPE_DISPLAY_ORIENTATION = 47,  ///< display orientation
} SEI_Type;

/**
* pic_struct in picture timing SEI message
*/
typedef enum {
	SEI_PIC_STRUCT_FRAME = 0, ///<  0: %frame
	SEI_PIC_STRUCT_TOP_FIELD = 1, ///<  1: top field
	SEI_PIC_STRUCT_BOTTOM_FIELD = 2, ///<  2: bottom field
	SEI_PIC_STRUCT_TOP_BOTTOM = 3, ///<  3: top field, bottom field, in that order
	SEI_PIC_STRUCT_BOTTOM_TOP = 4, ///<  4: bottom field, top field, in that order
	SEI_PIC_STRUCT_TOP_BOTTOM_TOP = 5, ///<  5: top field, bottom field, top field repeated, in that order
	SEI_PIC_STRUCT_BOTTOM_TOP_BOTTOM = 6, ///<  6: bottom field, top field, bottom field repeated, in that order
	SEI_PIC_STRUCT_FRAME_DOUBLING = 7, ///<  7: %frame doubling
	SEI_PIC_STRUCT_FRAME_TRIPLING = 8  ///<  8: %frame tripling
} SEI_PicStructType;

/**
* Chromaticity coordinates of the source primaries.
*/
enum ColorPrimaries {
	COL_PRI_RESERVED0 = 0,
	COL_PRI_BT709 = 1, ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
	COL_PRI_UNSPECIFIED = 2,
	COL_PRI_RESERVED = 3,
	COL_PRI_BT470M = 4, ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

	COL_PRI_BT470BG = 5, ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
	COL_PRI_SMPTE170M = 6, ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
	COL_PRI_SMPTE240M = 7, ///< functionally identical to above
	COL_PRI_FILM = 8, ///< colour filters using Illuminant C
	COL_PRI_BT2020 = 9, ///< ITU-R BT2020
	COL_PRI_NB,              ///< Not part of ABI
};
/**
* Color Transfer Characteristic.
*/
enum ColorTransferCharacteristic {
	COL_TRC_RESERVED0 = 0,
	COL_TRC_BT709 = 1,  ///< also ITU-R BT1361
	COL_TRC_UNSPECIFIED = 2,
	COL_TRC_RESERVED = 3,
	COL_TRC_GAMMA22 = 4,  ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
	COL_TRC_GAMMA28 = 5,  ///< also ITU-R BT470BG
	COL_TRC_SMPTE170M = 6,  ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
	COL_TRC_SMPTE240M = 7,
	COL_TRC_LINEAR = 8,  ///< "Linear transfer characteristics"
	COL_TRC_LOG = 9,  ///< "Logarithmic transfer characteristic (100:1 range)"
	COL_TRC_LOG_SQRT = 10, ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
	COL_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
	COL_TRC_BT1361_ECG = 12, ///< ITU-R BT1361 Extended Colour Gamut
	COL_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
	COL_TRC_BT2020_10 = 14, ///< ITU-R BT2020 for 10 bit system
	COL_TRC_BT2020_12 = 15, ///< ITU-R BT2020 for 12 bit system
	COL_TRC_NB,                ///< Not part of ABI
};
/**
* YUV colorspace type.
*/
enum ColorSpace {
	COL_SPC_RGB = 0,  ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB)
	COL_SPC_BT709 = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
	COL_SPC_UNSPECIFIED = 2,
	COL_SPC_RESERVED = 3,
	COL_SPC_FCC = 4,  ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
	COL_SPC_BT470BG = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
	COL_SPC_SMPTE170M = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC / functionally identical to above
	COL_SPC_SMPTE240M = 7,
	COL_SPC_YCOCG = 8,  ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
	COL_SPC_BT2020_NCL = 9,  ///< ITU-R BT2020 non-constant luminance system
	COL_SPC_BT2020_CL = 10, ///< ITU-R BT2020 constant luminance system
	COL_SPC_NB,               ///< Not part of ABI
};

enum PictureType {
	PICTURE_TYPE_NONE = 0, ///< Undefined
	PICTURE_TYPE_I,     ///< Intra
	PICTURE_TYPE_P,     ///< Predicted
	PICTURE_TYPE_B,     ///< Bi-dir predicted
	PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG4
	PICTURE_TYPE_SI,    ///< Switching Intra
	PICTURE_TYPE_SP,    ///< Switching Predicted
	PICTURE_TYPE_BI,    ///< BI type
};

/**
* Sequence parameter set
*/
/**
* rational number numerator/denominator
*/
typedef struct Rational{
	int num; ///< numerator
	int den; ///< denominator
} Rational;

typedef struct H264NALUnitArray {
	uint8_t  array_completeness;
	uint8_t  NAL_unit_type;
	uint16_t numNalus;
	uint16_t *nalUnitLength;
	uint8_t  **nalUnit;
} H264NALUnitArray;
typedef struct H264DecoderConfigurationRecord {
	uint8_t profile_idc;
	uint8_t level_idc;
	uint8_t chroma_format_idc;
	uint8_t  numOfArrays;
	H264NALUnitArray *array;
} H264DecoderConfigurationRecord;

typedef struct SPS {
	unsigned int sps_id;
	int profile_idc;
	int level_idc;
	int chroma_format_idc;
	int transform_bypass;              ///< qpprime_y_zero_transform_bypass_flag
	int log2_max_frame_num;            ///< log2_max_frame_num_minus4 + 4
	int poc_type;                      ///< pic_order_cnt_type
	int log2_max_poc_lsb;              ///< log2_max_pic_order_cnt_lsb_minus4
	int delta_pic_order_always_zero_flag;
	int offset_for_non_ref_pic;
	int offset_for_top_to_bottom_field;
	int poc_cycle_length;              ///< num_ref_frames_in_pic_order_cnt_cycle
	int ref_frame_count;               ///< num_ref_frames
	int gaps_in_frame_num_allowed_flag;
	int mb_width;                      ///< pic_width_in_mbs_minus1 + 1
	int mb_height;                     ///< pic_height_in_map_units_minus1 + 1
	int frame_mbs_only_flag;
	int mb_aff;                        ///< mb_adaptive_frame_field_flag
	int direct_8x8_inference_flag;
	int crop;                          ///< frame_cropping_flag

	/* those 4 are already in luma samples */
	unsigned int crop_left;            ///< frame_cropping_rect_left_offset
	unsigned int crop_right;           ///< frame_cropping_rect_right_offset
	unsigned int crop_top;             ///< frame_cropping_rect_top_offset
	unsigned int crop_bottom;          ///< frame_cropping_rect_bottom_offset
	int vui_parameters_present_flag;
	Rational sar;
	int video_signal_type_present_flag;
	int full_range;
	int colour_description_present_flag;
	enum ColorPrimaries color_primaries;
	enum ColorTransferCharacteristic color_trc;
	enum ColorSpace colorspace;
	int timing_info_present_flag;
	uint32_t num_units_in_tick;
	uint32_t time_scale;
	int fixed_frame_rate_flag;
	short offset_for_ref_frame[256]; // FIXME dyn aloc?
	int bitstream_restriction_flag;
	int num_reorder_frames;
	int scaling_matrix_present;
	uint8_t scaling_matrix4[6][16];
	uint8_t scaling_matrix8[6][64];
	int nal_hrd_parameters_present_flag;
	int vcl_hrd_parameters_present_flag;
	int pic_struct_present_flag;
	int time_offset_length;
	int cpb_cnt;                          ///< See H.264 E.1.2
	int initial_cpb_removal_delay_length; ///< initial_cpb_removal_delay_length_minus1 + 1
	int cpb_removal_delay_length;         ///< cpb_removal_delay_length_minus1 + 1
	int dpb_output_delay_length;          ///< dpb_output_delay_length_minus1 + 1
	int bit_depth_luma;                   ///< bit_depth_luma_minus8 + 8
	int bit_depth_chroma;                 ///< bit_depth_chroma_minus8 + 8
	int residual_color_transform_flag;    ///< residual_colour_transform_flag
	int constraint_set_flags;             ///< constraint_set[0-3]_flag
} SPS;

/**
* Picture parameter set
*/
typedef struct PPS {
	unsigned int sps_id;
	int cabac;                  ///< entropy_coding_mode_flag
	int pic_order_present;      ///< pic_order_present_flag
	int slice_group_count;      ///< num_slice_groups_minus1 + 1
	int mb_slice_group_map_type;
	unsigned int ref_count[2];  ///< num_ref_idx_l0/1_active_minus1 + 1
	int weighted_pred;          ///< weighted_pred_flag
	int weighted_bipred_idc;
	int init_qp;                ///< pic_init_qp_minus26 + 26
	int init_qs;                ///< pic_init_qs_minus26 + 26
	int chroma_qp_index_offset[2];
	int deblocking_filter_parameters_present; ///< deblocking_filter_parameters_present_flag
	int constrained_intra_pred;     ///< constrained_intra_pred_flag
	int redundant_pic_cnt_present;  ///< redundant_pic_cnt_present_flag
	int transform_8x8_mode;         ///< transform_8x8_mode_flag
	uint8_t scaling_matrix4[6][16];
	uint8_t scaling_matrix8[6][64];
	uint8_t chroma_qp_table[2][QP_MAX_NUM + 1];  ///< pre-scaled (with chroma_qp_index_offset) version of qp_table
	int chroma_qp_diff;
} PPS;

typedef struct Slice {
	uint8_t first_mb_in_slice;
	uint32_t slice_type;                  ///< entropy_coding_mode_flag
	uint8_t pic_parameter_set_id;      ///< pic_order_present_flag
	int frame_num;      ///< num_slice_groups_minus1 + 1
	uint8_t pic_order_cnt_lsb;
	

	//
	int pict_type;
} Slice;

extern uint8_t* avc_find_startcode(const uint8_t* inp, int size, int* nal_header_size);

extern int h264_decode_seq_parameter_set(uint8_t * buf, int size, SPS* sps, int ignore_truncation);
extern int h264_decode_picture_parameter_set(uint8_t * buf, int size, SPS *sps, PPS *pps, int bit_length);
extern int h264_decode_slice(uint8_t * buf, int size, Slice *slice, int log2_max_frame_num);

// note - must free string
extern char *h264_get_profile_level_string(const uint8_t profile,
	const uint8_t level);

#define ARRAY_ELEMS(a) (sizeof(a) / sizeof((a)[0]))
static const Rational h264_pixel_aspect[17] = {
	{ 0, 1 },
	{   1,  1 },
	{  12, 11 },
	{ 10, 11 },
	{ 16, 11 },
	{ 40, 33 },
	{ 24, 11 },
	{ 20, 11 },
	{ 32, 11 },
	{ 80, 33 },
	{ 18, 11 },
	{ 15, 11 },
	{ 64, 33 },
	{ 160, 99 },
	{ 4, 3 },
	{ 3, 2 },
	{ 2, 1 },
};

#ifdef __cplusplus
}
#endif

#endif