#ifndef _HEVC_PARSE_H
#define _HEVC_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
* 7.4.2.1
*/
#define MAX_SUB_LAYERS 7
#define MAX_VPS_COUNT 16
#define MAX_SPS_COUNT 32
#define MAX_PPS_COUNT 256
#define MAX_SHORT_TERM_RPS_COUNT 64
#define MAX_CU_SIZE 128


/**
 * Table 7-1 – NAL unit type codes and NAL unit type classes in
 * T-REC-H.265-201802
 */
enum HEVCNALUnitType {
    HEVC_NAL_TRAIL_N    = 0,
    HEVC_NAL_TRAIL_R    = 1,
    HEVC_NAL_TSA_N      = 2,
    HEVC_NAL_TSA_R      = 3,
    HEVC_NAL_STSA_N     = 4,
    HEVC_NAL_STSA_R     = 5,
    HEVC_NAL_RADL_N     = 6,
    HEVC_NAL_RADL_R     = 7,
    HEVC_NAL_RASL_N     = 8,
    HEVC_NAL_RASL_R     = 9,
    HEVC_NAL_VCL_N10    = 10,
    HEVC_NAL_VCL_R11    = 11,
    HEVC_NAL_VCL_N12    = 12,
    HEVC_NAL_VCL_R13    = 13,
    HEVC_NAL_VCL_N14    = 14,
    HEVC_NAL_VCL_R15    = 15,
    HEVC_NAL_BLA_W_LP   = 16,
    HEVC_NAL_BLA_W_RADL = 17,
    HEVC_NAL_BLA_N_LP   = 18,
    HEVC_NAL_IDR_W_RADL = 19,
    HEVC_NAL_IDR_N_LP   = 20,
    HEVC_NAL_CRA_NUT    = 21,
    HEVC_NAL_IRAP_VCL22 = 22,
    HEVC_NAL_IRAP_VCL23 = 23,
    HEVC_NAL_RSV_VCL24  = 24,
    HEVC_NAL_RSV_VCL25  = 25,
    HEVC_NAL_RSV_VCL26  = 26,
    HEVC_NAL_RSV_VCL27  = 27,
    HEVC_NAL_RSV_VCL28  = 28,
    HEVC_NAL_RSV_VCL29  = 29,
    HEVC_NAL_RSV_VCL30  = 30,
    HEVC_NAL_RSV_VCL31  = 31,
    HEVC_NAL_VPS        = 32,
    HEVC_NAL_SPS        = 33,
    HEVC_NAL_PPS        = 34,
    HEVC_NAL_AUD        = 35,
    HEVC_NAL_EOS_NUT    = 36,
    HEVC_NAL_EOB_NUT    = 37,
    HEVC_NAL_FD_NUT     = 38,
    HEVC_NAL_SEI_PREFIX = 39,
    HEVC_NAL_SEI_SUFFIX = 40,
    HEVC_NAL_RSV_NVCL41 = 41,
    HEVC_NAL_RSV_NVCL42 = 42,
    HEVC_NAL_RSV_NVCL43 = 43,
    HEVC_NAL_RSV_NVCL44 = 44,
    HEVC_NAL_RSV_NVCL45 = 45,
    HEVC_NAL_RSV_NVCL46 = 46,
    HEVC_NAL_RSV_NVCL47 = 47,
    HEVC_NAL_UNSPEC48   = 48,
    HEVC_NAL_UNSPEC49   = 49,
    HEVC_NAL_UNSPEC50   = 50,
    HEVC_NAL_UNSPEC51   = 51,
    HEVC_NAL_UNSPEC52   = 52,
    HEVC_NAL_UNSPEC53   = 53,
    HEVC_NAL_UNSPEC54   = 54,
    HEVC_NAL_UNSPEC55   = 55,
    HEVC_NAL_UNSPEC56   = 56,
    HEVC_NAL_UNSPEC57   = 57,
    HEVC_NAL_UNSPEC58   = 58,
    HEVC_NAL_UNSPEC59   = 59,
    HEVC_NAL_UNSPEC60   = 60,
    HEVC_NAL_UNSPEC61   = 61,
    HEVC_NAL_UNSPEC62   = 62,
    HEVC_NAL_UNSPEC63   = 63,
};

