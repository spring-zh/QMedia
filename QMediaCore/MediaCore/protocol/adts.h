#ifndef _ADTS_H
#define _ADTS_H

#include <stdint.h>
#include "bitstream.h"

#define ADTS_HEADER_SIZE 7
#define MAX_PCE_SIZE 320 ///<Maximum size of a PCE including the 3-bit ID_PCE

static const int sample_rates_table[16] = {
	96000, 88200, 64000, 48000, 44100, 32000,
	24000, 22050, 16000, 12000, 11025, 8000, 7350 , -1, -1, -1
};

static const uint8_t mpeg4audio_channels[8] = {
	0, 1, 2, 3, 4, 5, 6, 8
};

static int get_sample_rate_index(int sample_rate){
	int i;
	for (i = 0; i < 16; i++)
	{
		if (sample_rates_table[i] == sample_rate)
			break;
	}

	return i < 16 ? i : -1;
}

typedef struct ADTSContext {
	int objecttype;
	//int sample_rate_index;
	int sample_rate;
	int channel_conf;
	int pce_size;
	int apetag;
	int id3v2tag;
	uint8_t pce_data[MAX_PCE_SIZE];
} ADTSContext;


static int adts_write_frame_header(ADTSContext *ctx,
	uint8_t *buf, int size, int pce_size)
{
	PutBitContext pb;

	unsigned full_frame_size = (unsigned)ADTS_HEADER_SIZE + size + pce_size;

	int sample_rate_index = get_sample_rate_index(ctx->sample_rate);
	if (sample_rate_index == -1)
		return -1;

	init_put_bits(&pb, buf, ADTS_HEADER_SIZE);

	/* adts_fixed_header */
	put_bits(&pb, 12, 0xfff);   /* syncword */
	put_bits(&pb, 1, 0);        /* ID */
	put_bits(&pb, 2, 0);        /* layer */
	put_bits(&pb, 1, 1);        /* protection_absent */
	put_bits(&pb, 2, ctx->objecttype); /* profile_objecttype */
	put_bits(&pb, 4, sample_rate_index);
	put_bits(&pb, 1, 0);        /* private_bit */
	put_bits(&pb, 3, ctx->channel_conf); /* channel_configuration */
	put_bits(&pb, 1, 0);        /* original_copy */
	put_bits(&pb, 1, 0);        /* home */

	/* adts_variable_header */
	put_bits(&pb, 1, 0);        /* copyright_identification_bit */
	put_bits(&pb, 1, 0);        /* copyright_identification_start */
	put_bits(&pb, 13, full_frame_size); /* aac_frame_length */
	put_bits(&pb, 11, 0x7ff);   /* adts_buffer_fullness */
	put_bits(&pb, 2, 0);        /* number_of_raw_data_blocks_in_frame */

	flush_put_bits(&pb);

	return 0;
}

static int adts_decodec_header(ADTSContext *ctx,
	uint8_t *buf, int size)
{
	GetBitContext gb;
	int sample_rate_index;
	uint16_t full_frame_size;
	init_get_bits(&gb, buf, ADTS_HEADER_SIZE * 8);

	/* adts_fixed_header */
	if (get_bits(&gb, 12) != 0xfff)
		return -1;   /* syncword */
	skip_bits1(&gb);        /* ID */
	skip_bits(&gb, 2);        /* layer */
	get_bits(&gb, 1);        /* protection_absent */
	ctx->objecttype = get_bits(&gb, 2); /* profile_objecttype */
	sample_rate_index = get_bits(&gb, 4);
	skip_bits1(&gb);        /* private_bit */
	ctx->channel_conf = get_bits(&gb, 3); /* channel_configuration */
	skip_bits1(&gb);        /* original_copy */
	skip_bits1(&gb);        /* home */

	/* adts_variable_header */
	skip_bits1(&gb);        /* copyright_identification_bit */
	skip_bits1(&gb);        /* copyright_identification_start */
	full_frame_size = get_bits(&gb, 13); /* aac_frame_length */
	skip_bits(&gb, 11);   /* adts_buffer_fullness */
	get_bits(&gb, 2);        /* number_of_raw_data_blocks_in_frame */

	if (sample_rate_index < 16){
		ctx->sample_rate = sample_rates_table[sample_rate_index];
		return 0;
	}
		
	return -1;
}

typedef struct MPEG4AudioConfig {
	int object_type;
	int sampling_index;
	int sample_rate;
	int chan_config;
	int sbr;
	int channels;
	int ps;  ///< -1 implicit, 1 presence
	int frame_length_short;
} MPEG4AudioConfig;

static int mpeg4audio_get_config(MPEG4AudioConfig *c, const uint8_t *buf,
	int size, int sync_extension)
{
	GetBitContext gb;
	int sample_rate_index;
	int specific_config_bitindex, ret;

	ret = init_get_bits(&gb, buf, size * 8);
	if (ret < 0)
		return ret;

	c->object_type = get_bits(&gb, 5);
	sample_rate_index = get_bits(&gb, 4);
	c->chan_config = get_bits(&gb, 4);
	c->frame_length_short = get_bits(&gb, 1);
	skip_bits(&gb, 2);

	c->channels = c->chan_config;
	if (sample_rate_index < 16){
		c->sample_rate = sample_rates_table[sample_rate_index];
		return 0;
	}

	return -1;
}

static int mpeg4audio_set_config(MPEG4AudioConfig *c, uint8_t *buf,
	int size, int sync_extension)
{
	//GetBitContext gb;
	uint16_t aac_configure;
	int sample_rate_index = get_sample_rate_index(c->sample_rate);
	if (sample_rate_index == -1)
		return -1;
	
	c->chan_config = c->channels;

	aac_configure = ((c->object_type & 0x03) << 12)
		| ((sample_rate_index & 0x0f) << 7)
		| ((c->chan_config & 0x07) << 3);

	buf[0] = (aac_configure & 0xff00) >> 8;
	buf[1] = aac_configure & 0x00ff;
	return 2;
}

#endif