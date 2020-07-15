//
//  QGraphicNode_internal.h
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#import "QGraphicNode.h"
#import "QCombiner.h"
#include "GraphicCore/GcRenderNode.h"
#include "GraphicCore/GcDuplicateNode.h"
#include "EffectEditor/MediaGraphicChannel.h"

@interface QGraphicNode()
@property (nonatomic, assign) QTimeRange origin_renderRange;
@property (nonatomic, assign) bool origin_visible;
//@property (nonatomic, assign) float origin_rotation;
@property (nonatomic, assign) QVector origin_rotation3d;
@property (nonatomic, assign) float origin_scaleX;
@property (nonatomic, assign) float origin_scaleY;
@property (nonatomic, assign) float origin_scaleZ;

@property (nonatomic, assign) CGSize origin_contentSize;
@property (nonatomic, assign) CGPoint origin_position;
@property (nonatomic, assign) float origin_positionZ;
@property (nonatomic, assign) CGPoint origin_anchorPoint;
@property (nonatomic, assign) QColor4 origin_color4;
//@property (nonatomic, assign) float origin_alpha;
@property (nonatomic, assign) QVector origin_crop;
@property (nonatomic, assign) QBlendFunc origin_blendFunc;
@end

@interface QGraphicNode(internal)
- (instancetype)initWithNode:(GraphicCore::RenderNodeRef)graphicNode combiner:(QCombiner*)combiner;
- (instancetype)initWithNode:(GraphicCore::RenderNodeRef)graphicNode combiner:(QCombiner*)combiner uid:(NSString*)uid;
@property (nonatomic, readonly) GraphicCore::RenderNodeRef native;
@property (readonly, weak) QCombiner* combiner;

@end
