//
//  QAudioFrame.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "QAudioFrame.h"
#import <CoreFoundation/CoreFoundation.h>
#include "Utils/Comm.h"

static const int64_t kNumMillisecsPerSec = INT64_C(1000);

BlockFrameBuffer::BlockFrameBuffer(CMSampleBufferRef sampleBuffer){
    _blockBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    CFRetain(_blockBuffer);
    
    _size = (int)CMBlockBufferGetDataLength(_blockBuffer);
    
    const AudioStreamBasicDescription * audioStreamDescribe = CMAudioFormatDescriptionGetStreamBasicDescription(CMSampleBufferGetFormatDescription(sampleBuffer));
    _sampleRate = audioStreamDescribe->mSampleRate;
    _channel = audioStreamDescribe->mChannelsPerFrame;
    _sampleBits = audioStreamDescribe->mBitsPerChannel;
    if (audioStreamDescribe->mFormatFlags & kAudioFormatFlagIsFloat) {
        sample_format_ = RawAudioFormat::kFLOAT;
    }else {
        switch (_sampleBits) {
            case 8:
                sample_format_ = RawAudioFormat::kU8;
                break;
            case 16:
                if (audioStreamDescribe->mFormatFlags & kAudioFormatFlagIsSignedInteger) {
                    sample_format_ = RawAudioFormat::kS16;
                }else
                    sample_format_ = RawAudioFormat::kU16;
                break;
            case 32:
                if (audioStreamDescribe->mFormatFlags & kAudioFormatFlagIsSignedInteger) {
                    sample_format_ = RawAudioFormat::kS32;
                }else
                    sample_format_ = RawAudioFormat::kU32;
                break;
            default:
                break;
        }

    }
    
    //get timestamp
    CMTime currentSampleTime = CMSampleBufferGetOutputPresentationTimeStamp(sampleBuffer);
    _time_stamp = CMTimeGetSeconds(currentSampleTime) * kNumMillisecsPerSec;
}
BlockFrameBuffer::~BlockFrameBuffer() {
    if(_blockBuffer) {
        CFRelease(_blockBuffer);
        _blockBuffer = NULL;
    }
}
    
int BlockFrameBuffer::Channels() const { return _channel; }
int BlockFrameBuffer::SampleRate() const { return _sampleRate; }
int BlockFrameBuffer::SampleBits() const { return _sampleBits; }
int BlockFrameBuffer::Samples() const {
    return (_size * 8)/(_channel * _sampleBits);
}

 const uint8_t* BlockFrameBuffer::Data() const {
    size_t lengthAtOffsetOut;
    size_t totalLengthOut;
    char * dataPointerOut = NULL;
    CMBlockBufferGetDataPointer(_blockBuffer,0,&lengthAtOffsetOut,&totalLengthOut,&dataPointerOut);
    return (uint8_t*)dataPointerOut;
}
uint32_t BlockFrameBuffer::Size() const {
    return _size;
}

enum ChannelLayout BlockFrameBuffer::ChannelLayout() const {
    return _channel == 2 ? kStereo : kMono;
}

// Returns a new memory-backed frame buffer converted from this buffer's
// native handle.
std::shared_ptr<AudioFrameBuffer> BlockFrameBuffer::ReSampleBuffer(RawAudioFormat fmt, int samplerate, int channels) {
    return nullptr;
}
