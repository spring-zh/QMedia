//
//  QGraphicNode_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode.h"
#include "GraphicCore/GcRenderNode.h"
#include "GraphicCore/GcDuplicateNode.h"
#include "EffectEditor/MediaGraphicChannel.h"

@interface QGraphicNode(internal)
- (instancetype)initWithNode:(GraphicCore::RenderNodeRef)graphicNode;
@property (nonatomic, readonly) GraphicCore::RenderNodeRef native;

- (bool)addChildNodeDirect:(QGraphicNode*)childNode;

- (bool)removeChildNodeDirect:(QGraphicNode*)childNode;
@end
