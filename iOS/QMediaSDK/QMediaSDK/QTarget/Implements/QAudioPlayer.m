//
//  QAudioPlayer.m
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QAudioPlayer.h"
#import <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioQueue.h>

#define AudioQueueNumberBuffers (4)
const int REQUEST_SAMPLE_POINTS = 512;
const int audio_delay = REQUEST_SAMPLE_POINTS * 2;

static void AudioQueueOuptutCallback(void * inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    QAudioPlayer *audioPlayer = (__bridge QAudioPlayer *)(inUserData);
    id<QAudioRender> audioRender = audioPlayer.audioRender;
    if (audioRender != nil)
    {
        [audioRender onAudioRender:(uint8_t * const)inBuffer->mAudioData needBytes:inBuffer->mAudioDataByteSize wantTime:-1];
    }
    
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}


@interface QAudioPlayer () {
    
    AudioQueueRef _audioQueueRef;
    AudioQueueBufferRef _audioQueueBufferRefArray[AudioQueueNumberBuffers];
    bool _isInit;
    bool _isPaused;
    bool _isStopped;
    int _sizebyte;
    int _bytePreSamplePoint;
    float _volume;
    QAudioDescribe* _describe;
}

@end

@implementation QAudioPlayer

- (instancetype)init
{
    NSLog(@"XMAudioPlayer internalInit %@", [NSThread currentThread]);
    if (self = [super init]) {
        _isInit = false;
        _isPaused = true;
        _isStopped = true;
        _volume = 1.0f;
        _describe = nil;
    }
    return self;
}

- (void)dealloc
{
    [self unInitAudio];
    NSLog(@"XMAudioPlayer dealloc %@", [NSThread currentThread]);
}

@synthesize audioRender = _audioRender;

- (bool)initAudio:(QAudioDescribe*)describe{
    if (_isInit) {
        return true;
    }
    
    _describe = describe;
    
    _describe.rawFormat = QRawAudioFormatS16;

    /* Get the current format */
    AudioStreamBasicDescription desc;
    
    desc.mSampleRate = _describe.samplerate;
    desc.mFormatID = kAudioFormatLinearPCM;
    desc.mFormatFlags = kLinearPCMFormatFlagIsPacked;
    desc.mChannelsPerFrame = _describe.nchannel;
    desc.mFramesPerPacket = 1;
    
    desc.mBitsPerChannel = 16;
    desc.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
    
    desc.mBytesPerFrame = desc.mBitsPerChannel * desc.mChannelsPerFrame / 8;
    desc.mBytesPerPacket = desc.mBytesPerFrame * desc.mFramesPerPacket;
    
    /* Set the desired format */
    AudioQueueRef audioQueueRef;
    OSStatus status = AudioQueueNewOutput(&desc,
                                          AudioQueueOuptutCallback,
                                          (__bridge void*)self,
                                          NULL,
                                          kCFRunLoopCommonModes,
                                          0,
                                          &audioQueueRef);
    if (status != noErr) {
        NSLog(@"AudioQueue: AudioQueueNewOutput failed (%d)\n", (int)status);
        return false;
    }

    UInt32 propValue = 1;
    AudioQueueSetProperty(audioQueueRef, kAudioQueueProperty_EnableTimePitch, &propValue, sizeof(propValue));
    propValue = 1;
    AudioQueueSetProperty(_audioQueueRef, kAudioQueueProperty_TimePitchBypass, &propValue, sizeof(propValue));
    propValue = kAudioQueueTimePitchAlgorithm_Spectral;
    AudioQueueSetProperty(_audioQueueRef, kAudioQueueProperty_TimePitchAlgorithm, &propValue, sizeof(propValue));
    
    _audioQueueRef = audioQueueRef;
    _bytePreSamplePoint = desc.mChannelsPerFrame * desc.mBitsPerChannel/8;
    _sizebyte = REQUEST_SAMPLE_POINTS * _bytePreSamplePoint;
    for (int i = 0;i < AudioQueueNumberBuffers; i++)
    {
        AudioQueueAllocateBuffer(audioQueueRef, _sizebyte, &_audioQueueBufferRefArray[i]);
        _audioQueueBufferRefArray[i]->mAudioDataByteSize = _sizebyte;
        memset(_audioQueueBufferRefArray[i]->mAudioData, 0, _sizebyte);
        AudioQueueEnqueueBuffer(audioQueueRef, _audioQueueBufferRefArray[i], 0, NULL);
    }
    
    _isInit = true;
    return true;
}

- (void)unInitAudio {
    if (_isInit)
    {
        [self stopAudio];
        
        NSLog(@"AudioQueueDispose");
        AudioQueueDispose(_audioQueueRef, false);
        //    for (int i = 0;i < kIJKAudioQueueNumberBuffers; i++) {
        //        AudioQueueFreeBuffer(_audioQueueRef, _audioQueueBufferRefArray[i]);
        //    }
        _audioQueueRef = NULL;
        _isInit = false;
    }
}

- (bool)startAudio {
    if (!_isInit) {
        return false;
    }
    
    if (!_audioQueueRef)
        return false;
    
    NSError *error = nil;
    if (NO == [[AVAudioSession sharedInstance] setActive:YES error:&error]) {
        NSLog(@"AudioQueue: AVAudioSession.setActive(YES) failed: %@\n", error ? [error localizedDescription] : @"nil");
        return false;
    }
    _isPaused = false;
    _isStopped = false;
    OSStatus status = AudioQueueStart(_audioQueueRef, NULL);
    if (status != noErr)
        NSLog(@"AudioQueue: AudioQueueStart failed (%d)\n", (int)status);
    
    return (status == noErr);
}
- (bool)stopAudio {
    if(!_isInit)
        return false;
    NSLog(@"Audio Render Stop Entener...");
    AudioQueueStop(_audioQueueRef, true);
    _isStopped = true;
    return true;
}
- (void)pauseAudio {
    if (!_audioQueueRef)
        return;
    
    if (_isStopped)
        return;
    
    _isPaused = YES;
    OSStatus status = AudioQueuePause(_audioQueueRef);
    if (status != noErr)
        NSLog(@"AudioQueue: AudioQueuePause failed (%d)\n", (int)status);
}

- (void)resumeAudio {
    if(_isPaused){
        [self startAudio];
    }
}

- (void)flushAudio {
    if (!_audioQueueRef)
        return;
    
    if (_isStopped)
        return;
    
    AudioQueueFlush(_audioQueueRef);
}

- (void)setVolume:(float)volume
{
    if (!_audioQueueRef)
        return;
    
    if (_isStopped)
        return;
    _volume = volume;
    float aq_volume = volume;
    if (fabsf(aq_volume - 1.0f) <= 0.000001) {
        AudioQueueSetParameter(_audioQueueRef, kAudioQueueParam_Volume, 1.f);
    } else {
        AudioQueueSetParameter(_audioQueueRef, kAudioQueueParam_Volume, aq_volume);
    }
}

- (float)getVolume {
    return _volume;
}

- (int)getChannels {
    return _describe.nchannel;
}


- (QRawAudioFormat)getFormat {
    return _describe.rawFormat;
}


- (int)getSampleRate {
    return _describe.samplerate;
}

- (void)setAudioDelay:(int)audioDelay {
    //TODO: change audio target's delay
}
- (int)audioDelay {
    return audio_delay;
}

@end
