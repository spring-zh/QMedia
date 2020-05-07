//
//  QMediaFactory_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaFactory.h"
#import "QVideoTarget.h"
#import "QAudioTarget.h"
#include "MediaCore/output/VideoTarget.h"
#include "MediaCore/output/AudioTarget.h"

@interface QMediaFactory(internal)
//- (instancetype)initWithTarget:(id<QVideoTarget>)videoTarget audio:(id<QAudioTarget>)audioTarget;

- (void)setVideoTarget:(id<QVideoTarget>)videoTarget;
- (void)setAudioTarget:(id<QAudioTarget>)audioTarget;

@property (nonatomic, readonly) VideoTarget* nativeVideoTarget;
@property (nonatomic, readonly) AudioTarget* nativeAudioTarget;
@end
