//
//  QCombiner.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaDescribe.h"
#import "QMediaFactory.h"
#import "QGraphicNode.h"

@interface QCombiner : NSObject

@property (nonatomic) id<QVideoTarget> videoTarget;
@property (nonatomic) id<QAudioTarget> audioTarget;

@property (nonatomic, readonly) NSArray<QMediaTrack*>* subObjects;
@property (nonatomic, readonly) QGraphicNode* rootNode;
@property (nonatomic, readonly) QMediaFactory* mediaFactory;

// set the video/audio output target config
- (void)setVideoConfig:(QVideoDescribe*)describe;
- (void)setAudioConfig:(QAudioDescribe*)describe;

- (void)addMediaTrack:(QMediaTrack*)track;
- (void)removeMediaTrack:(QMediaTrack*)track;

@property (nonatomic, readonly) int64_t position;
@property (nonatomic, readonly) NSRange mediaTimeRange;

@end
