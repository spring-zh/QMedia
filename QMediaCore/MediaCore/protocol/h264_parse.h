#ifndef _H264_PARSE_H
#define _H264_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#define MAX_SPS_COUNT          32
#define MAX_PPS_COUNT         256
#define MAX_LOG2_MAX_FRAME_NUM    (12 + 4)

#define QP_MAX_NUM (51 + 6*6)           // The maximum supported qp

/*
* Table 7-1 – NAL unit type codes, syntax element categories, and NAL unit type classes in
* T-REC-H.264-201704
*/
enum {
	H264_NAL_UNSPECIFIED     = 0,
	H264_NAL_SLICE           = 1,
	H264_NAL_DPA             = 2,
	H264_NAL_DPB             = 3,
	H264_NAL_DPC             = 4,
	H264_NAL_IDR_SLICE       = 5,
	H264_NAL_SEI             = 6,
	H264_NAL_SPS             = 7,
	H264_NAL_PPS             = 8,
	H264_NAL_AUD             = 9,
	H264_NAL_END_SEQUENCE    = 10,
	H264_NAL_END_STREAM      = 11,
	H264_NAL_FILLER_DATA     = 12,
	H264_NAL_SPS_EXT         = 13,
	H264_NAL_PREFIX          = 14,
	H264_NAL_SUB_SPS         = 15,
	H264_NAL_DPS             = 16,
	H264_NAL_RESERVED17      = 17,
	H264_NAL_RESERVED18      = 18,
	H264_NAL_AUXILIARY_SLICE = 19,
	H264_NAL_EXTEN_SLICE     = 20,
	H264_NAL_DEPTH_EXTEN_SLICE = 21,
	H264_NAL_RESERVED22      = 22,
	H264_NAL_RESERVED23      = 23,
	H264_NAL_UNSPECIFIED24   = 24,
	H264_NAL_UNSPECIFIED25   = 25,
	H264_NAL_UNSPECIFIED26   = 26,
	H264_NAL_UNSPECIFIED27   = 27,
	H264_NAL_UNSPECIFIED28   = 28,
	H264_NAL_UNSPECIFIED29   = 29,
	H264_NAL_UNSPECIFIED30   = 30,
	H264_NAL_UNSPECIFIED31   = 31,
};

/**
  * Chromaticity coordinates of the source primaries.
  * These values match the ones defined by ISO/IEC 23001-8_2013 § 7.1.
  */
enum H264ColorPrimaries {
	H264COL_PRI_RESERVED0   = 0,
	H264COL_PRI_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 / SMPTE RP177 Annex B
	H264COL_PRI_UNSPECIFIED = 2,
	H264COL_PRI_RESERVED    = 3,
	H264COL_PRI_BT470M      = 4,  ///< also FCC Title 47 Code of Federal Regulations 73.682 (a)(20)

	H264COL_PRI_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM
	H264COL_PRI_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
	H264COL_PRI_SMPTE240M   = 7,  ///< functionally identical to above
	H264COL_PRI_FILM        = 8,  ///< colour filters using Illuminant C
	H264COL_PRI_BT2020      = 9,  ///< ITU-R BT2020
	H264COL_PRI_SMPTE428    = 10, ///< SMPTE ST 428-1 (CIE 1931 XYZ)
	H264COL_PRI_SMPTEST428_1 = H264COL_PRI_SMPTE428,
	H264COL_PRI_SMPTE431    = 11, ///< SMPTE ST 431-2 (2011) / DCI P3
	H264COL_PRI_SMPTE432    = 12, ///< SMPTE ST 432-1 (2010) / P3 D65 / Display P3
	H264COL_PRI_JEDEC_P22   = 22, ///< JEDEC P22 phosphors
	H264COL_PRI_NB                ///< Not part of ABI
};

/**
 * Color Transfer Characteristic.
 * These values match the ones defined by ISO/IEC 23001-8_2013 § 7.2.
 */
