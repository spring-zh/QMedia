//
//  QAudioTarget.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "QMediaDescribe.h"

@protocol QAudioRender <NSObject>
@required
- (bool)onAudioRender:(uint8_t * const)buffer needBytes:(unsigned)needBytes wantTime:(int64_t)wantTime;
@end

@protocol QAudioTarget <NSObject>
@required
- (bool)startAudio;
- (bool)stopAudio;
- (void)pauseAudio;
- (void)resumeAudio;
- (void)flushAudio;
//for audio
- (bool)initAudio:(QAudioDescribe*)describe;
- (void)unInitAudio;
- (int)getSampleRate;
- (int)getChannels;
- (QRawAudioFormat)getFormat;
- (void)setVolume:(float)volume;
- (float)getVolume;

//audioDelay bytes
@property (nonatomic) int audioDelay;

@property (nonatomic, weak) id<QAudioRender> audioRender;

@end
