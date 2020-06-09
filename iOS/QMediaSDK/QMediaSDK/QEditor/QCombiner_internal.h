//
//  QCombiner.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QCombiner.h"
#include "EffectEditor/EffectCombiner.h"

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

//add / remove audioNode, will be execute on audio thread
- (bool)attachAudioNode:(QAudioTrackNode*)child parent:(QAudioTrackNode*)parent;
- (bool)detachAudioNode:(QAudioTrackNode*)current;

@end
