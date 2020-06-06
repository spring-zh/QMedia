//
//  AudioDecoder.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaCore/mediadef/AudioFrame.h"
#include "MediaCore/mediadef/CodecSpecificInfo.h"
#include "MediaCore/mediadef/EncodedPacket.h"
#include <stdint.h>

class DecodedSampleCallback {
public:
	virtual ~DecodedSampleCallback() {}

	virtual int32_t Decoded(AudioFrame& decodedSample) = 0;

	virtual int32_t ReceivedDecodedFrame(const uint64_t sampleId) { return -1; }
};

class AudioDecoder {
public:
	virtual ~AudioDecoder() {}

	virtual int32_t InitDecode(const AudioCodecType codec_settings,
		int32_t number_of_cores, const CodecSpecificInfo& codec_specific_info) = 0;


	virtual int32_t Decode(const EncodedPacket& input_image,
		int64_t render_time_ms = -1) = 0;

	virtual int32_t Flush() = 0;

	virtual int32_t RegisterDecodeCompleteCallback(
		DecodedSampleCallback* callback) = 0;

	virtual int32_t Release() = 0;

	// Returns true if the decoder prefer to decode frames late.
	// That is, it can not decode infinite number of frames before the decoded
	// frame is consumed.
	virtual bool PrefersLateDecoding() const { return true; }

	virtual const char* ImplementationName() const { return "unknown"; }
};

class AudioDecoderFactory {
public:
	// Caller takes the ownership of the returned object and it should be released
	// by calling DestroyVideoDecoder().
	virtual AudioDecoder* CreateAudioDecoder(
		VideoCodecType type) = 0;
	virtual ~AudioDecoderFactory() {}

	virtual void DestroyAudioDecoder(AudioDecoder* decoder) = 0;

	// Returns a list of supported codecs in order of preference.
	virtual const std::vector<AudioCodecType>& supported_codecs() const {
		return codecs_list_;
	}

private:
	mutable std::vector<AudioCodecType> codecs_list_;
};