enum HEVCSliceType {
	HEVC_SLICE_B = 0,
	HEVC_SLICE_P = 1,
	HEVC_SLICE_I = 2,
};

enum {
	// 7.4.3.1: vps_max_layers_minus1 is in [0, 62].
			HEVC_MAX_LAYERS     = 63,
	// 7.4.3.1: vps_max_sub_layers_minus1 is in [0, 6].
			HEVC_MAX_SUB_LAYERS = 7,
	// 7.4.3.1: vps_num_layer_sets_minus1 is in [0, 1023].
			HEVC_MAX_LAYER_SETS = 1024,

	// 7.4.2.1: vps_video_parameter_set_id is u(4).
			HEVC_MAX_VPS_COUNT = 16,
	// 7.4.3.2.1: sps_seq_parameter_set_id is in [0, 15].
			HEVC_MAX_SPS_COUNT = 16,
	// 7.4.3.3.1: pps_pic_parameter_set_id is in [0, 63].
			HEVC_MAX_PPS_COUNT = 64,

	// A.4.2: MaxDpbSize is bounded above by 16.
			HEVC_MAX_DPB_SIZE = 16,
	// 7.4.3.1: vps_max_dec_pic_buffering_minus1[i] is in [0, MaxDpbSize - 1].
			HEVC_MAX_REFS     = HEVC_MAX_DPB_SIZE,

	// 7.4.3.2.1: num_short_term_ref_pic_sets is in [0, 64].
			HEVC_MAX_SHORT_TERM_REF_PIC_SETS = 64,
	// 7.4.3.2.1: num_long_term_ref_pics_sps is in [0, 32].
			HEVC_MAX_LONG_TERM_REF_PICS      = 32,

	// A.3: all profiles require that CtbLog2SizeY is in [4, 6].
			HEVC_MIN_LOG2_CTB_SIZE = 4,
	HEVC_MAX_LOG2_CTB_SIZE = 6,

	// E.3.2: cpb_cnt_minus1[i] is in [0, 31].
			HEVC_MAX_CPB_CNT = 32,

	// A.4.1: in table A.6 the highest level allows a MaxLumaPs of 35 651 584.
			HEVC_MAX_LUMA_PS = 35651584,
	// A.4.1: pic_width_in_luma_samples and pic_height_in_luma_samples are
	// constrained to be not greater than sqrt(MaxLumaPs * 8).  Hence height/
	// width are bounded above by sqrt(8 * 35651584) = 16888.2 samples.
			HEVC_MAX_WIDTH  = 16888,
	HEVC_MAX_HEIGHT = 16888,

	// A.4.1: table A.6 allows at most 22 tile rows for any level.
			HEVC_MAX_TILE_ROWS    = 22,
	// A.4.1: table A.6 allows at most 20 tile columns for any level.
			HEVC_MAX_TILE_COLUMNS = 20,

	// A.4.2: table A.6 allows at most 600 slice segments for any level.
			HEVC_MAX_SLICE_SEGMENTS = 600,

	// 7.4.7.1: in the worst case (tiles_enabled_flag and
	// entropy_coding_sync_enabled_flag are both set), entry points can be
	// placed at the beginning of every Ctb row in every tile, giving an
	// upper bound of (num_tile_columns_minus1 + 1) * PicHeightInCtbsY - 1.
	// Only a stream with very high resolution and perverse parameters could
	// get near that, though, so set a lower limit here with the maximum
	// possible value for 4K video (at most 135 16x16 Ctb rows).
			HEVC_MAX_ENTRY_POINT_OFFSETS = HEVC_MAX_TILE_COLUMNS * 135,
};
typedef struct ShortTermRPS {
	unsigned int num_negative_pics;
	int num_delta_pocs;
	int rps_idx_num_delta_pocs;
	int32_t delta_poc[32];
	uint8_t used[32];
} ShortTermRPS;

