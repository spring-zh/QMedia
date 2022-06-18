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
static const int REQUEST_SAMPLE_POINTS = 512;
static const int audio_delay = REQUEST_SAMPLE_POINTS * 2;

@implementation AudioInputBuffer

@synthesize audio_data = _audio_data;
@synthesize audio_size = _audio_size;

- (instancetype)initWith:(uint8_t*)data size:(int)data_size {
    if (self = [super init]) {
        _audio_data = data;
        _audio_size = data_size;
    }
    return self;
}

- (void)update:(uint8_t*)data size:(int)data_size {
    _audio_data = data;
    _audio_size = data_size;

}

@end

@interface QAudioPlayer () {
    __weak QAudioRender* _render;
    AudioQueueRef _audioQueueRef;
    AudioQueueBufferRef _audioQueueBufferRefArray[AudioQueueNumberBuffers];
    bool _isInit;
    bool _isPaused;
    bool _isStopped;
    int _sizebyte;
    int _bytePreSamplePoint;
    float _volume;

    QAudioDescription *_description;
}

@property (nonatomic, readonly) AudioInputBuffer* buffer;

@end

static void AudioQueueOuptutCallback(void * inUserData, AudioQueueRef inAQ, AudioQueueBufferRef inBuffer) {
    QAudioPlayer *audioPlayer = (__bridge QAudioPlayer *)(inUserData);
    QAudioRender* audioRender = [audioPlayer getRender];
    if (audioRender != nil)
    {
//        [audioRender onAudioRender:(uint8_t * const)inBuffer->mAudioData needBytes:inBuffer->mAudioDataByteSize wantTime:-1];
        [audioPlayer.buffer update:inBuffer->mAudioData size:(NSUInteger)inBuffer->mAudioDataByteSize];
        [audioRender OnPlayBuffer:audioPlayer.buffer sizeNeed:inBuffer->mAudioDataByteSize wantTime:-1];
    }
    
    AudioQueueEnqueueBuffer(inAQ, inBuffer, 0, NULL);
}

@implementation QAudioPlayer

@synthesize buffer = _buffer;

- (instancetype)init
{
    NSLog(@"XMAudioPlayer internalInit %@", [NSThread currentThread]);
    if (self = [super init]) {
        _isInit = false;
        _isPaused = true;
        _isStopped = true;
        _volume = 1.0f;
        _description = nil;
        _buffer = [AudioInputBuffer new];
    }
    return self;
}

- (void)dealloc
{
    [self audioUnInit];
    NSLog(@"XMAudioPlayer dealloc %@", [NSThread currentThread]);
}

- (void)setAudioDelay:(int)audioDelay {
    //TODO: change audio target's delay
}
- (int)audioDelay {
    return audio_delay;
}

- (QAudioRender*)getRender {
    return _render;
}

- (void)setAudioRender:(nullable QAudioRender *)render {
    _render = render;
}

- (BOOL)audioInit:(nonnull QAudioDescription *)desc {
    if (_isInit) {
        return true;
    }

    _description = desc;
    
//    _description.smapleFormat = QAudioDescriptionFormatS16;

    /* Get the current format */
    AudioStreamBasicDescription audio_desc;
    
    audio_desc.mSampleRate = _description.sampleRate;
    audio_desc.mFormatID = kAudioFormatLinearPCM;
    audio_desc.mFormatFlags = kLinearPCMFormatFlagIsPacked;
    audio_desc.mChannelsPerFrame = _description.nchannel;
    audio_desc.mFramesPerPacket = 1;
    
    audio_desc.mBitsPerChannel = 16;
    audio_desc.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
    
    audio_desc.mBytesPerFrame = audio_desc.mBitsPerChannel * audio_desc.mChannelsPerFrame / 8;
    audio_desc.mBytesPerPacket = audio_desc.mBytesPerFrame * audio_desc.mFramesPerPacket;
    
    /* Set the desired format */
    AudioQueueRef audioQueueRef;
    OSStatus status = AudioQueueNewOutput(&audio_desc,
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
    NSLog(@"audioInit 111");
    UInt32 propValue = 1;
    AudioQueueSetProperty(audioQueueRef, kAudioQueueProperty_EnableTimePitch, &propValue, sizeof(propValue));
    propValue = 1;
    AudioQueueSetProperty(_audioQueueRef, kAudioQueueProperty_TimePitchBypass, &propValue, sizeof(propValue));
    propValue = kAudioQueueTimePitchAlgorithm_Spectral;
    AudioQueueSetProperty(_audioQueueRef, kAudioQueueProperty_TimePitchAlgorithm, &propValue, sizeof(propValue));
    NSLog(@"audioInit 222");
    _audioQueueRef = audioQueueRef;
    _bytePreSamplePoint = audio_desc.mChannelsPerFrame * audio_desc.mBitsPerChannel/8;
    _sizebyte = REQUEST_SAMPLE_POINTS * _bytePreSamplePoint;
    for (int i = 0;i < AudioQueueNumberBuffers; i++)
    {
        AudioQueueAllocateBuffer(audioQueueRef, _sizebyte, &_audioQueueBufferRefArray[i]);
        _audioQueueBufferRefArray[i]->mAudioDataByteSize = _sizebyte;
        memset(_audioQueueBufferRefArray[i]->mAudioData, 0, _sizebyte);
        AudioQueueEnqueueBuffer(audioQueueRef, _audioQueueBufferRefArray[i], 0, NULL);
    }
    NSLog(@"audioInit 333");
    _isInit = true;
    return true;
}

- (void)audioUnInit {
    if (_isInit) {
        [self audioStop];
        
        NSLog(@"AudioQueueDispose");
        AudioQueueDispose(_audioQueueRef, false);
        //    for (int i = 0;i < kIJKAudioQueueNumberBuffers; i++) {
        //        AudioQueueFreeBuffer(_audioQueueRef, _audioQueueBufferRefArray[i]);
        //    }
        _audioQueueRef = NULL;
        _isInit = false;
    }
}

- (BOOL)audioStart {
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

- (void)audioStop {
    if (_isInit) {
        NSLog(@"Audio Render Stop Entener...");
        AudioQueueStop(_audioQueueRef, true);
        _isStopped = true;
    }
}

- (void)audioPause:(BOOL)bpause {
    if (bpause) {
        if (!_audioQueueRef) return;
        
        if (_isStopped) return;
        
        _isPaused = YES;
        OSStatus status = AudioQueuePause(_audioQueueRef);
        if (status != noErr)
            NSLog(@"AudioQueue: AudioQueuePause failed (%d)\n", (int)status);
    } else {
        if(_isPaused){
            [self audioStart];
        }
    }
}

- (void)audioFlush {
    if (!_audioQueueRef)
        return;
    
    if (_isStopped)
        return;
    
    AudioQueueFlush(_audioQueueRef);
}

- (void)setVolume:(float)volume {
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

@end
