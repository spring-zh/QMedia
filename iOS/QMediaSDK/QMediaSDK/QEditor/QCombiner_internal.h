//
//  QCombiner.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "EffectEditor/EffectCombiner.h"
#import "QCombiner.h"
#import "QLayer.h"

@interface QCombiner(internal)
- (instancetype)initWithNative:(EffectCombiner*)combiner_native;
@property (nonatomic, readonly) EffectCombiner* native;

// will update graphicNodes resource index .
// for serialize settings
- (void)addGraphicNodeIndex:(QGraphicNode*)node;
- (void)removeGraphicNodeIndex:(QGraphicNode*)node;

//add / remove graphicNode, will be execute on render thread
- (bool)attachRenderNode:(QGraphicNode*)child parent:(QGraphicNode*)parent;
- (bool)detachRenderNode:(QGraphicNode*)current;

//add / remove effect, will be execute on render thread
- (bool)attachEffect:(QLayer*)layer effect:(QEffect*)effect;
- (bool)detachEffect:(QLayer*)layer effect:(QEffect*)effect;

//add / remove audioNode, will be execute on audio thread
- (bool)attachAudioNode:(QAudioTrackNode*)child parent:(QAudioTrackNode*)parent;
- (bool)detachAudioNode:(QAudioTrackNode*)current;

@end
