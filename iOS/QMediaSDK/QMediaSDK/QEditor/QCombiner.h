//
//  QCombiner.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QMediaDescribe.h"
#import "QMediaFactory.h"
#import "QLayer.h"

@interface QDisplayLayer : QLayer
@end

@interface QCombiner : NSObject <Serializable>

@property (nonatomic) id<QVideoTarget> videoTarget;
@property (nonatomic) id<QAudioTarget> audioTarget;

//resource for index
@property (nonatomic, readonly) NSDictionary<NSString*, QMediaTrack*>* subObjects;
@property (nonatomic, readonly) NSDictionary<NSString*, QGraphicNode*>* graphicNodes;
@property (nonatomic, readonly) NSDictionary<NSString*, QGraphicNode*>* duplicateNodes;

@property (nonatomic, readonly) QDisplayLayer* rootNode;
@property (nonatomic, readonly) QMediaFactory* mediaFactory;

// set the video/audio output target config
- (void)setVideoConfig:(QVideoDescribe*)describe;
- (void)setAudioConfig:(QAudioDescribe*)describe;

// will update subObjects for resource index
- (void)addMediaTrack:(QMediaTrack*)track;
- (void)removeMediaTrack:(QMediaTrack*)track;

@property (nonatomic, readonly) int64_t position;
@property (nonatomic) QTimeRange mediaTimeRange;

//copy settings from other
- (void)copyFrom:(QCombiner*)from;

//serialize all settings, include mediaTracks, graphicNodes , render trees
- (NSDictionary*)serialize;
//init all seetings with serialize data;
- (bool)loadSerializeSettings:(NSDictionary*)settings;

@end
