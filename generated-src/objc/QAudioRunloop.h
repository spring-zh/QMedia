// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from audio_runloop.djinni

#import "QAudioDescription.h"
#import <Foundation/Foundation.h>
@class QAudioRender;


@protocol QAudioRunloop

- (void)setAudioRender:(nullable QAudioRender *)render;

- (BOOL)audioInit:(nonnull QAudioDescription *)desc;

- (BOOL)audioStart;

- (void)audioStop;

- (void)audioPause:(BOOL)bpause;

- (void)audioFlush;

- (void)setVolume:(float)volume;

- (float)getVolume;

@end