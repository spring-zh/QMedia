//
//  QLayer.mm
//  QMediaSDK
//
//  Created by spring on 20/05/2017.
//  Copyright © 2017 QMedia. All rights reserved.
//

#include "GraphicCore/GcLayer.h"
#import "QLayer.h"
#import "QCombiner_internal.h"
#import "QGraphicNode_internal.h"
#import "QEffect_internal.h"

using namespace GraphicCore;

@implementation QLayer {
    QColor4 _bkColor;
    GraphicCore::LayerRef _layer;
    NSMutableArray<QEffect*>* _effects;
}

- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner
{
    CFUUIDRef uuidref = CFUUIDCreate(kCFAllocatorDefault);
    CFStringRef uuid = CFUUIDCreateString(kCFAllocatorDefault, uuidref);
    CFRelease(uuidref);
    return [self initWithSize:size combiner:combiner uid:(__bridge_transfer NSString *)uuid];
}

- (instancetype)initWithSize:(CGSize)size combiner:(QCombiner*)combiner uid:(NSString*)uid
{
    _layer = GraphicCore::LayerRef(new GraphicCore::Layer(GraphicCore::Size(size.width,size.height)));
    if(self = [super initWithNode:_layer combiner:combiner uid:uid]) {
        _bkColor = QColorMake(0, 0, 0, 0);
        _effects = [NSMutableArray new];
    }
    return self;
}

- (void)dealloc {
    NSLog(@"QLayer dealloc");
}

- (void)addEffect:(QEffect*)effect {
    [_effects addObject:effect];
    [super.combiner attachEffect:self effect:effect];
//    _layer->addEffect(effect.native);
}

- (void)removeEffect:(QEffect*)effect {
    [_effects removeObject:effect];
    [super.combiner detachEffect:self effect:effect];
//    _layer->removeEffect(effect.native);
}

- (void)removeAllEffect {
    for (QEffect* effect in _effects) {
        [super.combiner detachEffect:self effect:effect];
    }
    [_effects removeAllObjects];
}

- (NSArray<QEffect *> *)effects {
    return _effects;
}

- (CGSize)layerSize {
    return CGSizeMake(_layer->getLayerSize().width, _layer->getLayerSize().height);
}

- (QColor4)bkColor{
    GraphicCore::Color4F color = _layer->getBKColor();
    return QColorMake(color.r, color.g, color.b, color.a);
}
- (void)setBkColor:(QColor4)bkColor{
    _bkColor = bkColor;
    GraphicCore::Color4F color(bkColor.r,bkColor.g,bkColor.b,bkColor.a);
    _layer->setBKColor(color);
}

@end
