//
//  QMediaSource_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaSource.h"
#import "QVideoTarget_internal.h"
#import "QAudioTarget_internal.h"

#include <memory>
#include "QVideoFrame.h"
#include "QAudioFrame.h"
#include "EffectEditor/MediaSource.h"

extern const struct VideoDescribe XMToVideoDescribe(QVideoDescribe* xmdesc);
extern const struct AudioDescribe XMToAudioDescribe(QAudioDescribe* xmdesc);

#pragma mark QMediaSource(object-c) to MediaSource(c++)
class MeidaSourceAdapter : public MediaSource {
public:
    explicit MeidaSourceAdapter(id<QMediaSource> source):_source(source){
        
    }
    ~MeidaSourceAdapter() {
    }
    
    virtual void setVideoTarget(const VideoTarget* videoTarget) override {}
    virtual void setAudioTarget(const AudioTarget* audioTarget) override {}
//
    const VideoTarget* getVideoTarget() const override {
        return _videoTargetProprety.get(); }
    const AudioTarget* getAudioTarget() const override {
        return _audioTargetProprety.get();
    }
    
    virtual bool load() override {
        if (!(_source.videoTarget && _source.audioTarget)) {
            return false;
        }
        
        if([_source isInit])
            return true;
        
        if([_source load]){
            _media_duration = [_source getMediaDuration];
            if (_source.videoDesc) {
                MediaDescribe mediaDescribe;
                mediaDescribe.mediatype_ = MediaType::Video;
                mediaDescribe._videodesc = XMToVideoDescribe(_source.videoDesc);
                _mediaDescs.push_back(mediaDescribe);
            }
            if (_source.audioDesc) {
                MediaDescribe mediaDescribe;
                mediaDescribe.mediatype_ = MediaType::Audio;
                mediaDescribe._audiodesc = XMToAudioDescribe(_source.audioDesc);
                _mediaDescs.push_back(mediaDescribe);
            }
            _videoTargetProprety = std::shared_ptr<VideoTargetAdapter>(new VideoTargetAdapter(_source.videoTarget));
            _audioTargetProprety = std::shared_ptr<AudioTargetAdapter>(new AudioTargetAdapter(_source.audioTarget));
            return true;
        }
        return false;
    }
    virtual void unload() override {
        return [_source unload];
    }
    
    virtual bool isInit() override {
        return [_source isInit];
    }
    
    virtual bool start(int64_t startMSec, int64_t endMSec) override {
        return (_isStarted = [_source start:startMSec end:endMSec]);
    }
    virtual void stop() override {
        [_source stop];
        _isStarted = false;
    }
    virtual bool seekTo(int64_t mSec) override {
        return [_source seekTo:mSec];
    }
    virtual bool isEOF() override {
        return [_source isEOF];
    }
    virtual VideoFrame readNextVideoFrame(int& error) override
    {
        VideoFrame videoframe = VideoFrame(nullptr,0,0);
        SampleBufferFrame sampleFrame = [_source readNextVideoFrame];
        error = sampleFrame.error;
        if (sampleFrame.error == 0) {
            videoframe = makeVideoFrame(sampleFrame.sampleBuffer,sampleFrame.invalid_pts);
            CMSampleBufferInvalidate(sampleFrame.sampleBuffer);
            CFRelease(sampleFrame.sampleBuffer);
        }
        return videoframe;
    }
    virtual AudioFrame readNextAudioFrame(int& error) override
    {
        AudioFrame audioframe = AudioFrame(nullptr,0,0);
        SampleBufferFrame sampleFrame = [_source readNextAudioFrame];
        error = sampleFrame.error;
        if (sampleFrame.error == 0) {
            audioframe = makeAudioFrame(sampleFrame.sampleBuffer,sampleFrame.invalid_pts);
            CMSampleBufferInvalidate(sampleFrame.sampleBuffer);
            CFRelease(sampleFrame.sampleBuffer);
        }
        return audioframe;
    }

    virtual VideoFrameDrawer* createVideoFrameDrawer() override {
        if (_source.video_frame_format == kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange ||
            _source.video_frame_format == kCVPixelFormatType_420YpCbCr8BiPlanarFullRange) {
            return new PixelFrameNV12Drawer(getVideoTarget());
        }else if (_source.video_frame_format == kCVPixelFormatType_32BGRA) {
            return new PixelFrameBGRADrawer(getVideoTarget());
        }
        return nullptr;
    }
    
protected:
    
    inline VideoFrame makeVideoFrame(CMSampleBufferRef sampleBuffer, bool invalid_pts) {
        PixelFrameBuffer* pixelFB = new PixelFrameBuffer(sampleBuffer);
        VideoFrame videoFrame(std::shared_ptr<VideoFrameBuffer>(pixelFB), pixelFB->_time_stamp,
                              invalid_pts? -1 : pixelFB->_time_stamp);
        return videoFrame;
    }
    
    inline AudioFrame makeAudioFrame(CMSampleBufferRef sampleBuffer, bool invalid_pts) {
        BlockFrameBuffer * blockFB = new BlockFrameBuffer(sampleBuffer);
        AudioFrame audioFrame(std::shared_ptr<AudioFrameBuffer>(blockFB), blockFB->_time_stamp,
                              invalid_pts? -1 : blockFB->_time_stamp);
        return audioFrame;
    }
    
    __weak id<QMediaSource> _source;
    
    std::shared_ptr<VideoTarget> _videoTargetProprety;
    std::shared_ptr<AudioTarget> _audioTargetProprety;
};

