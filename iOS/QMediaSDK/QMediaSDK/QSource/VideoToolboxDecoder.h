//
//  VideoToolboxDecoder.h
//  QMediaCore
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#ifndef _VIDEO_TOOLBOX_DECODER_H_
#define _VIDEO_TOOLBOX_DECODER_H_

#include <list>
#include "MediaCore/core/MediaComm.h"
#include "MediaCore/core/CodecSpecificInfo.h"
#include "MediaCore/decoder/Decoder.h"

#include <VideoToolbox/VideoToolbox.h>

class VTFrameBuffer: public DecodedFrameBuffer {
public:
    
    explicit VTFrameBuffer(CVImageBufferRef _Nonnull imageBuffer, CMTime presentationTimeStamp);
    ~VTFrameBuffer();
    
    // for audio
    int Channels() const override;
    int SampleRate() const override;
    int SampleBits() const override;
    int Samples() const override;
    const uint8_t *Data() const override;
    uint32_t Size() const override;
    enum ChannelLayout ChannelLayout() const override;
    // -------------------------------------

    virtual int width() const override ;
    virtual int height() const override ;

    virtual const uint8_t* DataY() const override ;
    virtual const uint8_t* DataU() const override ;
    virtual const uint8_t* DataV() const override ;

    virtual int StrideY() const override ;
    virtual int StrideU() const override ;
    virtual int StrideV() const override ;

    virtual void* native_handle() const override ;

    std::shared_ptr<AudioFrameBuffer>
    ReSampleBuffer(RawAudioFormat fmt, int samplerate, int channels) override { return nullptr; }
    virtual std::shared_ptr<VideoFrameBuffer> NativeToI420Buffer() override ;
    
    virtual MediaType getMediaType() const override {
        return MediaType::Video;
    }
    virtual int getFlags() const override {
        return pkt_flag;
    }
    
    int64_t _time_stamp;
    int _data_size;
    CMPixelFormatType _formatType;
    
    int pkt_flag = 0;
    
protected:
//    CMSampleBufferRef _sampleBuffer;
    CVImageBufferRef _imageBuffer;
    int _width;
    int _heigth;
};

class VideoToolboxDecoder : public Decoder {
public:
    VideoToolboxDecoder();
    virtual ~VideoToolboxDecoder();

	virtual int32_t InitDecode(const MediaDescribe& media_describe,
							   int32_t number_of_cores, const CodecSpecificInfo& codec_specific_info) override ;

	virtual int32_t Decode(const EncodedPacket &input_packet) override ;

	virtual int32_t Flush(bool wait_completed) override ;

	virtual void RegisterDecodedFrameCallback(DecodedFrameCallback *callback) override {callback_ = callback; }

	virtual int32_t Release() override ;

	virtual void setOutputRequest(const MediaDescribe& mediaDescribe)  override ;

protected:
    void DecompressionOutputCallback(
         void* CM_NULLABLE sourceFrameRefCon,
         OSStatus status,
         VTDecodeInfoFlags infoFlags,
         CM_NULLABLE CVImageBufferRef imageBuffer,
         CMTime presentationTimeStamp,
         CMTime presentationDuration);
    static void __output_callback(
         void* CM_NULLABLE decompressionOutputRefCon,
         void* CM_NULLABLE sourceFrameRefCon,
         OSStatus status,
         VTDecodeInfoFlags infoFlags,
         CM_NULLABLE CVImageBufferRef imageBuffer,
         CMTime presentationTimeStamp,
         CMTime presentationDuration);
private:
    CMVideoFormatDescriptionRef CreateVideoFormatDescriptionRef(const VideoDescribe& video_describe, const CodecSpecificInfo& codec_specific_info);
    VTDecompressionSessionRef CreateVTbSession(int pixelFormatType, int width, int height);
    VTDecompressionSessionRef _Nullable session_ = nullptr;
    OSType pixelFormatType_ = kCVPixelFormatType_420YpCbCr8BiPlanarFullRange;
    CMVideoFormatDescriptionRef _Nullable format_desc_ = nullptr;
    bool enable_async_decode = false;
    bool is_hevc_ = false;
    bool is_avcc_;
    DecodedFrameCallback *callback_ = nullptr;
};

#endif //_VIDEO_TOOLBOX_DECODER_H_
