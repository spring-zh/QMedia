//
//  QMediaTrack_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectEditor/MediaTrack.h"
#import "QMediaTrack.h"

@interface QMediaTrack(internal)
- (instancetype)initWithMediaSourceNative:(MediaSourceRef)mediaSource;
- (instancetype)initWithMediaSource:(id<QMediaSource>)mediaSource uid:(NSString*)uid;
@property (nonatomic, readonly) MediaTrackRef native;

- (bool)generateAudioTrackNode:(QCombiner*)combiner uid:(NSString*)uid;
- (bool)generateVideoTrackNode:(QCombiner*)combiner uid:(NSString*)uid;
@end
