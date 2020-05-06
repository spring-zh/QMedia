//
//  QAudioFrame.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#pragma once

#include <CoreMedia/CoreMedia.h>
#include "MediaCore/mediadef/AudioFrame.h"

//AudioFrameBuffer implemention
class BlockFrameBuffer: public AudioFrameBuffer {
public:
    explicit BlockFrameBuffer(CMSampleBufferRef sampleBuffer);
    ~BlockFrameBuffer() ;
    
    virtual int Channels() const override ;
    virtual int SampleRate() const override ;
    virtual int SampleBits() const override ;
    virtual int Samples() const override ;

    virtual const uint8_t* Data() const override ;
    virtual uint32_t Size() const override ;

    virtual enum ChannelLayout ChannelLayout() const override ;

    // Returns a new memory-backed frame buffer converted from this buffer's
    // native handle.
    virtual std::shared_ptr<AudioFrameBuffer> ReSampleBuffer(RawAudioFormat fmt, int samplerate, int channels) override ;
    
    int64_t _time_stamp;
    
protected:
    CMBlockBufferRef _blockBuffer;
    
    //audio paraments
    int _channel;
    int _sampleRate;
    int _samplesInFrame;
    int _sampleBits;
    int _size;
};