enum H264ColorTransferCharacteristic {
	H264COL_TRC_RESERVED0    = 0,
	H264COL_TRC_BT709        = 1,  ///< also ITU-R BT1361
	H264COL_TRC_UNSPECIFIED  = 2,
	H264COL_TRC_RESERVED     = 3,
	H264COL_TRC_GAMMA22      = 4,  ///< also ITU-R BT470M / ITU-R BT1700 625 PAL & SECAM
	H264COL_TRC_GAMMA28      = 5,  ///< also ITU-R BT470BG
	H264COL_TRC_SMPTE170M    = 6,  ///< also ITU-R BT601-6 525 or 625 / ITU-R BT1358 525 or 625 / ITU-R BT1700 NTSC
	H264COL_TRC_SMPTE240M    = 7,
	H264COL_TRC_LINEAR       = 8,  ///< "Linear transfer characteristics"
	H264COL_TRC_LOG          = 9,  ///< "Logarithmic transfer characteristic (100:1 range)"
	H264COL_TRC_LOG_SQRT     = 10, ///< "Logarithmic transfer characteristic (100 * Sqrt(10) : 1 range)"
	H264COL_TRC_IEC61966_2_4 = 11, ///< IEC 61966-2-4
	H264COL_TRC_BT1361_ECG   = 12, ///< ITU-R BT1361 Extended Colour Gamut
	H264COL_TRC_IEC61966_2_1 = 13, ///< IEC 61966-2-1 (sRGB or sYCC)
	H264COL_TRC_BT2020_10    = 14, ///< ITU-R BT2020 for 10-bit system
	H264COL_TRC_BT2020_12    = 15, ///< ITU-R BT2020 for 12-bit system
	H264COL_TRC_SMPTE2084    = 16, ///< SMPTE ST 2084 for 10-, 12-, 14- and 16-bit systems
	H264COL_TRC_SMPTEST2084  = H264COL_TRC_SMPTE2084,
	H264COL_TRC_SMPTE428     = 17, ///< SMPTE ST 428-1
	H264COL_TRC_SMPTEST428_1 = H264COL_TRC_SMPTE428,
	H264COL_TRC_ARIB_STD_B67 = 18, ///< ARIB STD-B67, known as "Hybrid log-gamma"
	H264COL_TRC_NB                 ///< Not part of ABI
};

/**
 * YUV colorspace type.
 * These values match the ones defined by ISO/IEC 23001-8_2013 § 7.3.
 */
enum H264ColorSpace {
	H264COL_SPC_RGB         = 0,  ///< order of coefficients is actually GBR, also IEC 61966-2-1 (sRGB)
	H264COL_SPC_BT709       = 1,  ///< also ITU-R BT1361 / IEC 61966-2-4 xvYCC709 / SMPTE RP177 Annex B
	H264COL_SPC_UNSPECIFIED = 2,
	H264COL_SPC_RESERVED    = 3,
	H264COL_SPC_FCC         = 4,  ///< FCC Title 47 Code of Federal Regulations 73.682 (a)(20)
	H264COL_SPC_BT470BG     = 5,  ///< also ITU-R BT601-6 625 / ITU-R BT1358 625 / ITU-R BT1700 625 PAL & SECAM / IEC 61966-2-4 xvYCC601
	H264COL_SPC_SMPTE170M   = 6,  ///< also ITU-R BT601-6 525 / ITU-R BT1358 525 / ITU-R BT1700 NTSC
	H264COL_SPC_SMPTE240M   = 7,  ///< functionally identical to above
	H264COL_SPC_YCGCO       = 8,  ///< Used by Dirac / VC-2 and H.264 FRext, see ITU-T SG16
	H264COL_SPC_YCOCG       = H264COL_SPC_YCGCO,
	H264COL_SPC_BT2020_NCL  = 9,  ///< ITU-R BT2020 non-constant luminance system
	H264COL_SPC_BT2020_CL   = 10, ///< ITU-R BT2020 constant luminance system
	H264COL_SPC_SMPTE2085   = 11, ///< SMPTE 2085, Y'D'zD'x
	H264COL_SPC_CHROMA_DERIVED_NCL = 12, ///< Chromaticity-derived non-constant luminance system
	H264COL_SPC_CHROMA_DERIVED_CL = 13, ///< Chromaticity-derived constant luminance system
	H264COL_SPC_ICTCP       = 14, ///< ITU-R BT.2100-0, ICtCp
	H264COL_SPC_NB                ///< Not part of ABI
};


typedef struct H264Rational{
	int num; ///< Numerator
	int den; ///< Denominator
} H264Rational;

