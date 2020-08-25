//
//  VideoDecoder.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaCore/core/VideoFrame.h"
#include "MediaCore/core/CodecSpecificInfo.h"
#include "MediaCore/core/EncodedPacket.h"
#include <stdint.h>

class DecodedImageCallback {
public:
	virtual ~DecodedImageCallback() {}

	virtual int32_t Decoded(VideoFrame& decodedImage) = 0;
	// Provides an alternative interface that allows the decoder to specify the
	// decode time excluding waiting time for any previous pending frame to
	// return. This is necessary for breaking positive feedback in the delay
	// estimation when the decoder has a single output buffer.
	virtual int32_t Decoded(VideoFrame& decodedImage, int64_t decode_time_ms) {
		// The default implementation ignores custom decode time value.
		return Decoded(decodedImage);
	}

	virtual int32_t ReceivedDecodedFrame(const uint64_t pictureId) { return -1; }
};

class VideoDecoder {
public:
	virtual ~VideoDecoder() {}

	virtual int32_t InitDecode(const VideoCodecType codec_settings,
		int32_t number_of_cores, const CodecSpecificInfo& codec_specific_info) = 0;


	virtual int32_t Decode(const EncodedPacket& input_image,
		int64_t render_time_ms = -1) = 0;

	virtual int32_t Flush() = 0;

	virtual int32_t RegisterDecodeCompleteCallback(
		DecodedImageCallback* callback) = 0;

	virtual int32_t Release() = 0;

	// Returns true if the decoder prefer to decode frames late.
	// That is, it can not decode infinite number of frames before the decoded
	// frame is consumed.
	virtual bool PrefersLateDecoding() const { return true; }

	virtual const char* ImplementationName() const { return "unknown"; }
};

class VideoDecoderFactory {
public:
	// Caller takes the ownership of the returned object and it should be released
	// by calling DestroyVideoDecoder().
	virtual VideoDecoder* CreateVideoDecoder(
		VideoCodecType type) = 0;
	virtual ~VideoDecoderFactory() {}

	virtual void DestroyVideoDecoder(VideoDecoder* decoder) = 0;

	// Returns a list of supported codecs in order of preference.
	virtual const std::list<VideoCodecType>& supported_codecs() const {
		return codecs_list_;
	}

	virtual bool isSupportCodecType(VideoCodecType codecType) const {
		for (auto & codectype : codecs_list_) {
			if (codectype == codecType)
				return true;
		}
		return false;
	}

private:
	mutable std::list<VideoCodecType> codecs_list_;
};
