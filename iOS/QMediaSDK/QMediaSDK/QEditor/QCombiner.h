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

@interface QCombiner : NSObject <Serializable>

@property (nonatomic) id<QVideoTarget> videoTarget;
@property (nonatomic) id<QAudioTarget> audioTarget;

//resource for index
@property (nonatomic, readonly) NSArray<QMediaTrack*>* subObjects;
@property (nonatomic, readonly) NSArray<QGraphicNode*>* graphicNodes;

@property (nonatomic, readonly) QGraphicNode* rootNode;
@property (nonatomic, readonly) QMediaFactory* mediaFactory;

// set the video/audio output target config
- (void)setVideoConfig:(QVideoDescribe*)describe;
- (void)setAudioConfig:(QAudioDescribe*)describe;

// will update subObjects for resource index
- (void)addMediaTrack:(QMediaTrack*)track;
- (void)removeMediaTrack:(QMediaTrack*)track;

// will update graphicNodes for resource index
- (void)addGraphicNode:(QGraphicNode*)node;
- (void)removeGraphicNode:(QGraphicNode*)node;

//add / remove graphicNode, will be execute on render thread
- (bool)attachRenderNode:(QGraphicNode*)child parent:(QGraphicNode*)parent;
- (bool)detachRenderNode:(QGraphicNode*)current;

@property (nonatomic, readonly) int64_t position;
@property (nonatomic, readonly) NSRange mediaTimeRange;


//serialize all settings, include mediaTracks, graphicNodes , render trees
- (NSDictionary*)serialize;
//init all seetings with serialize data;
- (bool)loadSerializeSettings:(NSDictionary*)settings;

@end