typedef struct LongTermRPS {
	int     poc[32];
	uint8_t used[32];
	uint8_t nb_refs;
} LongTermRPS;

typedef struct SliceHeader {
	unsigned int pps_id;

	///< address (in raster order) of the first block in the current slice segment
	unsigned int   slice_segment_addr;
	///< address (in raster order) of the first block in the current slice
	unsigned int   slice_addr;

	enum HEVCSliceType slice_type;

	int pic_order_cnt_lsb;

	uint8_t first_slice_in_pic_flag;
	uint8_t dependent_slice_segment_flag;
	uint8_t pic_output_flag;
	uint8_t colour_plane_id;

	///< RPS coded in the slice header itself is stored here
	int short_term_ref_pic_set_sps_flag;
	int short_term_ref_pic_set_size;
	ShortTermRPS slice_rps;
	const ShortTermRPS *short_term_rps;
	int long_term_ref_pic_set_size;
	LongTermRPS long_term_rps;
	unsigned int list_entry_lx[2][32];

	uint8_t rpl_modification_flag[2];
	uint8_t no_output_of_prior_pics_flag;
	uint8_t slice_temporal_mvp_enabled_flag;

	unsigned int nb_refs[2];

	uint8_t slice_sample_adaptive_offset_flag[3];
	uint8_t mvd_l1_zero_flag;

	uint8_t cabac_init_flag;
	uint8_t disable_deblocking_filter_flag; ///< slice_header_disable_deblocking_filter_flag
	uint8_t slice_loop_filter_across_slices_enabled_flag;
	uint8_t collocated_list;

	unsigned int collocated_ref_idx;

	int slice_qp_delta;
	int slice_cb_qp_offset;
	int slice_cr_qp_offset;

	uint8_t cu_chroma_qp_offset_enabled_flag;

	int beta_offset;    ///< beta_offset_div2 * 2
	int tc_offset;      ///< tc_offset_div2 * 2

	unsigned int max_num_merge_cand; ///< 5 - 5_minus_max_num_merge_cand

	unsigned *entry_point_offset;
	int * offset;
	int * size;
	int num_entry_point_offsets;

	int8_t slice_qp;

	uint8_t luma_log2_weight_denom;
	int16_t chroma_log2_weight_denom;

	int16_t luma_weight_l0[16];
	int16_t chroma_weight_l0[16][2];
	int16_t chroma_weight_l1[16][2];
	int16_t luma_weight_l1[16];

	int16_t luma_offset_l0[16];
	int16_t chroma_offset_l0[16][2];

	int16_t luma_offset_l1[16];
	int16_t chroma_offset_l1[16][2];

	int slice_ctb_addr_rs;
} SliceHeader;

typedef struct HEVCWindow {
	unsigned int left_offset;
	unsigned int right_offset;
	unsigned int top_offset;
	unsigned int bottom_offset;
} HEVCWindow;

typedef struct Rational{
    int num; ///< Numerator
    int den; ///< Denominator
} Rational;

