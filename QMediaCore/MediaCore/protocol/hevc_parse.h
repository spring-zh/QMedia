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

/* NAL unit types */
enum NALUnitType {
	HEVC_NAL_TRAIL_N = 0,
	HEVC_NAL_TRAIL_R = 1,
	HEVC_NAL_TSA_N = 2,
	HEVC_NAL_TSA_R = 3,
	HEVC_NAL_STSA_N = 4,
	HEVC_NAL_STSA_R = 5,
	HEVC_NAL_RADL_N = 6,
	HEVC_NAL_RADL_R = 7,
	HEVC_NAL_RASL_N = 8,
	HEVC_NAL_RASL_R = 9,
	HEVC_NAL_BLA_W_LP = 16,
	HEVC_NAL_BLA_W_RADL = 17,
	HEVC_NAL_BLA_N_LP = 18,
	HEVC_NAL_IDR_W_RADL = 19,
	HEVC_NAL_IDR_N_LP = 20,
	HEVC_NAL_CRA_NUT = 21,
	HEVC_NAL_VPS = 32,
	HEVC_NAL_SPS = 33,
	HEVC_NAL_PPS = 34,
	HEVC_NAL_AUD = 35,
	HEVC_NAL_EOS_NUT = 36,
	HEVC_NAL_EOB_NUT = 37,
	HEVC_NAL_FD_NUT = 38,
	HEVC_NAL_SEI_PREFIX = 39,
	HEVC_NAL_SEI_SUFFIX = 40,
};

#define MAX_SPATIAL_SEGMENTATION 4096 // max. value of u(12) field

typedef struct HVCCNALUnitArray {
	uint8_t  array_completeness;
	uint8_t  NAL_unit_type;
	uint16_t numNalus;
	uint16_t *nalUnitLength;
	uint8_t  **nalUnit;
} HVCCNALUnitArray;

typedef struct HEVCDecoderConfigurationRecord {
	uint8_t  configurationVersion;
	uint8_t  general_profile_space;
	uint8_t  general_tier_flag;
	uint8_t  general_profile_idc;
	uint32_t general_profile_compatibility_flags;
	uint64_t general_constraint_indicator_flags;
	uint8_t  general_level_idc;
	uint16_t min_spatial_segmentation_idc;
	uint8_t  parallelismType;
	uint8_t  chromaFormat;
	uint8_t  bitDepthLumaMinus8;
	uint8_t  bitDepthChromaMinus8;
	uint16_t avgFrameRate;
	uint8_t  constantFrameRate;
	uint8_t  numTemporalLayers;
	uint8_t  temporalIdNested;
	uint8_t  lengthSizeMinusOne;
	uint8_t  numOfArrays;
	HVCCNALUnitArray *array;
} HEVCDecoderConfigurationRecord;

typedef struct HVCCProfileTierLevel {
	uint8_t  profile_space;
	uint8_t  tier_flag;
	uint8_t  profile_idc;
	uint32_t profile_compatibility_flags;
	uint64_t constraint_indicator_flags;
	uint8_t  level_idc;
} HVCCProfileTierLevel;


//extern int hvcc_parse_sps(GetBitContext *gb,HEVCDecoderConfigurationRecord *hvcc);
//extern int hvcc_parse_pps(GetBitContext *gb,HEVCDecoderConfigurationRecord *hvcc);
//extern int hvcc_parse_vps(GetBitContext *gb,HEVCDecoderConfigurationRecord *hvcc);
extern int hvcc_add_nal_unit(uint8_t *nal_buf, uint32_t nal_size,
	int ps_array_completeness,
	HEVCDecoderConfigurationRecord *hvcc);
extern void hvcc_init(HEVCDecoderConfigurationRecord *hvcc);
extern void hvcc_close(HEVCDecoderConfigurationRecord *hvcc);


//for mp4
extern int DecoderConfiguration_to_Mp4hvcC(HEVCDecoderConfigurationRecord *hvcc, uint8_t *out_buf, uint32_t max_size, uint32_t *out_size);
extern int Mp4hvcC_to_DecoderConfiguration(uint8_t *in_buf, uint32_t in_size, HEVCDecoderConfigurationRecord *hvcc);

#ifdef __cplusplus
}
#endif

#endif