//
//  QAudioPlayer.h
//  QMediaSDK
//
//  Created by spring on 20/05/2019.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "QAudioTarget.h"

@interface QAudioPlayer : NSObject <QAudioTarget>
- (bool)initAudio:(QAudioDescribe*)describe;
- (void)unInitAudio;
- (bool)startAudio;
- (bool)stopAudio;
- (void)pauseAudio;
- (void)resumeAudio;
- (void)flushAudio;
- (void)setVolume:(float)volume;
- (float)getVolume;
- (int)getSampleRate;
- (int)getChannels;
- (QRawAudioFormat)getFormat;

@property (nonatomic) int audioDelay;

@end