typedef struct VUI {
	Rational sar;

	int overscan_info_present_flag;
	int overscan_appropriate_flag;

	int video_signal_type_present_flag;
	int video_format;
	int video_full_range_flag;
	int colour_description_present_flag;
	uint8_t colour_primaries;
	uint8_t transfer_characteristic;
	uint8_t matrix_coeffs;

	int chroma_loc_info_present_flag;
	int chroma_sample_loc_type_top_field;
	int chroma_sample_loc_type_bottom_field;
	int neutra_chroma_indication_flag;

	int field_seq_flag;
	int frame_field_info_present_flag;

	int default_display_window_flag;
	HEVCWindow def_disp_win;

	int vui_timing_info_present_flag;
	uint32_t vui_num_units_in_tick;
	uint32_t vui_time_scale;
	int vui_poc_proportional_to_timing_flag;
	int vui_num_ticks_poc_diff_one_minus1;
	int vui_hrd_parameters_present_flag;

	int bitstream_restriction_flag;
	int tiles_fixed_structure_flag;
	int motion_vectors_over_pic_boundaries_flag;
	int restricted_ref_pic_lists_flag;
	int min_spatial_segmentation_idc;
	int max_bytes_per_pic_denom;
	int max_bits_per_min_cu_denom;
	int log2_max_mv_length_horizontal;
	int log2_max_mv_length_vertical;
} VUI;

typedef struct PTLCommon {
	uint8_t profile_space;
	uint8_t tier_flag;
	uint8_t profile_idc;
	uint8_t profile_compatibility_flag[32];
	uint8_t level_idc;
	uint8_t progressive_source_flag;
	uint8_t interlaced_source_flag;
	uint8_t non_packed_constraint_flag;
	uint8_t frame_only_constraint_flag;
} PTLCommon;

typedef struct PTL {
	PTLCommon general_ptl;
	PTLCommon sub_layer_ptl[HEVC_MAX_SUB_LAYERS];

	uint8_t sub_layer_profile_present_flag[HEVC_MAX_SUB_LAYERS];
	uint8_t sub_layer_level_present_flag[HEVC_MAX_SUB_LAYERS];
} PTL;

typedef struct HEVCVPS {
	uint8_t vps_temporal_id_nesting_flag;
	int vps_max_layers;
	int vps_max_sub_layers; ///< vps_max_temporal_layers_minus1 + 1

	PTL ptl;
	int vps_sub_layer_ordering_info_present_flag;
	unsigned int vps_max_dec_pic_buffering[HEVC_MAX_SUB_LAYERS];
	unsigned int vps_num_reorder_pics[HEVC_MAX_SUB_LAYERS];
	unsigned int vps_max_latency_increase[HEVC_MAX_SUB_LAYERS];
	int vps_max_layer_id;
	int vps_num_layer_sets; ///< vps_num_layer_sets_minus1 + 1
	uint8_t vps_timing_info_present_flag;
	uint32_t vps_num_units_in_tick;
	uint32_t vps_time_scale;
	uint8_t vps_poc_proportional_to_timing_flag;
	int vps_num_ticks_poc_diff_one; ///< vps_num_ticks_poc_diff_one_minus1 + 1
	int vps_num_hrd_parameters;

	uint8_t data[4096];
	int data_size;
} HEVCVPS;

typedef struct ScalingList {
	/* This is a little wasteful, since sizeID 0 only needs 8 coeffs,
     * and size ID 3 only has 2 arrays, not 6. */
	uint8_t sl[4][6][64];
	uint8_t sl_dc[2][6];
} ScalingList;

