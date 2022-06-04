//
//  Decoder.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include "MediaCore/core/DecodedFrame.h"
#include "MediaCore/core/CodecSpecificInfo.h"
#include "MediaCore/core/EncodedPacket.h"
#include <stdint.h>

enum DecodeErr : int {
    DEC_OK = 0,
    DEC_UNKNOW = -1,
    DEC_INVALID_DECODER = -2,
    DEC_CONFIG_NOT_FOUND = -3,
    DEC_INVALID_DATA = -4,
    DEC_EAGAIN = -5,
};

class DecodedFrameCallback {
public:
	virtual ~DecodedFrameCallback() {}

	virtual int32_t OnDecoded(DecodedFrame& decodedFrame) = 0;
	// Provides an alternative interface that allows the decoder to specify the
	// decode time excluding waiting time for any previous pending frame to
	// return. This is necessary for breaking positive feedback in the delay
	// estimation when the decoder has a single output buffer.
	virtual int32_t OnDecoded(DecodedFrame& decodedFrame, int64_t decode_time_ms) {
		// The default implementation ignores custom decode time value.
		return OnDecoded(decodedFrame);
	}

	virtual void OnDecodedEnd() = 0;

	virtual int32_t ReceivedDecodedFrame(const uint64_t pictureId) { return -1; }
};

class Decoder {
public:
    using UPtr = std::unique_ptr<Decoder>;
    using SPtr = std::shared_ptr<Decoder>;
    
    
	enum RetCode : int32_t {
		ok = 0,
		unknow = -1,
		busy = -2,
		end = -3
	};

	virtual ~Decoder() {}

	virtual int32_t InitDecode(const MediaDescribe& media_describe,
									int32_t number_of_cores, const CodecSpecificInfo& codec_specific_info) = 0;

	/**
	 * @param input_packet
	 * @return RetCode
	 */
	virtual int32_t Decode(const EncodedPacket& input_packet) = 0;

	virtual int32_t Flush() = 0;

	virtual void RegisterDecodedFrameCallback(
            DecodedFrameCallback* callback) = 0;

	virtual int32_t Release() = 0;

	virtual const char* ImplementationName() const { return "unknown"; }

	virtual void setOutputRequest(const MediaDescribe& mediaDescribe) = 0;
};

class DecoderFactory {
public:
	// Caller takes the ownership of the returned object and it should be released
	// by calling DestroyDecoder().
	virtual Decoder* CreateDecoder(
			const MediaDescribe& mediaDescribe) = 0;

	virtual ~DecoderFactory() {}

	virtual void DestroyDecoder(Decoder* decoder) = 0;

	// Returns a list of supported codecs in order of preference.
	virtual const std::list<VideoCodecType>& supported_video_codecs() const {
		return video_codecs_list_;
	}

    virtual const std::list<AudioCodecType>& supported_audio_codecs() const {
        return audio_codecs_list_;
    }

	virtual bool isSupportVideoCodecType(VideoCodecType codecType) const {
		for (auto & codectype : video_codecs_list_) {
			if (codectype == codecType)
				return true;
		}
		return false;
	}

    virtual bool isSupportAudioCodecType(AudioCodecType codecType) const {
        for (auto & codectype : audio_codecs_list_) {
            if (codectype == codecType)
                return true;
        }
        return false;
    }

protected:
    std::list<VideoCodecType> video_codecs_list_;
    std::list<AudioCodecType> audio_codecs_list_;
};