/**
 * Sequence parameter set
 */
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
	///< (pic_height_in_map_units_minus1 + 1) * (2 - frame_mbs_only_flag)
	int mb_height;
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
	H264Rational sar;
	int video_signal_type_present_flag;
	int full_range;
	int colour_description_present_flag;
	enum H264ColorPrimaries color_primaries;
	enum H264ColorTransferCharacteristic color_trc;
	enum H264ColorSpace colorspace;
	int timing_info_present_flag;
	uint32_t num_units_in_tick;
	uint32_t time_scale;
	int fixed_frame_rate_flag;
	int32_t offset_for_ref_frame[256];
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
	uint8_t data[4096];
	size_t data_size;
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
	uint8_t chroma_qp_table[2][QP_MAX_NUM+1];  ///< pre-scaled (with chroma_qp_index_offset) version of qp_table
	int chroma_qp_diff;
	uint8_t data[4096];
	size_t data_size;

	uint32_t dequant4_buffer[6][QP_MAX_NUM + 1][16];
	uint32_t dequant8_buffer[6][QP_MAX_NUM + 1][64];
	uint32_t(*dequant4_coeff[6])[16];
	uint32_t(*dequant8_coeff[6])[64];
} PPS;

typedef struct H264ParamSets {
	SPS *sps_list[MAX_SPS_COUNT];
	PPS *pps_list[MAX_PPS_COUNT];

	/* currently active parameters sets */
	const PPS *pps;
	const SPS *sps;
} H264ParamSets;


typedef struct H264POCContext {
	int poc_lsb;
	int poc_msb;
	int delta_poc_bottom;
	int delta_poc[2];
	int frame_num;
	int prev_poc_msb;           ///< poc_msb of the last reference pic for POC type 0
	int prev_poc_lsb;           ///< poc_lsb of the last reference pic for POC type 0
	int frame_num_offset;       ///< for POC type 2
	int prev_frame_num_offset;  ///< for POC type 2
	int prev_frame_num;         ///< frame_num of the last pic for POC type 1/2
} H264POCContext;

enum H264PictureType {
	H264_PICTURE_TYPE_NONE = 0, ///< Undefined
	H264_PICTURE_TYPE_I,     ///< Intra
	H264_PICTURE_TYPE_P,     ///< Predicted
	H264_PICTURE_TYPE_B,     ///< Bi-dir predicted
	H264_PICTURE_TYPE_S,     ///< S(GMC)-VOP MPEG-4
	H264_PICTURE_TYPE_SI,    ///< Switching Intra
	H264_PICTURE_TYPE_SP,    ///< Switching Predicted
	H264_PICTURE_TYPE_BI,    ///< BI type
};

enum H264PictureStructure {
	H264_PICTURE_STRUCTURE_UNKNOWN,      //< unknown
	H264_PICTURE_STRUCTURE_TOP_FIELD,    //< coded as top field
	H264_PICTURE_STRUCTURE_BOTTOM_FIELD, //< coded as bottom field
	H264_PICTURE_STRUCTURE_FRAME,        //< coded as frame
};

typedef struct H264ParseContext {
	H264ParamSets ps;
	H264POCContext poc;
	int is_avc;
	int nal_length_size;
	int got_first;
	int picture_structure;
	uint8_t parse_history[6];
	int parse_history_count;
	int parse_last_mb;
	int64_t reference_dts;
	int last_frame_num, last_picture_structure;

	int pict_type; /* XXX: Put it back in AVCodecContext. */
	int key_frame;

	int output_picture_number;

	/**
	 * Dimensions of the decoded video intended for presentation.
	 */
	int width;
	int height;

	/**
     * Dimensions of the coded video.
     */
	int coded_width;
	int coded_height;

	int profile;
	int level;

	uint8_t rbsp[2048];
} H264ParseContext;

typedef struct H264NAL {
	/**
     * NAL unit type
     */
	int type;

	/**
     * H.264 only, nal_ref_idc
     */
	int ref_idc;
} H264NAL;

H264ParseContext * CreateH264ParserContext();
void ReleaseH264ParserContext(H264ParseContext **p_ctx);
int H264DecodeExtradata(H264ParseContext *ctx, const uint8_t *buf, int buf_size);
int H264ParseNalUnits(H264ParseContext *ctx, const uint8_t *buf, int buf_size, int is_avcc);
int H264ParseSingleNalUnits(H264ParseContext *ctx, const uint8_t *buf, int buf_size);

int H264DecodeNalSPS(const uint8_t *data, int size, H264ParamSets *ps, int ignore_truncation);
int H264DecodeNalPPS(const uint8_t *data, int size, H264ParamSets *ps, int bit_length);
int H264ParseSliceHeader(const uint8_t *data, int size, const H264NAL* nal, H264ParseContext *ctx);

#ifdef __cplusplus
}
#endif

#endif