typedef struct HEVCSPS {
	unsigned vps_id;
	int chroma_format_idc;
	uint8_t separate_colour_plane_flag;

	HEVCWindow output_window;

	HEVCWindow pic_conf_win;

	int bit_depth;
	int bit_depth_chroma;
	int pixel_shift;
//	enum AVPixelFormat pix_fmt;

	unsigned int log2_max_poc_lsb;
	int pcm_enabled_flag;

	int max_sub_layers;
	struct {
		int max_dec_pic_buffering;
		int num_reorder_pics;
		int max_latency_increase;
	} temporal_layer[HEVC_MAX_SUB_LAYERS];
	uint8_t temporal_id_nesting_flag;

	VUI vui;
	PTL ptl;

	uint8_t scaling_list_enable_flag;
	ScalingList scaling_list;

	unsigned int nb_st_rps;
	ShortTermRPS st_rps[HEVC_MAX_SHORT_TERM_REF_PIC_SETS];

	uint8_t amp_enabled_flag;
	uint8_t sao_enabled;

	uint8_t long_term_ref_pics_present_flag;
	uint16_t lt_ref_pic_poc_lsb_sps[HEVC_MAX_LONG_TERM_REF_PICS];
	uint8_t used_by_curr_pic_lt_sps_flag[HEVC_MAX_LONG_TERM_REF_PICS];
	uint8_t num_long_term_ref_pics_sps;

	struct {
		uint8_t bit_depth;
		uint8_t bit_depth_chroma;
		unsigned int log2_min_pcm_cb_size;
		unsigned int log2_max_pcm_cb_size;
		uint8_t loop_filter_disable_flag;
	} pcm;
	uint8_t sps_temporal_mvp_enabled_flag;
	uint8_t sps_strong_intra_smoothing_enable_flag;

	unsigned int log2_min_cb_size;
	unsigned int log2_diff_max_min_coding_block_size;
	unsigned int log2_min_tb_size;
	unsigned int log2_max_trafo_size;
	unsigned int log2_ctb_size;
	unsigned int log2_min_pu_size;

	int max_transform_hierarchy_depth_inter;
	int max_transform_hierarchy_depth_intra;

	int sps_range_extension_flag;
	int transform_skip_rotation_enabled_flag;
	int transform_skip_context_enabled_flag;
	int implicit_rdpcm_enabled_flag;
	int explicit_rdpcm_enabled_flag;
	int extended_precision_processing_flag;
	int intra_smoothing_disabled_flag;
	int high_precision_offsets_enabled_flag;
	int persistent_rice_adaptation_enabled_flag;
	int cabac_bypass_alignment_enabled_flag;

	///< coded frame dimension in various units
	int width;
	int height;
	int ctb_width;
	int ctb_height;
	int ctb_size;
	int min_cb_width;
	int min_cb_height;
	int min_tb_width;
	int min_tb_height;
	int min_pu_width;
	int min_pu_height;
	int tb_mask;

	int hshift[3];
	int vshift[3];

	int qp_bd_offset;

	uint8_t data[4096];
	int data_size;
} HEVCSPS;

typedef struct HEVCPPS {
	unsigned int sps_id; ///< seq_parameter_set_id

	uint8_t sign_data_hiding_flag;

	uint8_t cabac_init_present_flag;

	int num_ref_idx_l0_default_active; ///< num_ref_idx_l0_default_active_minus1 + 1
	int num_ref_idx_l1_default_active; ///< num_ref_idx_l1_default_active_minus1 + 1
	int pic_init_qp_minus26;

	uint8_t constrained_intra_pred_flag;
	uint8_t transform_skip_enabled_flag;

	uint8_t cu_qp_delta_enabled_flag;
	int diff_cu_qp_delta_depth;

	int cb_qp_offset;
	int cr_qp_offset;
	uint8_t pic_slice_level_chroma_qp_offsets_present_flag;
	uint8_t weighted_pred_flag;
	uint8_t weighted_bipred_flag;
	uint8_t output_flag_present_flag;
	uint8_t transquant_bypass_enable_flag;

	uint8_t dependent_slice_segments_enabled_flag;
	uint8_t tiles_enabled_flag;
	uint8_t entropy_coding_sync_enabled_flag;

	uint16_t num_tile_columns;   ///< num_tile_columns_minus1 + 1
	uint16_t num_tile_rows;      ///< num_tile_rows_minus1 + 1
	uint8_t uniform_spacing_flag;
	uint8_t loop_filter_across_tiles_enabled_flag;

	uint8_t seq_loop_filter_across_slices_enabled_flag;

	uint8_t deblocking_filter_control_present_flag;
	uint8_t deblocking_filter_override_enabled_flag;
	uint8_t disable_dbf;
	int beta_offset;    ///< beta_offset_div2 * 2
	int tc_offset;      ///< tc_offset_div2 * 2

	uint8_t scaling_list_data_present_flag;
	ScalingList scaling_list;

	uint8_t lists_modification_present_flag;
	int log2_parallel_merge_level; ///< log2_parallel_merge_level_minus2 + 2
	int num_extra_slice_header_bits;
	uint8_t slice_header_extension_present_flag;
	uint8_t log2_max_transform_skip_block_size;
	uint8_t pps_range_extensions_flag;
	uint8_t cross_component_prediction_enabled_flag;
	uint8_t chroma_qp_offset_list_enabled_flag;
	uint8_t diff_cu_chroma_qp_offset_depth;
	uint8_t chroma_qp_offset_list_len_minus1;
	int8_t  cb_qp_offset_list[6];
	int8_t  cr_qp_offset_list[6];
	uint8_t log2_sao_offset_scale_luma;
	uint8_t log2_sao_offset_scale_chroma;

	// Inferred parameters
	unsigned int *column_width;  ///< ColumnWidth
	unsigned int *row_height;    ///< RowHeight
	unsigned int *col_bd;        ///< ColBd
	unsigned int *row_bd;        ///< RowBd
	int *col_idxX;

	int *ctb_addr_rs_to_ts; ///< CtbAddrRSToTS
	int *ctb_addr_ts_to_rs; ///< CtbAddrTSToRS
	int *tile_id;           ///< TileId
	int *tile_pos_rs;       ///< TilePosRS
	int *min_tb_addr_zs;    ///< MinTbAddrZS
	int *min_tb_addr_zs_tab;///< MinTbAddrZS

	uint8_t data[4096];
	int data_size;
} HEVCPPS;

