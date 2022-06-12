//
//  SoftwareDecoder.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <list>
#include "MediaCore/core/MediaComm.h"
#include "MediaCore/core/CodecSpecificInfo.h"
#include "Decoder.h"

struct AVPacket;
struct AVFrame;
struct AVCodec;
struct AVCodecContext;
struct SwrContext;
struct SwsContext;

class SoftwareEncodedPacketBuffer : public EncodedPacketBuffer {
public:
	explicit SoftwareEncodedPacketBuffer(AVPacket* packet, bool is_bframe = false);
	~SoftwareEncodedPacketBuffer();

	int64_t pts() const override;
	int64_t dts() const override;
	const uint8_t *data() const override;
	size_t size() const override;
	bool is_keyframe() const override ;
	bool bframe() const override;
	bool isCompleteFrame() const override { return true; }
	int stream_id() const override ;

private:
	AVPacket * _avPacket;
	bool _is_bframe;
};

class SoftwareDecodedFrameBuffer : public DecodedFrameBuffer{
public:
    SoftwareDecodedFrameBuffer(MediaType mediaType,AVFrame* frame);
    ~SoftwareDecodedFrameBuffer();
    // for audio
    int Channels() const override;
    int SampleRate() const override;
    int SampleBits() const override;
    int Samples() const override;
    const uint8_t *Data() const override;
    uint32_t Size() const override;
    enum ChannelLayout ChannelLayout() const override;

    // for video
    int width() const override;
    int height() const override;
    const uint8_t *DataY() const override;
    const uint8_t *DataU() const override;
    const uint8_t *DataV() const override;
    int StrideY() const override;
    int StrideU() const override;
    int StrideV() const override;

    //other
    MediaType getMediaType() const override { return _mediaType; }
    int getFlags() const override ;

    void *native_handle() const override { return _avFrame; }

    std::shared_ptr<AudioFrameBuffer>
    ReSampleBuffer(RawAudioFormat fmt, int samplerate, int channels) override { return nullptr; }

    std::shared_ptr<VideoFrameBuffer> NativeToI420Buffer() override { return nullptr; }

private:
    const MediaType _mediaType;
    AVFrame* _avFrame;
    int _sample_bits;
};

class SoftwareDecoder : public Decoder {
public:
    SoftwareDecoder();
	virtual ~SoftwareDecoder() {}

	virtual int32_t InitDecode(const MediaDescribe& media_describe,
							   int32_t number_of_cores, const CodecSpecificInfo& codec_specific_info) override ;

	virtual int32_t Decode(const EncodedPacket &input_packet) override ;

	virtual int32_t Flush(bool wait_completed) override ;

	virtual void RegisterDecodedFrameCallback(DecodedFrameCallback *callback) override {_callback = callback; }

	virtual int32_t Release() override ;

	virtual void setOutputRequest(const MediaDescribe& mediaDescribe)  override ;

protected:
    int32_t __DoReceiveFrame();
	bool __retrieveAVFrame(struct AVFrame ** pFrame);
    bool _isInit;
	MediaDescribe _mediaDescribe;

    const AVCodec * _avcodec;
    AVCodecContext * _avcontext;
    DecodedFrameCallback * _callback;

    bool _outputRequest;
	MediaDescribe _outputDescribe;
	struct SwrContext* _swrContext;
	struct SwsContext* _swsContext;
};
