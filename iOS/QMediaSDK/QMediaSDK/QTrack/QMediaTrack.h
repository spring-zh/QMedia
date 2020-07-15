//
//  QMediaTrack.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaSource.h"
#import "QVideoTrackNode.h"
#import "QAudioTrackNode.h"

@class QCombiner;

@interface QMediaTrack : NSObject

- (instancetype)initWithMediaSource:(id<QMediaSource>)mediaSource;
- (bool) prepare;
- (bool) setTimeTo:(int)mSec;//set read position to designated location
- (void) stopMedia;//stop

- (bool)isPrepare;
- (float)getPlaySpeed;
- (int64_t)getSourceDuration;//source media total duration

- (bool)generateAudioTrackNode:(QCombiner*)combiner;
- (bool)generateVideoTrackNode:(QCombiner*)combiner;

@property (nonatomic, readonly) NSString* uid;
@property (nonatomic) QTimeRange sourceRange;
@property (nonatomic) QTimeRange displayRange;
@property (nonatomic) float timeScale;
@property (nonatomic) int repeatTimes;
@property (nonatomic, readonly) id<QMediaSource> source;
@property (nonatomic, readonly) QVideoTrackNode* graphic;
@property (nonatomic, readonly) QAudioTrackNode* audio;

@end