typedef struct HEVCParamSets {
	HEVCVPS *vps_list[HEVC_MAX_VPS_COUNT];
	HEVCSPS *sps_list[HEVC_MAX_SPS_COUNT];
	HEVCPPS *pps_list[HEVC_MAX_PPS_COUNT];

	/* currently active parameter sets */
	const HEVCVPS *vps;
	const HEVCSPS *sps;
	const HEVCPPS *pps;
} HEVCParamSets;

typedef struct HEVCParserContext {
    HEVCParamSets ps;
//    HEVCSEI sei;
    SliceHeader sh;

    int is_avc;
    int nal_length_size;
    int parsed_extradata;

    int poc;
    int pocTid0;

    int coded_width;
    int coded_height;
    int width;
    int height;
    int format;

    int profile;
    int level;

    int output_picture_number;
    
    uint8_t rbsp[2048];
} HEVCParserContext;

typedef struct H2645NAL {
    /**
     * NAL unit type
     */
    int type;

    /**
     * HEVC only, nuh_temporal_id_plus_1 - 1
     */
    int temporal_id;

    /**
     * H.264 only, nal_ref_idc
     */
    int ref_idc;
} H2645NAL;

HEVCParserContext * CreateHEVCParserContext();
void ReleaseHEVCParserContext(HEVCParserContext **p_ctx);
int HEVCDecodeExtradata(HEVCParserContext *ctx, const uint8_t *buf, int buf_size);
int HEVCParseNalUnits(HEVCParserContext *ctx, const uint8_t *buf, int buf_size, int is_avcc);
int HEVCParseSingleNalUnits(HEVCParserContext *ctx, const uint8_t *buf, int buf_size);

int HEVCParseNalHeader(H2645NAL *nal, const uint8_t *data, int size);
int HEVCDecodeNalVPS(const uint8_t *data, int size, HEVCParamSets *ps);
int HEVCDecodeNalSPS(const uint8_t *data, int size, HEVCParamSets *ps, int apply_defdispwin);
int HEVCDecodeNalPPS(const uint8_t *data, int size, HEVCParamSets *ps);
int HEVCParseSliceHeader(const uint8_t *data, int size, HEVCParserContext *ctx, H2645NAL* nal, SliceHeader *sh);


#ifdef __cplusplus
}
#endif

#endif
